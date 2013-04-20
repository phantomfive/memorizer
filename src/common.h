/** Common code and defines.
    Copyright 2012 by Andrew. Usable under the terms of the GPL */

#ifndef COMMON_H
#define COMMON_H

//define basic boolean type
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef BOOL
#define BOOL char
#endif
#ifndef YES
#define YES TRUE
#endif
#ifndef NO
#define NO FALSE
#endif
#ifndef SUCCESS
#define SUCCESS TRUE
#endif
#ifndef FAIL
#define FAIL FALSE
#endif

//Some defines for printing colors to the terminal
#define GREEN_ON_BLACK "\033[0;32m"
#define YELLOW_ON_BLACK "\033[0;33m"
#define NORMAL_COLOR "\033[0m"

/** Tries to append src to dest, which should both be NULL terminated.
  * destSize should be the total space available in dest. If there is
  * not enough room, will return FAIL without changing anything*/
BOOL strncatSafe(char *dest, char *src, int destSize);


/**Reads a line from the user, and chops off the extra whitespace
   from the beginning and end. Returns true on success, false on error.*/
BOOL readTypedInput(char *buf, int bufLen);

#endif

