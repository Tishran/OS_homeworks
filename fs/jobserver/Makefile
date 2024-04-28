RESULT := main
CFLAGS := -g -fsanitize=address,undefined -Wall -Werror

all: run

run: compile
	/bin/bash ./test.sh

compile: Makefile main.c
	gcc $(CFLAGS) main.c -o $(RESULT)

clean:
	rm -f $(RESULT)
