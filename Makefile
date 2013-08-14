CC=g++
LDLIBS=-lglut -lGLEW -lGL
CXXFLAGS=-Wtrigraphs -g
LDFLAGS=-Wtrigraphs -g
export
all: main
main: shader_utils.o
main.o: res_texture.c
clean:
	rm -f *.o main
.PHONY: all clean
