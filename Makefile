
main: main.c
	gcc -I/usr/include/SDL2 -L/usr/lib main.c -o main -lSDL2 -lSDL2_image

run: main
	./main
