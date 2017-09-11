/*
Student Name:		Benjamin Rogers
Student Number:		4503600
Student Login:		bjr342
File Description:	CSCI376 Assignment 2 Part 1b
*/

#define _CRT_SECURE_NO_WARNINGS
#define FILE_NAME "plaintext.txt"
#define MAX_FILE_LENGTH 30000	
#define PROGRAM_FILE "cipherB.cl"
#define KERNEL_FUNC_ENCRYPT "cipherB"
#define KERNEL_FUNC_DECRYPT "decrypt"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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

	int err, aChar, key;
	int i = 0;
	int j = 0;
	FILE *fin; //File pointer
	unsigned int fileSize;

	size_t global_size = MAX_FILE_LENGTH;
	size_t local_size = 6;

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

/////////////////////////////////////////////////////////////////////////////////

	//Get key
	printf("Key: ");
	while (scanf("%d", &key) != 1) {
		while ((i = getchar()) != EOF && i != '\n');
		printf("Invalid input. Please input a valid integer: ");
	}
	fseek(stdin, 0, SEEK_END); //Clear buffer

	//Open the file
	if ((fin = fopen(FILE_NAME, "rb")) == NULL) {
		//File didn't open succussfully
		perror("File opening failed");
		return EXIT_FAILURE;
	}
	else {
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
			} else {
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
	kernel[0] = clCreateKernel(program, KERNEL_FUNC_ENCRYPT, &err);
	if (err < 0) {
		handle_error("Couldn't create a kernel");
	}

	//Create kernel for decryption
	kernel[1] = clCreateKernel(program, KERNEL_FUNC_DECRYPT, &err);
	if (err < 0) {
		handle_error("Couldn't create a kernel");
	}

/////////////////////////////////////////////////////////////////////////////////

	//Create buffer for the input text
	inputTextBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(inputText), inputText, &err);
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Create buffer for the encrypted text
	encryptedTextBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(encryptedText), NULL, &err);
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Set kernel arguments
	err = clSetKernelArg(kernel[0], 0, sizeof(unsigned int), &fileSize); //size
	err |= clSetKernelArg(kernel[0], 1, sizeof(int), &key); //key
	err |= clSetKernelArg(kernel[0], 2, sizeof(cl_mem), &inputTextBuffer); //input text
	err |= clSetKernelArg(kernel[0], 3, sizeof(cl_mem), &encryptedTextBuffer); //encrypted text
	if (err < 0) {
		handle_error("Couldn't set kernel arguments");
	}

	//Create command queue
	queue = clCreateCommandQueue(context, device, 0, &err);
	if (err < 0) {
		handle_error("Couldn't create a command queue");
	}


	//Enqueue kernel
	//Uses clEnqueueNDRangeKernel as this will perform it on all of the data
	err = clEnqueueNDRangeKernel(queue, kernel[0], 1, NULL, &global_size, &local_size, 0, NULL, NULL);
	if (err < 0) {
		handle_error("Couldn't enqueue the kernel");
	}

/////////////////////////////////////////////////////////////////////////////////
	
	//Enqueue command to read from buffer
	err = clEnqueueReadBuffer(queue, encryptedTextBuffer, CL_TRUE, 0, sizeof(encryptedText), &encryptedText, 0, NULL, NULL);
	if (err < 0) {
		handle_error("Couldn't read the buffer");
	}

	//Create buffer for encrypted text
	encryptedTextBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(encryptedText), encryptedText, &err);
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Create buffer for plaintext
	plainTextBuffer = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(plainText), NULL, &err);
	if (err < 0) {
		perror("Couldn't create a buffer");
	}

	//Set file length, 
	fileSize = strlen(encryptedText);	

	//Set kernel arguments
	err = clSetKernelArg(kernel[1], 0, sizeof(unsigned int), &fileSize);
	err |= clSetKernelArg(kernel[1], 1, sizeof(int), &key);
	err |= clSetKernelArg(kernel[1], 2, sizeof(cl_mem), &encryptedTextBuffer);
	err |= clSetKernelArg(kernel[1], 3, sizeof(cl_mem), &plainTextBuffer);
	if (err < 0) {
		handle_error("Couldn't set kernel arguments");
	}

	//Enqueue kernel
	err = clEnqueueNDRangeKernel(queue, kernel[1], 1, NULL, &global_size, &local_size, 0, NULL, NULL);
	if (err < 0) {
		handle_error("Couldn't enqueue the kernel");
	}

/////////////////////////////////////////////////////////////////////////////////

	//Enqueue read buffer to get results
	err = clEnqueueReadBuffer(queue, plainTextBuffer, CL_TRUE, 0, sizeof(plainText), &plainText, 0, NULL, NULL);
	if (err < 0) {
		handle_error("Couldn't read the buffer");
	}

	//Output results
	printf("\n\nEncrypted Text: \n");
	printf("%s", encryptedText);
	printf("\n\nDecrypted text \n");
	printf("%s\n\n", plainText);



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