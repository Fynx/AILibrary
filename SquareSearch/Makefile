CXXFLAGS = -O2 -Wall -std=c++11

all: main.o SquareSearch.o
	g++ $(CXXFLAGS) main.o SquareSearch.o -o main

main.o: SquareSearch.h main.cpp
	g++ -c $(CXXFLAGS) main.cpp

qua.o: SquareSearch.h SquareSearch.cpp
	g++ -c $(CXXFLAGS) SquareSearch.cpp

clean:
	rm -rf *.o main