
#include <stdio.h>
#include "common.h"
#include "compileChoices.h"
#include "wordSelector.h"
#include "cliForAddWord.h"

BOOL addWords(int chapter) {
	WordForReview *w;
	char local[ sizeof(w->localWord) ];
	char foreign[sizeof(w->foreignWord)];
	char hint[sizeof(w->hint)];

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

		//Right now we only use the hint
		//If it is character based
		//because those are extra hard.
		hint[0]=0;
		if(CHARACTER_BASED==YES) {
			printf("Enter a hint: ");
			if(!readTypedInput(hint, sizeof(hint)))
				return FAIL;
		}


        
		//add the word to the database
		if(addNewWordForReview(local, foreign, language,hint, chapter)==SUCCESS) {
			printf("Word added\n\n\n");
		}else {
			printf("Error while adding word\n");
			return FAIL;
		}
	}
	return SUCCESS;
}

