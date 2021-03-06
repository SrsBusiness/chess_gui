CC = g++
CFLAGS = -Wall -g -O3
LIBS = -lpthread -lSDL2 -lSDL2_image -lSDL2_ttf

main: main.cpp chess.cpp
	${CC} ${CFLAGS} -o main main.cpp chess.cpp ${LIBS}
movie:main.cpp chess.cpp
	${CC} ${CFLAGS} -o movie main.cpp chess.cpp ${LIBS}
clean:
	rm main
