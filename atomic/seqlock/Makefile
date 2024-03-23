all: run

RESULT=main
CFLAGS=-g -O2 -Wall -Werror -lpthread

compile: $(wildcard .h) $(wildcard *.c)
	! grep pthread seqlock.h
	gcc $(CFLAGS) main.c -o $(RESULT)

run: compile
	./$(RESULT)

clean:
	rm -f $(RESULT)
