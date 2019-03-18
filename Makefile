CC = gcc
CFLAGS = -Wall -ansi -pedantic -Wextra -std=gnu99
LFLAGS = -lm -pthread `sdl2-config --cflags --libs` -lSDL2_ttf 
DEPS = header.h
OBJ = main.o images.o input.o ai.o main_menu.o threads.o
OUT_EXE = geofront

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(OUT_EXE): $(OBJ)
	$(CC) -o $@ $^ $(LFLAGS)

clean:
	rm -f *.o $(OUT_EXE)

install:
	cp ./$(OUT_EXE) /bin

rebuild: clean
	 make
