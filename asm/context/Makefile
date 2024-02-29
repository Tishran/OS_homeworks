all: run

RESULT=main

compile: main.c context.S context.h Makefile
	gcc -no-pie -O2 -fno-omit-frame-pointer context.S main.c -o $(RESULT)

run: compile
	./$(RESULT)

clean:
	rm -f $(RESULT)
