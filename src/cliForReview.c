/**********************************************************************
 * cliForReview.c                                                     *
 * There are two types of words, advanced and basic. We have a        *
 * different set of functions for each, and treat them differently.   *
 *                                                                    *
 * Basic:                                                             *
 *        We show the foreign word first, and                         *
 *        let them guss the word in English.                          *
 *                                                                    *
 * Advanced:                                                          *
 *        We show the word in English first,                          *
 *        and let them type the foreign word.                         *
 *                                                                    *
 *********************************************************************/


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


//-----------------------------------------------------------------
// Methods for basic words (Read the comment above for more info)
//-----------------------------------------------------------------

/**Reviews a basic word  by showing it in the foreign language, then letting
 * you guess the English.*/
static void reviewBasicWord(WordForReview *word) {
	char input[1000];

	//Show them the word and
	//give them a prompt
	printf("----\n");
	if(!CHARACTER_BASED) {
		printf("The %s word is " GREEN_ON_BLACK "%s" NORMAL_COLOR".\n",
	   	    language, word->foreignWord);
	}
	else {
	   //If it's character based, show them the hint
		printf("The %s is " GREEN_ON_BLACK "%s" NORMAL_COLOR".\n",
		       language, word->foreignWord);
		printf("The hint is %s.\n", word->hint);
	}
	printf("Can you guess the English word? --> ");

    
	//Read their answer and
	//check if it's right
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


//---------------------------------------------------------------
// Methods for advanced words
//---------------------------------------------------------------

static void showAdvancedWord(WordForReview *word, char *input, int size) {

	//print the prompt
	printf("----\n");
	printf("The English word is " GREEN_ON_BLACK "%s" NORMAL_COLOR ".\n",
	       word->localWord);
	if(CHARACTER_BASED) {
		printf("Press <Enter> when you know the character, "\
		       " or 'h' for a hint\n<ENTER>\n");
	}else{
		printf(" Can you guess the %s word? --> ", language);
   }

	//read the answer
	if(!readTypedInput(input, size))
		exit(-1);

	//If they asked for a hint, show it and
	//read more input for their answer
	if(CHARACTER_BASED && !strcasecmp(input, "h")) {
		printf("\n   hint:\n %s\n\n"\
		   "Press <Enter> when you've guessed the character\n<ENTER>",word->hint);
		if(!readTypedInput(input, sizeof(input)))
			exit(-1);
	}
}

/**Returns true if they got the answer right, or false if they didn't*/
static BOOL checkAdvancedAnswer(WordForReview *word, char *answer) {
	char input[1000];

	//If it's not character based, this is easy,
	//just check they they typed the correct word
	if(!CHARACTER_BASED) {
		if(!strcmp(answer, word->foreignWord)) {
			printf("   "YELLOW_ON_BLACK"YES!"NORMAL_COLOR " %s\n", compliment());
			return SUCCESS;
		}else{
			printf("   " GREEN_ON_BLACK "%s" NORMAL_COLOR
		       " is the word for %s\n", word->foreignWord, word->localWord);
		}
	}
	else {
		//If it is character based, then we don't have a
		//way of checking right now. Someday, I will implement
		//handwriting recognition. Until then, we will
		//ask them if they got it right. We have to trust them.
		printf("The word is"YELLOW_ON_BLACK" %s"NORMAL_COLOR".\n",
		       word->foreignWord);
		printf("Now, did you get it right? (y/n) -->");
		if(!readTypedInput(input, sizeof(input)))
			exit(-1);

		//see if they typed y
		if(!strcasecmp(input,"y")) {
			printf("Excellent! %s\n", compliment());
			return SUCCESS;
		}
		else {
			printf("ok.\n");
		}
	}

	return FAIL;
}

/**Reviews an advanced word by showing it in English, 
 * then letting you guess the foreign language.*/
static void reviewAdvancedWord(WordForReview *word) {
	char input[1000];
	BOOL result;

	//show the word and let them guess it
	showAdvancedWord(word, input, sizeof(input));


	//Display the correct output if they got it right,
	//or the correction if they got it wrong
	result = checkAdvancedAnswer(word, input);
	markWordAsReviewed(word, result);
	
	//give them a chance to breath before moving on
	printf("   [Press Enter]\n");
	if(!readTypedInput(input, sizeof(input)))
		exit(-1);
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
