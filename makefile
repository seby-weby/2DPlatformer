all:
	g++ main.cpp -o main -std=c++17 `sdl2-config --cflags --libs`
	./main