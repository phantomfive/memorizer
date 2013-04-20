#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "wordSelector.h"
#include "compileChoices.h"
#include "common.h"

#include "cliForReview.h"

//---------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------


/**Choose a compliment for the user*/
static const char *compliment() {
	char *list[]  = {
	     "Good Job!", 
	     "You are great!", 
	     "You're a genius!",
	     "Do it again!", 
	     "You're awesome!", 
	     "Keep up the good work!",
	     "You'll learn this in no time!", 
	     "Memorizing master!",
	     "You're a star!", 
	     "Bringing down the house!",
	     "Memorizer style!", 
	     "Beethoven wrote a symphony about you!",
	     "Wow!"};
	int listLen = sizeof(list)/sizeof(list[0]);
	int select = rand()% listLen;
	return list[select];
}


/**Reviews a basic word  by showing it in the foreign language, then letting
 * you guess the English.*/
static void reviewBasicWord(WordForReview *word) {
	char input[1000];
	printf("----\n");
	printf("The %s word is " GREEN_ON_BLACK "%s" NORMAL_COLOR
	       ".\n   Can you guess the English word? --> ",
	       language, word->foreignWord);
    
	if(!readTypedInput(input, sizeof(input)))
		exit(-1);
    
	if(!strcasecmp(input, word->localWord)) {
		printf("   "YELLOW_ON_BLACK"YES!"NORMAL_COLOR" %s\n", compliment());
		markWordAsReviewed(word, SUCCESS);
	}
	else {
		printf("   " GREEN_ON_BLACK "%s" NORMAL_COLOR \
		       " is the word for %s\n", word->localWord, word->foreignWord);
		markWordAsReviewed(word, FAIL);
		printf("   [Press Enter]\n");
		if(!readTypedInput(input, sizeof(input)))
			exit(-1);
	}
}

/**Reviews an advanced word by showing it in English, then letting you
 * guess the foreign language.*/
static void reviewAdvancedWord(WordForReview *word) {
	char input[1000];
	printf("----\n");
	printf("The English word is " GREEN_ON_BLACK "%s" NORMAL_COLOR
	       ".\n Can you guess the %s word? --> ",
           word->localWord, language);
    
	if(!readTypedInput(input, sizeof(input)))
		exit(-1);
	
	if(!strcmp(input, word->foreignWord)) {
		printf("   "YELLOW_ON_BLACK"YES!"NORMAL_COLOR " %s\n", compliment());
		markWordAsReviewed(word, SUCCESS);
	}
	else {
		printf("   " GREEN_ON_BLACK "%s" NORMAL_COLOR
		       " is the word for %s\n", word->foreignWord, word->localWord);
		markWordAsReviewed(word, FAIL);
		printf("   [Press Enter]\n");
		if(!readTypedInput(input, sizeof(input)))
			exit(-1);
	}
}


//---------------------------------------------------------------------
// Public methods
//---------------------------------------------------------------------

BOOL reviewWords(int chapter) {
    
	printf("Reviewing words!!\n");
    
	while(1) {
		WordForReview *word = getNextWordForReview();
		if(word==NULL) {
			printf("Cannot go on! Exiting.\n");
			exit(-1);
		}
        
		if(word->type==WordGroupA || word->type==WordGroupB)
			reviewBasicWord(word);
		else
			reviewAdvancedWord(word);
        
        
        //this is printed for debugging
		printf("word group %d, skill %d, id %d\n",word->type,
               word->competencyLevel,
               word->id);
	}
}
