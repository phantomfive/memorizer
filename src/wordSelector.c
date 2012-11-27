/** COPYRIGHT 2012 andrewthompson10@gmail.com
 *  All rights reserved.
 *  May be used under the terms of the GPL 3 or higher. */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "wordDatabase.h"

#include "wordSelector.h"

//private methods
static WordGroupType chooseNextWordGroup(int quantity, int index);
static BOOL getWordForGroupA(WordForReview *word, int index);
static BOOL getWordForGroupB(WordForReview *word, int index);
static BOOL getWordForGroupC(WordForReview *word, int index);
static BOOL getWordForGroupD(WordForReview *word, int index);
static BOOL getWordForGroupE(WordForReview *word, int index);
static void moveWordUpAGroup(WordForReview *word);
static void moveWordDownAGroup(WordForReview *word);
//-----------------------------------------------------------------------------
//Public methods
//-----------------------------------------------------------------------------
BOOL selectWordsForReview(WordForReview *words, int quantity) {
	int i=0;
	BOOL rv = SUCCESS;

	for(i=0;i<quantity;i++) {
		WordGroupType type = chooseNextWordGroup(quantity, i);

		printf("Got selected type %d\n", type);
	
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
//--------------------------------------------------------------------------
static int wordsReturned = 0;
#define CYCLE_POINT 8
#define NEW_WORDS_TO_REVIEW_AT_A_TIME 10
#define GROUP_MIN 10
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

	//cycle is an internal measurements that means 'every X words'
	//If we have learned all the words in our review set, then
	//get some more from group A and start learning them!
	if(groupBcount+groupCcount+index < NEW_WORDS_TO_REVIEW_AT_A_TIME 
	   && groupAcount>index)
		 return WordGroupA;

	//Some things we want to make sure to do every X words.
	int cycle = wordsReturned % CYCLE_POINT;
	wordsReturned++;
	printf("Cycle %d, groupE %d, groupD %d\n", cycle, groupEcount,
	       groupDcount);

	//return at least one word from groupE and one from groupD every cycle
	if(cycle==1 && groupDcount>index && groupDcount>=GROUP_MIN)return WordGroupD;
	if(cycle==0 &&                      groupEcount>=GROUP_MIN)return WordGroupE;
	

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
