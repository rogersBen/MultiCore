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

	if((fin = fopen(filename, "r")) == NULL) {
		//File didn't open succussfully
		perror("File opening failed");
		return EXIT_FAILURE;
	}else {
    	//File opened succussfully
    	char t;
    	char message[4] = "wipe";
    	int i = 0;

    	printf("\nCipher text: \n");
    	while((t = fgetc(fin)) != EOF) {
	   
	    	//Read in char from file
	    	if(t >= 'a' && t <= 'z') {
	    		//Lowercase letter
	    		t = t + key;

	    		if(t > 'z') {
	    			t = t - 'z' + 'a' - 1;
	    		}
	    		
	    	}else if (t >= 'A' && t <= 'Z') {
	    		//Uppercase letter
	    		t = t + key;

	    		if(t > 'Z') {
	    			t = t - 'Z' + 'A' - 1;
	    		}

	    	} else {
	    		//Discard
	    		t = '\0';
	    	}

	    	if(i <= 4 && i >= 0) {
	    		
	    		printf("%c", t);
	    		i++;
	    	}else {
	    		//printf(" ");
	    		i = 0;
	    	}	
	    }

	    //Navigate back to start of file
	    rewind(fin);
	    printf("\n\nPlain text: \n");
	    while((t = fgetc(fin)) != EOF) {
	    	printf("%c", t);
	    }


	}


   

    fclose(fin);

	return 0;
}