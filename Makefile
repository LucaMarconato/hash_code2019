build: main.cpp
	g++ -std=c++14 main.cpp

run: build
	time ./a.out
