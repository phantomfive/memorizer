#ifndef WORD_SELECTOR_H
#define WORD_SELECTOR_H

#include "common.h"

typedef enum WordGroupTypeEnum {
	WordGroupA=1,
	WordGroupB=2,
	WordGroupC=3,
	WordGroupD=4
} WordGroupType;

typedef struct WordForReviewStruct {
	int id;
	
	char language[50];
	char localWord[100];
	char foreignWord[100];
   WordGroupType type;
	int competencyLevel;
} WordForReview;


/** Gets the next 'quantity' words from the database for review.
  * 'words' should be an array of WordForReview at least 'quantity' long.
  * Returns SUCCESS on success or FAIL if there is an error. */
BOOL selectWordsForReview(WordForReview *words, int quantity);

/** Call this after a word gets reviewed, to update the database as to the
    result. Send in the word, and SUCCESS if they got it, or FAIL if they didn't.
	 'word' gets updated with the new data */
void markWordAsReviewed(WordForReview *word, BOOL result);

#endif

