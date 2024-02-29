all: run

RESULT=main

compile: sum.S Makefile main.S
	gcc -g -no-pie sum.S main.S -I. -o $(RESULT)

run: compile
	./$(RESULT)

clean:
	rm -f $(RESULT)
