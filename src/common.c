//copyright andrewthompson10@gmail.com 2012.
//Usable under the terms of the GPL 3 or later only.

#include <string.h>

#include "common.h"

BOOL strncatSafe(char *dest, char *src, int destSize) {
	if(strlen(dest) + strlen(src) +1 > destSize) {
		return FAIL;
	}
	
	strcat(dest, src);
	return SUCCESS;
}
