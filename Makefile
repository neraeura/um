#
# Makefile for the UM Test lab
# 
CC = gcc


IFLAGS  = -I/comp/40/build/include -I/usr/sup/cii40/include/cii
CFLAGS  = -g -std=gnu99 -Wall -Wextra -Werror -pedantic $(IFLAGS)
LDFLAGS = -g -L/comp/40/build/lib -L/usr/sup/cii40/lib64 
LDLIBS  = -lbitpack -l40locality -lcii40-O2 -lm

EXECS   = writetests

all: um

um: um.o instructionSet.o registers.o memory.o fetcher.o executor.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

instructionSet: um.o instructionSet.o registers.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

fetcher: um.o fetcher.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

registers: um.o registers.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

memory: um.o memory.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

executor: um.o executor.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

writetests: umlabwrite.o umlab.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

reg-test: um.o registers.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

mem-test: um.o memory.o registers.o
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

fetcher-test: um.o fetcher.o 
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

# To get *any* .o file, compile its .c file with the following rule.
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(EXECS)  *.o

