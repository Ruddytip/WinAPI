TARGET = main
DEBUG = Debug
SRC := $(wildcard *.cpp)
FLAGS = -Wall -std=c++2a -lgdi32 -O3
DIR = ./

all: $(TARGET)

$(TARGET): $(SRC)
	g++ -o $(TARGET) $(SRC) $(FLAGS)

debug:
	 g++ -g3 -pg -O3 -o $(DEBUG) $(SRC) $(FLAGS)
	 $(DEBUG).exe
	 gprof $(DEBUG).exe

clean:
	-rm $(TARGET).exe

clean32:
	erase $(TARGET).exe
	
clean$(DEBUG):
	-rm $(DEBUG).exe
	-rm *.out