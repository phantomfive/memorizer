#include <stdio.h>
#include <string.h>
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

BOOL addNewWordForReview(const char *localWord, const char *foreignWord,
                         const char *language) {
	WordForReview word;
	strncpy(word.language, language, sizeof(word.language));
	word.language[sizeof(word.language)-1] = 0;

	strncpy(word.localWord, localWord, sizeof(word.localWord));
	word.localWord[sizeof(word.localWord)-1] = 0;

	strncpy(word.foreignWord, foreignWord, sizeof(word.foreignWord));
	word.foreignWord[sizeof(word.foreignWord)-1] = 0;

	word.type = WordGroupA;
	word.competencyLevel = 0;

	return databaseAddWord(&word);
}


//This function is too long.......
#define COMPETENCY_B 5
#define COMPETENCY_C 10
#define COMPETENCY_D 15
#define COMPETENCY_E 10
BOOL markWordAsReviewed(WordForReview *word, BOOL result) {
	
	//if it's wordGroupA, then we move it to B automatically, since they
	//only need to see it to get out of A
	if(word->type==WordGroupA) {
		word->type = WordGroupB;
		word->competencyLevel = 0;
	   return databaseUpdateWord(word);
	}

	//in all the other groups, adjust the competency level
	if(result) word->competencyLevel++;
	else       word->competencyLevel--;
	
	
	//if it's in group B, then once the competency level hits a
	//certain level, move it to the next group
	if(word->type==WordGroupB) {
		if(word->competencyLevel>=COMPETENCY_B) {
			word->type = WordGroupC;
			word->competencyLevel = 0;
		}else if(word->competencyLevel<=-COMPETENCY_B) {
			word->type = WordGroupA;
			word->competencyLevel = 0;
		}
	}
	
	//if it's in group C, then once the competency level hits a
	//certain level, move it to the next group
	if(word->type==WordGroupC) {
		if(word->competencyLevel>=COMPETENCY_C) {
			word->type = WordGroupD;
			word->competencyLevel = 0;
		}else if(word->competencyLevel<=-COMPETENCY_C) {
			word->type = WordGroupB;
			word->competencyLevel = 0;
		}
	}

	//if it's in group D, then once the competency level hits a
	//certain level, move it to the next group
	if(word->type==WordGroupD) {
		if(word->competencyLevel>=COMPETENCY_D) {
			word->type = WordGroupE;
			word->competencyLevel = 0;
		}else if(word->competencyLevel<=-COMPETENCY_D) {
			word->type = WordGroupC;
			word->competencyLevel = 0;
		}
	}

	//if it's in group E, then it stays there. If the competency
	//gets too low, we'll move it back to D, though
	if(word->type==WordGroupE) {
		if(word->competencyLevel<= -COMPETENCY_E) {
			word->type = WordGroupD;
			word->competencyLevel = 0;
		}
	}

	//ok, there was more repetition in that section than is healthy.
	//However, I might want to change the behavior of each word type individually
	//later on, so it's probably ok.
	return databaseUpdateWord(word);
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
