/*
Student Name:		Benjamin Rogers
Student Number:		4503600
Student Login:		bjr342
File Description:	CSCI376 Assignment 2 Part 1c
*/

#define _CRT_SECURE_NO_WARNINGS
#define FILE_NAME "plaintext.txt"
#define MAX_FILE_LENGTH 30000	
#define PROGRAM_FILE "cipherC.cl"
#define KERNEL_FUNC_ENCRYPT "cipherC"
#define KERNEL_FUNC_DECRYPT "decrypt"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

//Prints error and quits
void handle_error(const char* msg) {
	perror(msg);

	/* Wait until a key is pressed before exiting */
	getchar();

	exit(1);
}

//Creates platform and then device associated with GPU or CPU
cl_device_id create_device() {

	cl_platform_id platform;
	cl_device_id dev;
	int err;

	//Get platforms
	err = clGetPlatformIDs(1, &platform, NULL);
	if (err < 0) {
		handle_error("Couldn't identify a platform");
	}

	//Get device
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
	if (err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}
	if (err < 0) {
		handle_error("Couldn't access any devices");
	}

	return dev;
}

//Build a program from file specified
cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

	//Program data structures
	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	int err;


	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
		perror("Couldn't find the program file");
	}
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle); //Calculates the size of the file
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1); //Create buffer to hold file
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle); //Read file into buffer
	fclose(program_handle);

	//Create the program
	program = clCreateProgramWithSource(ctx, 1,
		(const char**)&program_buffer, &program_size, &err);
	if (err < 0) {
		handle_error("Couldn't create the program");
	}
	free(program_buffer);

	//Build
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {
		/* Find size of log and print to std output */
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		program_log = (char*)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		getchar();
		getchar();
		exit(1);
	}

	return program;
}

