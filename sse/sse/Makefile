RESULT := main
CFLAGS := -g -O2 -march=haswell -std=c++17 -Wall -Werror

all: run

run: compile
	python3 test.py

compile: main.cpp model.cpp sse.cpp sse.h Makefile
	g++ $(CFLAGS) main.cpp model.cpp sse.cpp -o $(RESULT)

clean:
	rm -f $(RESULT)
