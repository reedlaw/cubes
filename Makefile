CC=g++
LDLIBS=-lglut -lGLEW -lGL -lm
CXXFLAGS=-Wtrigraphs -g
LDFLAGS=-Wtrigraphs -g
export
all: main
main: shader_utils.o
clean:
	rm -f *.o main
.PHONY: all clean
