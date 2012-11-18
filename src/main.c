#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordSelector.h"


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

	while(1) {
		printf("Menu.\n");
		printf("1) Add word\n");
		printf("2) Quit\n");
		printf("--> ");

		if(fgets(input, sizeof(input), stdin)==NULL)
			break;

		choice = atoi(input);	

		if     (choice==1) addWord();
		else if(choice==2) break;
		else printf("Unrecognized Option\n");

		printf("\n\n\n\n\n\n");
	}

	return 0;
}

