﻿__kernel void cipherC(unsigned int size, __global int *shiftValues, __global char *text, __global char *encryptedText) {  	size_t index = get_global_id(0); 	size_t shiftIndex = get_local_id(0);	  	if (index < size) { 		if ((text[index] >= 'a') && (text[index] <= 'z')) { 			text[index] = text[index] - 32 + shiftValues[shiftIndex];  			if (text[index] > 'Z') { 				text[index] = text[index] - 'Z' + 'A' - 1; 			} 			else if (text[index] < 'A') { 				text[index] = text[index] - 'A' + 'Z' + 1; 			}  			encryptedText[index] = text[index]; 		} 		else if ((text[index] >= 'A') && (text[index] <= 'Z')) { 			text[index] = text[index] + shiftValues[shiftIndex];  			if (text[index] > 'Z') { 				text[index] = text[index] - 'Z' + 'A' - 1; 			} 			else if (text[index] < 'A') { 				text[index] = text[index] - 'A' + 'Z' + 1; 			}  			encryptedText[index] = text[index]; 		} 		else { 			encryptedText[index] = text[index]; 		} 	} }  __kernel void decrypt(unsigned int size, __global int *shiftValues, __global char *encryptedText, __global char *decryptedText) {  	size_t index = get_global_id(0); 	size_t shiftIndex = get_local_id(0);  	if (index < size) { 		if ((encryptedText[index] >= 'A') && (encryptedText[index] <= 'Z')) { 			encryptedText[index] = encryptedText[index] - shiftValues[shiftIndex];  			if (encryptedText[index] > 'Z') { 				encryptedText[index] = encryptedText[index] - 'Z' + 'A' - 1; 			} 			else if (encryptedText[index] < 'A') { 				encryptedText[index] = encryptedText[index] - 'A' + 'Z' + 1; 			}  			decryptedText[index] = encryptedText[index]; 		} 		else { 			decryptedText[index] = encryptedText[index]; 		} 	} }