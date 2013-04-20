#ifndef MAIN_H
#define MAIN_H

/**I thought about how to do this, but I think the easiest way is to have
 * different binaries depending on what you want to study. So at compile
 * time, define exactly one of these to choosee a different language.
 * You can define it anywhere, but I do so in the Makefile.
 * Author: Andrew. Copyright 2013, usable under the GPL 3.0 or later */


#ifdef RUSSIAN
static const char *language = "Russian";
#endif

#ifdef JAPANESE
static const char *language = "Japanese";
#endif

#ifdef JAPANESE_CHARS
static const char *language = "JapaneseChars";
#define COMPLEX
#endif

#ifdef KOREAN
static const char *language = "Korean";
#endif






#endif
