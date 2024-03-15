RESULT := main
CFLAGS := -Wall -Werror -lm

all: run

run: compile
	./$(RESULT)

compile: $(wildcard *.h) $(wildcard *.c)
	gcc main.c handler.c $(CFLAGS) -o $(RESULT)

clean:
	rm -f $(RESULT)
