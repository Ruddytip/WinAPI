SRC := $(wildcard *.cpp)
CFLAGS = -I. -Wall -std=c++2a

all: test

test: $(SRC)
	g++ -o $@ $^ $(CFLAGS)

clean:
	rm test
