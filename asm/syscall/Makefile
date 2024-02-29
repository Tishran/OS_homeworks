all: run

RESULT=main
CFLAGS=-g -no-pie -nostdlib -Wall -fno-stack-protector

compile: Makefile entry.S syscall.h syscall.c main.c
	gcc $(CFLAGS) entry.S -I. ./syscall.c main.c -o $(RESULT)

run: compile
	./$(RESULT)

clean:
	rm -f $(RESULT)
