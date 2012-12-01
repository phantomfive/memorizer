/** Interface to the database. This encapsulates the database so we can
    swap out the implementation and use a different database if we like.
	 Copyright 2012 by Andrew. Usable under the terms of the GPL, 3 or later */

#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"
#include "wordSelector.h"

#define DATABASE_VERSION 1

/** Should let the callerr choose a database name */
BOOL setWordDatabaseName(const char *name);

/** Should fill the array at 'index' randomly with a given word from the word 
  * group. expects every space in the array before 'index' to be filled in, and
  * will not duplicate these words. Returns SUCCESS or FAIL */
BOOL databaseFillWordFromGroup(WordForReview *word, int index, 
                               WordGroupType type);

/** Should fill the array at 'index' with a given word from the word group. 
  * Expects every space in the array before 'index' to be filled in, and will 
  * not duplicate these words. Orders by the least recently reviewed. 
  * Returns SUCCESS or FAIL*/
BOOL databaseFillWordFromGroupOrderByLeastRecent(WordForReview*word, int index,
                                                 WordGroupType type);

/** Should fill the array at 'index' with a given word from the word group. 
  * Expects every space in the array before 'index' to be filled in, and will 
  * not duplicate these words. Returns SUCCESS or FAIL*/
BOOL databaseFillWordFromGroupOrderByLeastSkilled(WordForReview *word, 
                                                  int index,
                                                  WordGroupType type);


/**Sets 'count' to the number of words in the database of group 'type'.
  *Returns TRUE on success, FAIL of fail*/
BOOL databaseGetCountForWordGroup(WordGroupType type, int *count);

/** Updates the word according to its ID anything that's changed in the struct
  * will be changed in the database.*/
BOOL databaseUpdateWord(const WordForReview *word);

/**Adds a word to the database*/
BOOL databaseAddWord(const WordForReview *word);

#endif

