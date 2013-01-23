# Basic Makefile.
# Earlier my Makefile checked all the dependencies and only compiled the ones
# that had changed. Then I found it's faster if you just compile it all
# in a single invocation. Better yet, it's a shorter Makefile. -AT

CC=gcc
CCFLAGS=-Wall -Werror -std=c99 # -ggdb
                               # Uncomment -ggdb to debug
LDFLAGS= -lsqlite3

CSRC := $(wildcard src/*.c)
HDRS := $(wildcard src/*.h)

all: japanese russian

japanese: $(CSRC) $(HDRS)
	$(CC) -o japanese -D JAPANESE $(CCFLAGS) $(LDFLAGS) $(CSRC)

russian: $(CSRC) $(HDRS)
	$(CC) -o russian -D RUSSIAN $(CCFLAGS) $(LDFLAGS) $(CSRC)

clean:
	rm -rf japanese russian russian.dSYM japanese.dSYM

run:
	@echo type ./japanese or ./russian

.PHONY: clean all

