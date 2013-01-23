/** COPYRIGHT 2012 andrewthompson10@gmail.com
 *  All rights reserved.
 *  May be used under the terms of the GPL 3 or higher. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wordDatabase.h"

#include "wordSelector.h"

static WordForReview *tmp; //only used to get the size in the following line
static char language[sizeof(tmp->language)] = "data";

//private methods
static WordGroupType chooseNextWordGroup(int quantity, int index);
static BOOL getWordForGroupA(WordForReview *word, int index);
static BOOL getWordForGroupB(WordForReview *word, int index);
static BOOL getWordForGroupC(WordForReview *word, int index);
static BOOL getWordForGroupD(WordForReview *word, int index);
static BOOL getWordForGroupE(WordForReview *word, int index);
static BOOL putWordsInArray(WordForReview *words, int quantity);
static void moveWordUpAGroup(WordForReview *word);
static void moveWordDownAGroup(WordForReview *word);
static WordForReview *getNextWordFromList();
static BOOL populateWordList();

//-----------------------------------------------------------------------------
//Public methods
//The comments for these methods are in the header file, wordSelector.h
//-----------------------------------------------------------------------------
BOOL selectWordLanguage(const char *name) {
	char databaseName[1500];
	
	//set the name of the database we will be using
	snprintf(databaseName, sizeof(databaseName), "data/%s.db", name);
	if(setWordDatabaseName(databaseName)==FAIL) return FAIL;

	//store the name of the language
	strncpy(language, name, sizeof(language)-1);
	language[sizeof(language)-1] = 0;
	return SUCCESS;
}

WordForReview *getNextWordForReview() {
	WordForReview *rv;

	//We want to select three words, review each word twice, then
	//select three new words. So we keep those three words in the list.
	//once the list is empty, we repopulate the list.
	rv = getNextWordFromList();
	if(rv!=NULL)
	return rv;

	//If we get here, the list is empty, so we better populate it again
	populateWordList();
	rv = getNextWordFromList();
	
	return rv;
}

BOOL addNewWordForReview(const char *localWord, const char *foreignWord,
                         const char *language, int chapter) {
	WordForReview word;
	strncpy(word.language, language, sizeof(word.language));
	word.language[sizeof(word.language)-1] = 0;

	strncpy(word.localWord, localWord, sizeof(word.localWord));
	word.localWord[sizeof(word.localWord)-1] = 0;

	strncpy(word.foreignWord, foreignWord, sizeof(word.foreignWord));
	word.foreignWord[sizeof(word.foreignWord)-1] = 0;

	word.type = WordGroupA;
	word.competencyLevel = 0;
	word.chapter = chapter;

	return databaseAddWord(&word);
}


//This function is too long.......
#define COMPETENCY_B  5
#define COMPETENCY_C 10
#define COMPETENCY_D  7
#define COMPETENCY_E  3
BOOL markWordAsReviewed(WordForReview *word, BOOL result) {
	
	//if it's wordGroupA, then we move it to B automatically, since they
	//only need to see it to get out of A
	if(word->type==WordGroupA) {
		moveWordUpAGroup(word);
	   return databaseUpdateWord(word);
	}

	//in all the other groups, adjust the competency level
	if(result) word->competencyLevel++;
	else       word->competencyLevel--;
	
	
	//if it's in group B, then once the competency level hits a
	//certain level, move it to the next group
	if(word->type==WordGroupB) {
		if(word->competencyLevel>=COMPETENCY_B) {
			moveWordUpAGroup(word);
		}
	}
	
	//if it's in group C, then once the competency level hits a
	//certain level, move it to the next group
	else if(word->type==WordGroupC) {
		if(word->competencyLevel>=COMPETENCY_C) {
			moveWordUpAGroup(word);
		}else if(word->competencyLevel<=-COMPETENCY_C) {
			moveWordDownAGroup(word);
		}
	}

	//if it's in group D, then once the competency level hits a
	//certain level, move it to the next group
	else if(word->type==WordGroupD) {
		if(word->competencyLevel>=COMPETENCY_D) {
			moveWordUpAGroup(word);
		}else if(word->competencyLevel<=-COMPETENCY_D) {
			moveWordDownAGroup(word);
		}
	}

	//if it's in group E, then it stays there. If the competency
	//gets too low, we'll move it back to D, though
	else if(word->type==WordGroupE) {
		if(word->competencyLevel<= -COMPETENCY_E) {
			moveWordUpAGroup(word);
		}
	}

	//ok, there was more repetition in that section than is healthy.
	//However, I might want to change the behavior of each word type individually
	//later on, so it's probably ok.
	return databaseUpdateWord(word);
}

int getNumberOfWordsMemorized() {
	//we will consider it learned if it is in group D or group E
	int groupEcount=0;
	int groupDcount=0;
	if(!databaseGetCountForWordGroup(WordGroupE, &groupEcount) ||
	   !databaseGetCountForWordGroup(WordGroupD, &groupDcount) ) {
			return -1;
	}

	return groupEcount + groupDcount;
}

//--------------------------------------------------------------------------
// Private methods
// These are for managing the word lists
//--------------------------------------------------------------------------

//The number of words in the list. We will put each word in the list twice.
//When it runs out, we will return false.
#define WORDS_IN_LIST 3
#define TIMES_TO_REVIEW_LIST 2
static WordForReview wordList[WORDS_IN_LIST*2];
static int wordsLeftInList=0;

//Returns the next word in the list. If the wordlist is empty, returns NULL
static WordForReview *getNextWordFromList() {
	WordForReview *rv;
	if(wordsLeftInList<=0) return NULL;

	wordsLeftInList--;
	rv = &wordList[wordsLeftInList % WORDS_IN_LIST];
	return rv;
}

//Adds word to the list. Return SUCCESS or FAIL
static BOOL populateWordList() {
	//First we get some words from the database.
	if(!putWordsInArray(wordList, WORDS_IN_LIST)) {
		return FAIL;
	}

	//calculate the words left, which will be higher since we
	//review each word on the list twice before getting new words
	wordsLeftInList = WORDS_IN_LIST*TIMES_TO_REVIEW_LIST;

	return SUCCESS;
}

//-----------------------------------------------------------------------------
// More private methods
// These manage the logic of which word will be chosen next from the database
//-----------------------------------------------------------------------------

/**Tries to fill 'words' with 'quantity' words from the database.
 * returns FAIL or SUCCESS.*/
