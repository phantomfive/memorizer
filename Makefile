CC=gcc
CCFLAGS=-Wall -Werror -std=c99 -ggdb
LDFLAGS= -lsqlite3

OBJDIR=out

CSRC := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, $(OBJDIR)/%.o, $(CSRC))

memorizer: $(CSRC)
	make objs
	$(CC) -o memorizer $(LDFLAGS) $(OBJS)


objs: $(OBJDIR) $(OBJS)
$(OBJDIR)/%.o: src/%.c
	$(CC) $(CCFLAGS) $(INC) -c $< -o $@
$(OBJDIR):
	mkdir -p $(OBJDIR)

depend: $(CSRC)
	makedepend -Y -p$(OBJDIR) $(INC) $(CSRC) 2> /dev/null
	rm Makefile.bak

run:
	./memorizer

clean:
	rm -rf $(OBJDIR) $(TOCLEAN)
	rm -f memorizer

.PHONY: objs depend clean memorizer


# DO NOT DELETE

outsrc/common.o: src/common.h
outsrc/main.o: src/wordSelector.h src/common.h
outsrc/wordDatabase.o: src/wordDatabase.h src/common.h src/wordSelector.h
outsrc/wordSelector.o: src/wordDatabase.h src/common.h src/wordSelector.h
