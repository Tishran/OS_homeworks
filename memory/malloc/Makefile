all: run

RESULT=main
CFLAGS=-g -O3 -no-pie -nostdlib -fno-builtin -Wall -fno-stack-protector

compile: $(wildcard .h) $(wildcard *.c) $(wildcard *.S)
	gcc $(CFLAGS) entry.S syscall.S malloc.c main.c -o $(RESULT)

run: compile
	./$(RESULT)

clean:
	rm -f $(RESULT)
