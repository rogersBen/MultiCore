/*
	Student Name:		Benjamin Rogers
	Student Number:		4503600
	Student Login:		bjr342
	File Description:	CSCI376 Assignment 2
*/

#include <stdio.h>
#include <stdlib.h>


int main (int argc, char *argv[]) {

	//Get filename from user
	char filename[32];
	int key = 0;
	printf("Key: ");
	scanf("%d", &key);
	printf("\nFilename: ");
	scanf("%s", filename);
	//printf("\n%s\n", filename);

	//Create file object and open file
	FILE *fin;

	if((fin = fopen(filename, "r")) == NULL) {
		perror("File opening failed");
		return EXIT_FAILURE;
	}else {
    	//File opened succussfully
    	char t;
    	char discard;
    	while((t = fgetc(fin)) != EOF) {
	    	//printf("%c", t);
	    	//Read in char from file
	    	if(t >= 'a' && t <= 'z') {
	    		//Lowercase letter
	    		t = t + key;

	    		if(t > 'z') {
	    			t = t - 'z' + 'a' - 1;
	    		}
	    		printf("%c", t);
	    	}else if (t >= 'A' && t <= 'Z') {
	    		//Uppercase letter
	    		t = t + key;

	    		if(t > 'Z') {
	    			t = t - 'Z' + 'A' - 1;
	    		}
	    		printf("%c", t);
	    	} else if (t = ' ') {
	    		//Otherwise discard
	    		printf("%c", t);
	    	}
	    	  
		}
	}


   

    fclose(fin);

	return 0;
}