int main() {
	/////////////////////////////////////////////////////////////////////////////////
	//OpenCL data structures
	cl_device_id device;
	cl_context context;
	cl_command_queue queue;
	cl_program program;
	cl_kernel kernel[2];

	//Utility data structures
	int err, aChar;
	int i = 0;
	int j = 0;
	FILE *fin; //File pointer
	unsigned int fileSize, numShift;
	int keyValues[4];

	//Global and local size
	size_t global_size = MAX_FILE_LENGTH;
	size_t local_size; //Number of shift values

	//Arrays to hold text
	char inputText[MAX_FILE_LENGTH];
	char originalText[MAX_FILE_LENGTH];
	char encryptedText[MAX_FILE_LENGTH];
	char plainText[MAX_FILE_LENGTH];
	char t;

	//Buffers
	cl_mem inputTextBuffer;
	cl_mem encryptedTextBuffer;
	cl_mem plainTextBuffer;
	cl_mem keyValuesBuffer;
	//Maybe one more?

	/////////////////////////////////////////////////////////////////////////////////

	//Get number of key values to be used
	printf("Number of key values: ");
	while(scanf("%d", &numShift) != 1 || numShift > 4 || numShift < 1) {
		if (numShift > 4 || numShift < 1) {
			printf("\nInvalid input");
		}
		while ((i = getchar()) != EOF && i != '\n');
	}
	fseek(stdin, 0, SEEK_END); //Clear buffer

	//Sets local size to the number of shift values
	local_size = numShift;

	for (int j = 0; j < numShift; j++) {
		printf("Key: ");
		while (scanf("%d", &keyValues[j]) != 1) {
			printf("Invalid input. Please input a valid integer: ");
			while ((i = getchar()) != EOF && i != '\n');
		}
	}
	fseek(stdin, 0, SEEK_END); //Clear buffer

	//Open the file
	if ((fin = fopen(FILE_NAME, "rb")) == NULL) {
		//File didn't open succussfully
		perror("File opening failed");
		return EXIT_FAILURE;
	}
	else {
		//File opened succussfully
		//Determine filesize by navigating to the end of the file
		fseek(fin, 0, SEEK_END);
		fileSize = ftell(fin);
		rewind(fin); //rewind back to start

		//Output plain text
		printf("\n\nPlain text: \n");
		while ((t = fgetc(fin)) != EOF) {
			printf("%c", t);
		}

		//Navigate back to start of file
		rewind(fin);

		//Read in a character until end of file is reached
		for (i = 1; (aChar = fgetc(fin)) != EOF; i++) {

			//If character is a letter
			if ((aChar >= 'a' && aChar <= 'z') || (aChar >= 'A' && aChar <= 'Z')) {
				inputText[i - 1] = (char)aChar; //Add to inputText
			}
			else {
				//Not a letter, maintain count
				i--;
			}
		}

		//Add eof character
		inputText[i - 1] = '\0';

		//Determine size of text to be encrypted
		fileSize = strlen(inputText);

	}

	//All information has been read by the program so it can close the file
	fclose(fin);

	/////////////////////////////////////////////////////////////////////////////////

	//Create a device and a context						
	device = create_device();
	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if (err < 0) {
		handle_error("Couldn't create a context");
	}

	//Build program
	program = build_program(context, device, PROGRAM_FILE);

	//Create kernel for encryption
	kernel[0] = clCreateKernel(program, 
		KERNEL_FUNC_ENCRYPT, //Kernel name
		&err); //Errorcode return
	if (err < 0) {
		handle_error("Couldn't create a kernel");
	}

	//Create kernel for decryption
	kernel[1] = clCreateKernel(program, 
		KERNEL_FUNC_DECRYPT, //Kernel name 
		&err); //Error code return
	if (err < 0) {
		handle_error("Couldn't create a kernel");
	}

	/////////////////////////////////////////////////////////////////////////////////

	//Create buffer for the input text
	inputTextBuffer = clCreateBuffer(context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, //flags, read only and copy memory to pointer
		sizeof(inputText), //size in bytes of the buffer memory object to be allocated
		inputText, //pointer to the buffer data
		&err); //error code return
	if (err < 0) {
		handle_error("Couldn't create inputText buffer");
	}

	//Create buffer for keyValues
	keyValuesBuffer = clCreateBuffer(context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, //flags, read only and copy memory to pointer
		sizeof(int) * 4, //size of buffer memory object to be allocated
		keyValues, //buffer data
		&err); //error code return
	if (err < 0) {
		handle_error("Couldn't create key values buffer");
	}

	//Create buffer for the encrypted text
	encryptedTextBuffer = clCreateBuffer(context, 
		CL_MEM_WRITE_ONLY, //flags, write only means will be written but not read by kernel
		sizeof(encryptedText),  //size of memory to be allocated
		NULL, //buffer data
		&err); //error code
	if (err < 0) {
		handle_error("Couldn't create encryptedText buffer");
	}

	//Set kernel arguments
	//kernel, argument index, argument size and argument value
	err = clSetKernelArg(kernel[0], 0, sizeof(unsigned int), &fileSize); //size
	err |= clSetKernelArg(kernel[0], 1, sizeof(cl_mem), &keyValuesBuffer); //key values
	err |= clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &inputTextBuffer); //input text
	err |= clSetKernelArg(kernel[0], 3, sizeof(cl_mem), &encryptedTextBuffer); //encrypted text
	if (err < 0) {
		handle_error("Couldn't set kernel arguments");
	}

	//Create command queue
	queue = clCreateCommandQueue(context, //valid openCL context
		device, //device associated with context
		0,  //properties, can specify properties for the command queue
		&err); //error code return
	if (err < 0) {
		handle_error("Couldn't create a command queue");
	}


	//Enqueue kernel
	//Uses clEnqueueNDRangeKernel as this will perform it on all of the data
	err = clEnqueueNDRangeKernel(queue, //command queue
		kernel[0], //kernel object
		1, //one-dimensional, work dimensions
		NULL, &global_size, &local_size, //global work offset, global work size, local work size
		0, NULL, NULL); //num events in wait list, event wait list, event
	if (err < 0) {
		handle_error("Couldn't enqueue the kernel");
	}

	/////////////////////////////////////////////////////////////////////////////////

	//Enqueue command to read from buffer
	err = clEnqueueReadBuffer(queue, //command queue
		encryptedTextBuffer, //buffer
		CL_TRUE, //blocking read, indicates if read operations are blovking or non
		0, sizeof(encryptedText), &encryptedText, //offset, size in bytes of data being read, pointer to buffer
		0, NULL, NULL); //event stuff
	if (err < 0) {
		handle_error("Couldn't read the buffer");
	}

	//Create buffer for encrypted text
	encryptedTextBuffer = clCreateBuffer(context, 
		CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, //flags
		sizeof(encryptedText), //size in bytes
		encryptedText, //buffer
		&err); //error code retunr
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Create buffer for plaintext
	plainTextBuffer = clCreateBuffer(context, 
		CL_MEM_WRITE_ONLY, //flags
		sizeof(plainText), //size in bytes
		NULL, //buffer
		&err); //errpr
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Set file length, 
	fileSize = strlen(encryptedText);

	//Set kernel arguments
	//kernel, argument index, argument size and argument value
	err = clSetKernelArg(kernel[1], 0, sizeof(unsigned int), &fileSize);
	err |= clSetKernelArg(kernel[1], 1, sizeof(cl_mem), &keyValuesBuffer);
	err |= clSetKernelArg(kernel[1], 2, sizeof(cl_mem), &encryptedTextBuffer);
	err |= clSetKernelArg(kernel[1], 3, sizeof(cl_mem), &plainTextBuffer);
	if (err < 0) {
		handle_error("Couldn't set kernel arguments");
	}

	//Enqueue kernel
	err = clEnqueueNDRangeKernel(queue, //command queue
		kernel[1], //kernel object
		1, //work dimensions
		NULL, &global_size, &local_size, //global work offset, global work size, local work size
		0, NULL, NULL); //event stuff
	if (err < 0) {
		handle_error("Couldn't enqueue the kernel");
	}

	/////////////////////////////////////////////////////////////////////////////////

	//Enqueue read buffer to get results
	err = clEnqueueReadBuffer(queue, //command queue
		plainTextBuffer, //buffer
		CL_TRUE, //blocking read
		0, sizeof(plainText), &plainText,  //offset, size in bytes and buffer
		0, NULL, NULL); //events
	if (err < 0) {
		handle_error("Couldn't read the buffer");
	}

	//Output results
	int space = 0;

	//Encrypted
	printf("\n\nEncrypted Text: \n");
	for (int j = 0; encryptedText[j] != '\0'; j++) {
		if (space % 5 == 0) {
			if (space != 0) {
				printf(" ");
				space = 0;
			}
		}
		printf("%c", encryptedText[j]);
		space++;
	}

	space = 0;
	//Decrypted
	printf("\n\nDecrypted text \n");
	for (int j = 0; plainText[j] != '\0'; j++) {
		if (space % 5 == 0) {
			if (space != 0) {
				printf(" ");
				space = 0;
			}
		}
		printf("%c", plainText[j]);
		space++;
	}



	/* Wait for a key press before exiting  */
	getchar();

	//Release memory
	clReleaseMemObject(inputTextBuffer);
	clReleaseMemObject(encryptedTextBuffer);
	clReleaseMemObject(plainTextBuffer);
	clReleaseKernel(kernel[0]);
	clReleaseKernel(kernel[1]);
	clReleaseCommandQueue(queue);
	clReleaseProgram(program);
	clReleaseContext(context);
	return 0;
}