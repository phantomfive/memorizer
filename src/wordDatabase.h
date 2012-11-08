/** Interface to the database. This encapsulates the database so we can
    swap out the implementation and use a different database if we like.
	 Copyright 2012 by Andrew. Usable under the terms of the GPL */

#ifndef DATABASE_H
#define DATABASE_H

#include "common.h"
#include "wordSelector.h"

/** Should fill the array at 'index' randomly with a given word from the word group.
  * expects every space in the array before 'index' to be filled in, and will not
  * duplicate these words. Returns SUCCESS or FAIL */
BOOL databaseFillWordFromGroup(WordForReview **word, int index, WordGroupType type);

/** Should fill the array at 'index' with a given word from the word group. Expects
  * every space in the array before 'index' to be filled in, and will not duplicate   
  * these words. Orders by the least recently reviewed. Returns SUCCESS or FAIL*/
BOOL databaseFillWordFromGroupOrderByLeastRecent(WordForReview **word, int index,
                                                 WordGroupType type);

/** Should fill the array at 'index' with a given word from the word group. Expects
  * every space in the array before 'index' to be filled in, and will not duplicate
  * these words. Returns SUCCESS or FAIL*/
BOOL databaseFillWordFromGroupOrderByLeastSkilled(WordForReview **word, int index,
                                                 WordGroupType type);


/**Returns the number of words in a given group in the database*/
int databaseGetCountForWordGroup(WordGroupType type);

#endif

