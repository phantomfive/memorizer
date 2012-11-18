#include <string.h>

#include "common.h"

BOOL strncatSafe(char *dest, char *src, int destSize) {
	if(strlen(dest) + strlen(src) +1 > destSize) {
		return FAIL;
	}
	
	strcat(dest, src);
	return SUCCESS;
}
