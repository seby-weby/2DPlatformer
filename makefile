all:
	g++ main.cpp -o main -std=c++20 `sdl2-config --cflags --libs` -lSDL2_ttf
	./main