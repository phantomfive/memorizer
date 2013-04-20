/***************************************************************************
 * The starting point for the program using a command line interface.      *
 *                                                                         *
 * Author andrewthompson10@gmail.com. Copyright 2012. All Rights Reserved. *
 * Usable under the terms of the GPL 3 or later.                           *
 ***************************************************************************/

#include <stdio.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include "wordSelector.h"

#include "cliForReview.h"
#include "cliForAddWord.h"
#include "compileChoices.h"


//This is our current chapter
static int chapter = 1;


static void printWelcome() {
	int wordsLearned;
	printf("Welcome to %s!\n", language);
	
	//Only show them how many words they've learned if it's high enough,
	//so they don't get demoralized too easily.
	wordsLearned = getNumberOfWordsMemorized();
	if(wordsLearned>21) {
		printf("You have learned %d words so far.\n", wordsLearned);
	}
	printf("\n");
}
		

/**Currently we only use the chapter for entering words. Eventually
 it might be used for reviewing them too*/
static void changeChapter() {
	printf("Currently on chapter %d\n", chapter);
	printf("Select new chapter: ");
	scanf("%d", &chapter);
	printf("Chapter %d chosen\n", chapter);
}


int main() {
	char input[1000];
	int choice; 

	srand(time(NULL));

	selectWordLanguage(language);

	printWelcome();

	while(1) {
		printf("Menu.\n");
		printf("1) Review Words\n");
		printf("2) Add words\n");
		printf("3) Select chapter\n");
		printf("4) Quit\n");
		printf("--> ");

		if(fgets(input, sizeof(input), stdin)==NULL)
			break;

		choice = atoi(input);	

		printf("\n\n\n\n\n\n");
		if     (choice==1) reviewWords(chapter);
		else if(choice==2) addWords(chapter);
		else if(choice==3) changeChapter();
		else if(choice==4) break;
		else printf("Unrecognized Option\n");

	}

	return 0;
}

