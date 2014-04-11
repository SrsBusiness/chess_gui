CC = g++
CFLAGS = -Wall -g -O3
LIBS = -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf

main: main.cpp chess.cpp chess.h gui.h
	${CC} ${CFLAGS} -o main main.cpp chess.cpp ${LIBS}
clean:
	rm main
