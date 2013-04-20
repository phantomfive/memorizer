#ifndef GUIFORADDWORD_H
#define GUIFORADDWORD_H
/***************************************************************************
 * This file provides a command line interface for entering words, for     *
 * data entry.                                                             *
 * Author andrewthompson10@gmail.com. Copyright 2013. All Rights Reserved. *
 * Usable under the terms of the GPL 3 or later.                           *
 ***************************************************************************/

#include "common.h"

/**Prompts the user to type words and adds them as needed. Words will
 * be added by default to the passed in chapter. If FAIL is returned,
 * it's probably a database error*/
BOOL addWords(int chapter);

#endif
