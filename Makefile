CC=gcc
CFLAGS=-O2 -Wall -fopenmp
DEPS= mcmp_bounded_lock_based.h mcmp_bounded_lock_free.h utils.h getticks.h

all: test_lock_based test_lock_free

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

test_lock_based: mcmp_bounded_lock_based.o test_lock_based.o
	$(CC) -o test_lock_based test_lock_based.o mcmp_bounded_lock_based.o $(CFLAGS) 

test_lock_free: mcmp_bounded_lock_free.o test_lock_free.o
	$(CC) -o test_lock_free test_lock_free.o mcmp_bounded_lock_free.o $(CFLAGS) 

.PHONY: clean

clean:
	rm -f *.o test_lock_based test_lock_free
