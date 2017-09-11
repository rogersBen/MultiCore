/*
Student Name:		Benjamin Rogers
Student Number:		4503600
Student Login:		bjr342
File Description:	CSCI376 Assignment 2
*/

#define _CRT_SECURE_NO_WARNINGS
#define PROGRAM_FILE "cipher.cl"
#define KERNEL_FUNC "cipher"
#define ARRAY_LENGTH 20

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#ifdef MAC
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

static const char *sourceCode =
"__kernel\n"
"void caesar(__global char *in, __global char *out) {\n"
"    int num = get_global_id(0);\n"
"    out[num] = in[num] - 3;\n"
"}\n";

void handle_error(const char* msg) {
	perror(msg);

	/* Wait until a key is pressed before exiting */
	getchar();

	exit(1);
}

cl_device_id createDevice() {
	cl_platform_id platform;
	cl_device_id dev;
	int err;

	/* Identify a platform */
	err = clGetPlatformIDs(1, &platform, NULL);
	if (err < 0) {
		handle_error("Couldn't identify a platform");
	}

	/* Access a device */
	err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
	if (err == CL_DEVICE_NOT_FOUND) {
		err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
	}
	if (err < 0) {
		handle_error("Couldn't access any devices");
	}

	return dev;

}

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

	cl_program program;
	FILE *program_handle;
	char *program_buffer, *program_log;
	size_t program_size, log_size;
	int err;

	/* Read program file and place content into buffer */
	program_handle = fopen(filename, "r");
	if (program_handle == NULL) {
		handle_error("Couldn't find the program file");
	}

	//Seek to end of file and read size
	fseek(program_handle, 0, SEEK_END);
	program_size = ftell(program_handle);
	rewind(program_handle);
	program_buffer = (char*)malloc(program_size + 1);
	program_buffer[program_size] = '\0';
	fread(program_buffer, sizeof(char), program_size, program_handle);
	fclose(program_handle);

	/* Create program from file */
	program = clCreateProgramWithSource(ctx, 1, (const char**)&program_buffer, &program_size, &err);
	if (err < 0) {
		handle_error("Couldn't create the program");
	}

	free(program_buffer);

	/* Build program */
	//program is the program object
	//0 is the number of devices
	err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
	if (err < 0) {

		/* Find the size of log and print to std output */
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
		program_log = (char*)malloc(log_size + 1);
		program_log[log_size] = '\0';
		clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, log_size + 1, program_log, NULL);
		printf("%s\n", program_log);
		free(program_log);
		exit(1);
	}

	return program;
}

