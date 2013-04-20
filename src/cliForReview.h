#ifndef CLIFORREVIEW_H
#define CLIFORREVIEW_H

/**Keep reviewing words until they quit with ctrl-c. High quality interface!
 * If there is a database error, this will probably call exit()
 * 'chapter' has no use right now, but it seems like it could be useful in
 * the future. */
BOOL reviewWords(int chapter);


#endif
