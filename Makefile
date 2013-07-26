CC=g++
LDLIBS=-lglut -lGLEW -lGL
CFLAGS=-Wtrigraphs
all: main
main: shader_utils.o
clean:
	rm -f *.o main
.PHONY: all clean
