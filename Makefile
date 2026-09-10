CC = gcc
CFLAGS = -Wall -g

all: test test-coalesce

test: test.o memory.o
	$(CC) $(CFLAGS) $^ -o $@

test-coalesce: test.o memory-coalesce.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f *.o
	rm -f test test-coalesce