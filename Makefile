TARGET = main
SRC := $(wildcard *.cpp)
CFLAGS = -I. -Wall -std=c++2a

all: $(TARGET)

$(TARGET): $(SRC)
	g++ -Wall -o $(TARGET) $(SRC)

debug:
	 g++ -g3 -pg -O3 $(SRC)
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