static BOOL putWordsInArray(WordForReview *words, int quantity) {
	int i=0;
	BOOL rv = SUCCESS;

	for(i=0;i<quantity;i++) {
		WordGroupType type = chooseNextWordGroup(quantity, i);

		printf("Got selected type %d\n", type);
	
		//if we can't get the preffered group, go one group up.
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



#define NEW_WORDS_TO_REVIEW_AT_A_TIME 10
#define GROUP_MIN 40
#define GROUP_D_FREQUENCY 4
#define GROUP_D_URGENT_FREQ 3 //if wordcount gets too high for group D,
                              //we increase the frequency of showing it
#define GROUP_D_URGENT_LEVEL 350
#define GROUP_E_FREQUENCY 12
static WordGroupType chooseNextWordGroup(int quantity, int index) {
	int groupBcount;
	int groupCcount;
	int groupAcount;
	int groupDcount;
	int groupEcount;
	if(!databaseGetCountForWordGroup(WordGroupB, &groupBcount) ||
	   !databaseGetCountForWordGroup(WordGroupC, &groupCcount) ||
		!databaseGetCountForWordGroup(WordGroupA, &groupAcount) ||
		!databaseGetCountForWordGroup(WordGroupD, &groupDcount) ||
		!databaseGetCountForWordGroup(WordGroupE, &groupEcount) ) {
		return WordGroupE;
	}

	//If we have learned all the words in our review set, then
	//get some more from group A and start learning them!
	if(groupBcount+groupCcount+index < NEW_WORDS_TO_REVIEW_AT_A_TIME 
	   && groupAcount>index)
		 return WordGroupA;

	//For group E, once it gets past the min, we return a word
	//to work on with a certain frequency
	if(groupEcount>=GROUP_MIN && 0==(rand()%GROUP_E_FREQUENCY))
		return WordGroupE;

	//For group D, we return a word to work on with a certain
	//frequency. The frequency changes if it gets too full
	if(groupDcount>=GROUP_MIN) {
		if(groupDcount>=GROUP_D_URGENT_LEVEL) {
			if     (0==rand()%GROUP_D_URGENT_FREQ) return WordGroupD;
		   else if(0==(rand()%GROUP_D_FREQUENCY)) return WordGroupD;
		}
	}

	//choose a word randomly from group B or C
	//If we've already chosen 'index' words from this group, then choose a
	//different group. That's what the 'groupdCount>index' condition is about
	if((rand()%2==0 || groupCcount<=index) && groupBcount>index) {
			return WordGroupB;
	}else {
		if(groupCcount>index)
			return WordGroupC;
	}

	//default to group E
	return WordGroupE;
}


static BOOL getWordForGroupA(WordForReview *word, int index) {
	return databaseFillWordFromGroup(word, index, WordGroupA);
}

static BOOL getWordForGroupB(WordForReview *word, int index) {
	if(rand()%2==0)
	  return databaseFillWordFromGroupOrderByLeastRecent(word,index,WordGroupB);
	else
	  return databaseFillWordFromGroupOrderByLeastSkilled(word,index,WordGroupB);
}

static BOOL getWordForGroupC(WordForReview *word, int index) {
	if(rand()%2==0)
	  return databaseFillWordFromGroupOrderByLeastRecent(word,index,WordGroupC);
	else
	  return databaseFillWordFromGroupOrderByLeastSkilled(word,index,WordGroupC);
}


static BOOL getWordForGroupD(WordForReview *word, int index) {
	if(rand()%2==0)
	  return databaseFillWordFromGroupOrderByLeastSkilled(word,index,WordGroupD);
	else
	  return databaseFillWordFromGroupOrderByLeastRecent(word,index,WordGroupD);
}

static BOOL getWordForGroupE(WordForReview *word, int index) {
	if(rand()%2==0)
	  return databaseFillWordFromGroupOrderByLeastSkilled(word,index,WordGroupE);
	else
	  return databaseFillWordFromGroupOrderByLeastRecent(word,index,WordGroupE);
}

/**Convenience function that moves the word up a group and
  *resets the competency level. Error prone, repeated operation,
  *so good to put it in a function*/
static void moveWordUpAGroup(WordForReview *word) {
	word->type++;
	if(word->type>WordGroupE)
		word->type = WordGroupE;
	
	word->competencyLevel = 0;
}

/**Convenience function that moves the word down a group and
 * resets the competency level.*/
static void moveWordDownAGroup(WordForReview *word) {
	word->type--;
	if(word->type<WordGroupA)
		word->type = WordGroupA;
	
	word->competencyLevel = 0;
}
