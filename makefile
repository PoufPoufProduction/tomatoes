CC=g++
CFLAGS=-g -W -Wall -ansi -DSDL=1
INCLUDES=-Iinc -I/usr/local/include -I/usr/include -I/usr/include/SDL
LDFLAGS=-L/usr/local/lib -ldl -rdynamic -lSDL -lSDL_image -lSDL_mixer -lconfig++ -lsplashouille
EXEC=tomatoes

all: $(EXEC)

$(EXEC): obj/Main.o obj/Menu.o obj/Game.o obj/Configuration.o obj/SDL_Keyboard.o
	$(CC) -o $@ $^ $(LDFLAGS)

obj/Main.o : src/Main.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Menu.o : src/Menu.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Game.o : src/Game.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/Configuration.o : src/Configuration.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

obj/SDL_Keyboard.o : src/SDL_Keyboard.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(INCLUDES)

clean:
	@rm -f obj/*
	@rm -f tomatoes
 
 
