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
    	char message[4] = "Hell";
    	int i = 0;
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
	    		//Symbol
	    		t = ' ';
	    	}
	    	/*
	    	if(i >= 0 && i <= 4) {
	    		//printf("%c", t);
	    		message[i] = t;
	    		i++;
	    	}else {
	    		printf("%s", message);
	    		for(int l = 0; l < 5; l++) {
	    			message[l] = 'A';
	    		}
	    		i = 0;
	    	}
	    	*/
	    	
	    	
	    	if(i <= 4 && i >= 0 && t != ' ') {
	    		printf("%c", t);
	    		i++;
	    	}else {
	    		printf(" ");
	    		i = 0;
	    	}
	    	

	    }
	}


   

    fclose(fin);

	return 0;
}