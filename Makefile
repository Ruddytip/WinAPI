TARGET = main
SRC := $(wildcard *.cpp)
FLAGS = -Wall -std=c++2a -lgdi32

all: $(TARGET)

$(TARGET): $(SRC)
	g++ -o $(TARGET) $(SRC) $(FLAGS)

debug:
	 g++ -g3 -pg -O3 $(SRC) $(FLAGS)
	 a.exe
	 gprof a.exe

clean:
	erase $(TARGET).exe
	erase *.tga
	erase *.out
	erase a.exe

cleanDebug:
	erase *.out
	erase a.exe