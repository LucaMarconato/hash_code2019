build: main.cpp
	g++ -std=c++11 main.cpp

run: build
	time ./a.out
