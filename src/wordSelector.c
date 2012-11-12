#include <stdio.h>
#include <stdlib.h>

#include "wordDatabase.h"

#include "wordSelector.h"

//private methods
static WordGroupType chooseNextWordGroup(int quantity, int index);
static BOOL getWordForGroupA(WordForReview **word, int index);
static BOOL getWordForGroupB(WordForReview **word, int index);
static BOOL getWordForGroupC(WordForReview **word, int index);
static BOOL getWordForGroupD(WordForReview **word, int index);
static BOOL getWordForGroupE(WordForReview **word, int index);
//---------------------------------------------------------------------------------
//Public methods
//---------------------------------------------------------------------------------
BOOL selectWordsForReview(WordForReview **words, int quantity) {
	int i=0;
	BOOL rv = FAIL;

	for(i=0;i<quantity;i++) {
		WordGroupType type = chooseNextWordGroup(quantity, i);
	
		if(type==WordGroupE)             rv=getWordForGroupE(words, i);
		if(type==WordGroupD || rv==FAIL) rv=getWordForGroupD(words, i);
		if(type==WordGroupC || rv==FAIL) rv=getWordForGroupC(words, i);
		if(type==WordGroupB || rv==FAIL) rv=getWordForGroupB(words, i);
		if(type==WordGroupA || rv==FAIL) rv=getWordForGroupA(words, i);

		if(rv==FAIL) {
			printf("Failed to get word\n");
			return FAIL;
		}
	}

	return SUCCESS;
}

void markWordAsReviewed(WordForReview *word, BOOL result) {

}


//---------------------------------------------------------------------------------
// Private methods
//---------------------------------------------------------------------------------
static int wordsReturned = 0;
#define CYCLE_POINT 8
#define NEW_WORDS_TO_REVIEW_AT_A_TIME 10
static WordGroupType chooseNextWordGroup(int quantity, int index) {
	int groupBcount = databaseGetCountForWordGroup(WordGroupB);
	int groupCcount = databaseGetCountForWordGroup(WordGroupC);

	//return at least one word from groupE and one from groupD every cycle
	int cycle = wordsReturned % CYCLE_POINT;
	wordsReturned++;
	if(cycle == 0) return WordGroupE;
	if(cycle == 1) return WordGroupD;
	
	//If we have learned all the words in our review set, then
	//get some more from group A and start learning them!
	if(groupBcount+groupCcount < NEW_WORDS_TO_REVIEW_AT_A_TIME) 
		return WordGroupA;

	//choose a word randomly from group B or C
	if(rand()%2==0)
		return WordGroupB;
	else
		return WordGroupC;
}


static BOOL getWordForGroupA(WordForReview **word, int index) {
	return databaseFillWordFromGroup(word, index, WordGroupA);
}

static BOOL getWordForGroupB(WordForReview **word, int index) {
	if(rand()%2==0)
		return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupB);
	else
		return databaseFillWordFromGroupOrderByLeastSkilled(word, index, WordGroupB);
}

static BOOL getWordForGroupC(WordForReview **word, int index) {
	if(rand()%2==0)
		return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupC);
	else
		return databaseFillWordFromGroupOrderByLeastSkilled(word, index, WordGroupD);
}


static BOOL getWordForGroupD(WordForReview **word, int index) {
	if(rand()%2==0)
		return databaseFillWordFromGroupOrderByLeastSkilled(word, index, WordGroupD);
	else
		return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupD);
}

static BOOL getWordForGroupE(WordForReview **word, int index) {
	if(rand()%2==0)
		return databaseFillWordFromGroupOrderByLeastSkilled(word, index, WordGroupE);
	else
		return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupE);
}
