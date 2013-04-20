
#include <stdio.h>
#include "compileChoices.h"
#include "wordSelector.h"
#include "cliForAddWord.h"

BOOL addWords(int chapter) {
	char local[100];
	char foreign[100];
	while(1) {
		//read the local word
		printf("Currently on chapter %d\n", chapter);
		printf("Enter English word: ");
		if(!readTypedInput(local, sizeof(local)))
			return FAIL;
		
		//read the foreign word
		printf("Enter %s word: ", language);
		if(!readTypedInput(foreign, sizeof(foreign)))
			return FAIL;
        
		//add the word to the database
		if(addNewWordForReview(local, foreign, language,"", chapter)==SUCCESS) {
			printf("Word added\n\n\n");
		}else {
			printf("Error while adding word\n");
			return FAIL;
		}
	}
	return SUCCESS;
}

