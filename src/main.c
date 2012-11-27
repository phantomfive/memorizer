/***************************************************************************
 * Author andrewthompson10@gmail.com. Copyright 2012. All Rights Reserved. *
 * Usable under the terms of the GPL 3 or later.                           *
 ***************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "wordSelector.h"

#define WORDS_AT_A_TIME 3
#define TIMES_THROUGH_BEFORE_NEW_WORDS 2

#define GREEN_ON_BLACK "\033[0;32m"
#define YELLOW_ON_BLACK "\033[0;33m"
#define NORMAL_COLOR "\033[0m"

/**Choose a compliment for the user*/
static const char *compliment() {
	char *list[]  = {"Good Job!", "You are great!", "You're a genius!",
	                 "Do it again!", "You're awesome!", "Keep up the good work!",
	                 "You'll learn this in no time!", "Memorizing master!",
	                 "You're a star!"};
	int listLen = sizeof(list)/sizeof(list[0]);
	int select = rand()% listLen;
	return list[select];
}

static void printWelcome() {
	int wordsLearned;
	printf("Welcome!\n");
	
	//Only show them how many words they've learned if it's high enough,
	//so they don't get demoralized too easily.
	wordsLearned = getNumberOfWordsMemorized();
	if(wordsLearned>21) {
		printf("You have learned %d words so far.\n", wordsLearned);
	}
	printf("\n");
}

/**Read input from the user. This needs to be cleaned up, but
  *that will be done during the UI design phase*/
static BOOL readTypedInput(char *buf, int bufLen) {
	int len;
	if(fgets(buf, bufLen, stdin)==NULL) {
		return FAIL;
	}

	//since we are expecting to read the line from the commandline,
	//it might have a newline at the end. Chop it off.
	len = strlen(buf);
	if(buf[len-1]=='\n')
		buf[len-1] = 0;

	return SUCCESS;
}

/**Reviews a basic word  by showing it in Russian, then letting
 * you guess the English.*/
static void reviewBasicWord(WordForReview *word) {
	char input[1000];
	printf("----\n");
	printf("The Russian word is %s.\n   Can you guess the English word? --> ",
	       word->foreignWord);

	if(!readTypedInput(input, sizeof(input)))
		exit(-1);

	if(!strcmp(input, word->localWord)) {
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
 * guess the Russian.*/
static void reviewAdvancedWord(WordForReview *word) {
	char input[1000];
	printf("----\n");
	printf("The English word is %s.\n Can you guess the Russian word? --> ",
	       word->localWord);

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

/**Keep reviewing words until they quit*/
static BOOL reviewWords() {
	int i,j;
	WordForReview words[WORDS_AT_A_TIME]={};

	printf("Reviewing words!!\n");

	while(1) {
		if(!selectWordsForReview(words, WORDS_AT_A_TIME)) {
			printf("Cannot go on! Exiting.\n");
			exit(-1);
		}
		
		for(j=0;j<TIMES_THROUGH_BEFORE_NEW_WORDS;j++) {
			for(i=0;i<WORDS_AT_A_TIME;i++) {
				if(words[i].type==WordGroupA || words[i].type==WordGroupB) {
					reviewBasicWord(&words[i]);
				}
				else {
					reviewAdvancedWord(&words[i]);
				}
				printf("word group %d, skill %d, id %d\n",words[i].type,
		                                         words[i].competencyLevel,
															  words[i].id);
			}
		}
	}
}


		

/**UI to add a word to the database*/
BOOL addWord() {
	char eng[100];
	char rus[100];
	printf("Enter English word: ");
	fgets(eng, sizeof(eng), stdin);
	eng[strlen(eng)-1] = 0;
	printf("Enter Russian word: ");
	fgets(rus, sizeof(rus), stdin);
	rus[strlen(rus)-1] = 0;

	if(addNewWordForReview(eng, rus, "Russian")==SUCCESS) {
		printf("Word added\n");
	}else {
		printf("Error while adding word\n");
	}
	return TRUE;
}


int main() {
	char input[1000];
	int choice; 

	srand(time(NULL));

	printWelcome();

	while(1) {
		printf("Menu.\n");
		printf("1) Review Words\n");
		printf("2) Add word\n");
		printf("3) Quit\n");
		printf("--> ");

		if(fgets(input, sizeof(input), stdin)==NULL)
			break;

		choice = atoi(input);	

		printf("\n\n\n\n\n\n");
		if     (choice==1) reviewWords();
		if     (choice==2) addWord();
		else if(choice==3) break;
		else printf("Unrecognized Option\n");

	}

	return 0;
}

