//copyright andrewthompson10@gmail.com 2012.
//Usable under the terms of the GPL 3 or later only.

#include <string.h>
#include <stdio.h>
#include <ctype.h>

#include "common.h"

BOOL strncatSafe(char *dest, char *src, int destSize) {
	if(strlen(dest) + strlen(src) +1 > destSize) {
		return FAIL;
	}
	
	strcat(dest, src);
	return SUCCESS;
}


BOOL readTypedInput(char *buf, int bufLen) {
	int len, spaceCount=0, i;
	if(fgets(buf, bufLen, stdin)==NULL) {
		return FAIL;
	}
    
	//chop off extra space and newlines at the end
	len = strlen(buf);
	while(isspace((int)buf[len-1])) {
		buf[len-1] = 0;
		len--;
	}
    
	//chop off extra space from the beginning
	while(isspace((int)buf[spaceCount])) {
		spaceCount++;
	}
	if(spaceCount>0) {
		for(i=spaceCount;i<=len;i++) {
			buf[i-spaceCount] = buf[i];
		}
	}
	
	return SUCCESS;
}
