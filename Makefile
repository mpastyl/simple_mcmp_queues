CC=gcc
#CFLAGS=

mcmp_bounded_lock_based: mcmp_bounded_lock_based.o
	$(CC) -o lock_based mcmp_bounded_lock_based.o 

.PHONY: clean

clean:
	rm -f *.o lock_based
