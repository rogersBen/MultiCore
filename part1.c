/*
	Student Name:		Benjamin Rogers
	Student Number:		4503600
	Student Login:		bjr342
	File Description:	CSCI376 Assignment 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[]) {

	//Get filename from user
	char filename[32];
	int key = 0;

	//Get filename
	printf("\nFilename: ");
	scanf("%s", filename);

	//Get key
	printf("Key: ");
	scanf("%d", &key);

	//Create file object and open file
	FILE *fin;
	long textSize;
	char *testBuffer;

	if((fin = fopen(filename, "rb")) == NULL) {
		//File didn't open succussfully
		perror("File opening failed");
		return EXIT_FAILURE;
	}else {
		char t;

		//Output plain text
		printf("\n\nPlain text: \n");
		while ((t = fgetc(fin)) != EOF) {
			printf("%c", t);
		}

		//Navigate back to start of file
		rewind(fin);

		fseek(fin, 0L, SEEK_END);
		textSize = ftell(fin);

		//Allocate memory for text
		testBuffer = calloc(1, textSize + 1);
		if (!testBuffer) {
			fclose(fin);
			//handle_error("Memory allocation failed");
		}

		//Copy the file into the buffer
		fread(testBuffer, textSize, 1, fin);

		printf("\n\nCipher Text: \n");
		printf("%s", testBuffer);
	}


   

    fclose(fin);

	return 0;
}