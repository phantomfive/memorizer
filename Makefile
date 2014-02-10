# Basic Makefile.
# Earlier my Makefile checked all the dependencies and only compiled the ones
# that had changed. Then I found it's faster if you just compile it all
# in a single invocation. Better yet, it's a shorter Makefile. -AT

CC=gcc
CCFLAGS=-Wall -Werror -Wno-unneeded-internal-declaration -std=c99 # -ggdb
                                                      # Uncomment -ggdb to debug
LDFLAGS= -lsqlite3

CSRC := $(wildcard src/*.c)
HDRS := $(wildcard src/*.h)

all: japanese russian korean japaneseCharacters

japanese: $(CSRC) $(HDRS)
	$(CC) -o japanese -D JAPANESE $(CCFLAGS) $(LDFLAGS) $(CSRC)

russian: $(CSRC) $(HDRS)
	$(CC) -o russian -D RUSSIAN $(CCFLAGS) $(LDFLAGS) $(CSRC)

korean: $(CSRC) $(HDRS)
	$(CC) -o korean -D KOREAN $(CCFLAGS) $(LDFLAGS) $(CSRC)

japaneseCharacters: $(CSRC) $(HDRS)
	$(CC) -o japaneseCharacters -D JAPANESE_CHARS $(CCFLAGS) $(LDFLAGS) $(CSRC)


clean:
	rm -rf japanese russian russian.dSYM japanese.dSYM korean korean.dSYM

run:
	@echo type ./japanese or ./russian or ./korean or ./japaneseCharacters

.PHONY: clean all