int main(int argc, char *argv[]) {

//////////////////////////////////////////////////////////////////////////////////////
	
	/* OpenCL data structures 
	cl_device_id device;
	cl_context context;

	cl_program program;
	cl_kernel kernel;


	
	//Declare data


	char t;
	cl_mem bufferIn;
	cl_mem bufferOut;
	cl_bool result_check = CL_TRUE;
	*/
	size_t numWorkItems = ARRAY_LENGTH; //Setting this to array length will create a work item for each member of the array
	cl_command_queue queue;
	errno_t err;
	char filename[32];
	int key = 0;

//////////////////////////////////////////////////////////////////////////////////////

	//part 1

	//Get filename
	printf("\nFilename: ");
	scanf("%s", filename);

	//Get key
	printf("Key: ");
	scanf("%d", &key);

	//Create file object and open file
	FILE *fin;
	long textSize; 
	char *textBuffer; //Used to store the text to be encrypted

	if ((err = fopen_s(&fin, filename, "r")) != 0) {
		//File didn't open succussfully
		perror("File opening failed");
		return EXIT_FAILURE;
	}
	else {
		
		//File opened succussfully
		char t;

		//Output plain text
		printf("\n\nPlain text: \n");
		while ((t = fgetc(fin)) != EOF) {
			printf("%c", t);
		}

		//Navigate back to start of file
		rewind(fin);

		//Calculate size of text for char array
		fseek(fin, 0L, SEEK_END);
		textSize = ftell(fin);

		//Navigate back to start of file again
		rewind(fin);

		//Allocate memory for text
		textBuffer = calloc(1, textSize + 1);
		if (!textBuffer) {
			fclose(fin);
			handle_error("Memory allocation failed");
		}

		//Copy file into buffer
		fread(textBuffer, textSize, 1, fin);

		printf("%s", textBuffer);

	}

///////////////////////////////////////////////////////////////////////////////////////////

	printf("Continue?");
	getchar();

//
//
//	/* Create a device and context */
//	device = createDevice();
//	//creates 1 context
//	context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
//	if (err < 0) {
//		handle_error("Couldn't create a context");
//	}
//
//	printf("\nContext done\nContinue?");
//	getchar();
//
//	/* Build the program and create the kernel */
//	//clCreateKernel takes the program object, the name of the kernel function and an error code
//	program = build_program(context, device, PROGRAM_FILE);
//	kernel = clCreateKernel(program, KERNEL_FUNC, &err);
//	if (err < 0) {
//		handle_error("Couldn't create a kernel");
//	}
//
//	printf("\nProgram and kernel done\nContinue?");
//	getchar();
//
//	/* Create a command queue */
//	queue = clCreateCommandQueue(context, device, 0, &err);
//	if (err < 0) {
//		handle_error("Couldn't create a command queue");
//	}
//
//	printf("\nQueue done\nContinue?");
//	getchar();
//
///////////////////////////////////////////////////////////////////////////////////////////
//
//	/* Create buffers */
//	//Creates from context a read only buffer the size of the char array that is being input
//	bufferIn = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(char)*ARRAY_LENGTH, NULL, &err);
//	if (err < 0) {
//		handle_error("Couldn't create buffer a");
//	}
//
//	/* Buffer for the result */
//	bufferOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(char)*ARRAY_LENGTH, NULL, &err);
//	if (err < 0) {
//		handle_error("Couldn't create result buffer");
//	}
//
//	printf("\nBuffer done\nContinue?");
//	getchar();
//
////////////////////////////////////////////////////////////////////////////////////////
//
//	//Write data to device buffer
//	//clEnququeWriteBuffer(queue, bufferIn, CL_FALSE, 0, sizeof(char)*ARRAY_LENGTH, testBuffer, 0, NULL, NULL);
//
//	/* Set buffers as arguments to the kernel */
//	err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferIn);
//	err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOut);
//	if (err < 0) {
//		handle_error("Couldn't set kernel argument");
//	}
//
//	printf("\nSet arguments done\nContinue?");
//	getchar();
//
//	//Enqueue Kernel
//	//enqueue nd range will work for however many work_units_per_kernel i have specified
//	//Have to use clEnqueueNDRangeKernel instead of ClEnqueueTask as the latter will only add first element of arrays
//	//So maybe need to use the latter instead??
//	//err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &num_of_work_items, NULL, 0, NULL, NULL);
//	err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &numWorkItems, NULL, 0, NULL, NULL);
//	if (err < 0) {
//		handle_error("Couldn't enqueue the kernel");
//	}
//
//	printf("\nEnqueue Kernel done\nContinue?");
//	getchar();
//
///////////////////////////////////////////////////////////////////////////////////////
//
//	//Enqueue command to read from bufferOut
//	err = clEnqueueReadBuffer(queue, bufferOut, CL_TRUE, 0, sizeof(char)*ARRAY_LENGTH, textBuffer, 0, NULL, NULL);
//	if (err < 0) {
//		handle_error("Couldn't read from buffer result");
//	}
//
//	printf("\nEnqueue read buffer done\nContinue?");
//	getchar();
//
////////////////////////////////////////////////////////////////////////////////////////
//
//	//Check the results
//	result_check = CL_TRUE;
//
//	if (result_check) {
//		printf("Successfully processed %d array elements", ARRAY_LENGTH);
//	}
//


//////////////////////////////////////////////////////////////////////////////////////

	//data = textBuffer
	size_t size = sizeof(char)*ARRAY_LENGTH;
	char *answer = calloc(1, size+1);

	//Initializse platforms
	cl_uint numPlatforms;
	cl_int status = clGetPlatformIDs(0, NULL, &numPlatforms);

	cl_platform_id *platforms = (cl_platform_id*)malloc(sizeof(cl_platform_id) * numPlatforms);
	status = clGetPlatformIDs(numPlatforms, platforms, NULL);

	//Pick first platform
	cl_platform_id platform = platforms[0];  

	//initialize devices
	cl_uint numDevices;
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &numDevices);

	cl_device_id *devices = (cl_device_id*)malloc(sizeof(cl_device_id) * numDevices);
	status = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);

	cl_device_id device = devices[0];

	//Create a context
	cl_context context = clCreateContext(NULL, numDevices, devices,
		NULL, NULL, &status);

	//Create a command queue
	cl_command_queue commandQueue = clCreateCommandQueue(context, device, 0, &status);

	//Create and compile program
	cl_program program = clCreateProgramWithSource(context, 1, &sourceCode, NULL, &status);

	status = clBuildProgram(program, numDevices, devices, NULL, NULL, NULL);

	//Create device buffers
	cl_mem bufferIn = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, &status);

	cl_mem bufferOut = clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, &status);


	//Write data to device buffer
	clEnqueueWriteBuffer(commandQueue, bufferIn,
		CL_FALSE,   // non-blocking
		0, size,    // elements to use
		textBuffer,       // local data source
		0, NULL, NULL);

	// --- create kernel
	cl_kernel kernel = clCreateKernel(program, "caesar", &status);


	// --- set kernel arguments
	status = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferIn);

	status = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferOut);


	//Run the kernel
	status = clEnqueueNDRangeKernel(commandQueue, kernel, 1, NULL, &numWorkItems, NULL, 0, NULL, NULL);

	// --- read results from kernel
	clEnqueueReadBuffer(commandQueue, bufferOut,
		CL_TRUE,  // blocking read
		0, size,  // offsets
		answer,
		0, NULL, NULL);

	answer[size] = 0;
	printf("%s", answer);

	fclose(fin);
	free(textBuffer);

	//Deallocate resources
	clReleaseMemObject(bufferIn);
	clReleaseMemObject(bufferOut);
	clReleaseKernel(kernel);
	clReleaseCommandQueue(commandQueue);
	clReleaseProgram(program);
	clReleaseContext(context);

	return 0;
}