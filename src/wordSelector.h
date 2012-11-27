#ifndef WORD_SELECTOR_H
#define WORD_SELECTOR_H
/************************************************************
 *  Copyright 2012 andrewthompson10@gmail.com               *
 *  All Rights Reserved.                                    *
 *  May be used under the terms of the GPL, 3 or higher.    *
 ************************************************************/

#include "common.h"

/**Please see the README for a description of these groups*/
typedef enum WordGroupTypeEnum {
	WordGroupA=1,
	WordGroupB=2,
	WordGroupC=3,
	WordGroupD=4,
	WordGroupE=5
} WordGroupType;

typedef struct WordForReviewStruct {
	int id;
	
	/**The language for the word. Right now we only use Russian. Can be used
	 * to distinguish various datasets.*/
	char language[50];

	/**The word in 'English'*/
	char localWord[100];

	/**The word in 'Russian'*/
	char foreignWord[100];

	/**Indicates what review group we are in*/
   WordGroupType type;

	/**Indicates the competency level within the review group.
	  *See markWordAsReviewed() for details*/
	int competencyLevel;
} WordForReview;


/** Gets the next 'quantity' words from the database for review.
  * 'words' should be an array of WordForReview at least 'quantity' long.
  * Returns SUCCESS on success or FAIL if there is an error. */
BOOL selectWordsForReview(WordForReview *words, int quantity);

/** Call this after a word gets reviewed, to update the database as to the
    result. Send in the word, and SUCCESS if they got it, or FAIL otherwise.
	 'word' gets updated with the new data. Fails only on database error.*/
BOOL markWordAsReviewed(WordForReview *word, BOOL result);

/** Adds a word to the database */
BOOL addNewWordForReview(const char *localWord,  const char *foreignWord, 
                         const char *language);

/**Returns the number of words learned so far. If there is an error,
 * returns -1. */
int getNumberOfWordsMemorized();

#endif

