#include <stdio.h>

#include "database.h"

#include "wordSelector.h"

static WordGroupType chooseNextWordGroup();
static BOOL getWordForGroupA(WordForReview **word, int index);
static BOOL getWordForGroupB(WordForReview **word, int index);
static BOOL getWordForGroupC(WordForReview **word, int index);
static BOOL getWordForGroupD(WordForReview **word, int index);

//---------------------------------------------------------------------------------
//Public methods
//---------------------------------------------------------------------------------
BOOL selectWordsForReview(WordForReview **words, int quantity) {
	int i=0;
	BOOL rv = FAIL;

	for(i=0;i<quantity;i++) {
		WordGroupType type = chooseNextWordGroup();
	
		if(type==WordGroupD)             rv=getWordForGroupD(words, i);
		if(type==WordGroupC || rv==FAIL) rv=getWordForGroupC(words, i);
		if(type==WordGroupB || rv==FAIL) rv=getWordForGroupB(words, i);
		if(type==WordGroupA || rv==FAIL) rv=getWordForGroupA(words, i);

		if(rv==FAIL) {
			printf("Not enough words\n");
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
static WordGroupType chooseNextWordGroup() {
	return WordGroupD;
}

static BOOL getWordForGroupA(WordForReview **word, int index) {
	return databaseFillWordFromGroup(word, index, WordGroupA);
}

static BOOL getWordForGroupB(WordForReview **word, int index) {
	return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupB);
}

static BOOL getWordForGroupC(WordForReview **word, int index) {
	return databaseFillWordFromGroupOrderByLeastRecent(word, index, WordGroupC);
}

static BOOL getWordForGroupD(WordForReview **word, int index) {
	return databaseFillWordFromGroupOrderByLeastSkilled(word, index, WordGroupD);
}


