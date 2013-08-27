CC=g++
LDLIBS=-lglut -lGLEW -lGL -lm
CXXFLAGS=-Wtrigraphs -g
LDFLAGS=-Wtrigraphs -g
export
all: cubes
cubes: utils/shader_utils.o meshers/greedy.o meshers/surface_nets.o generators/cells.o generators/noise.o generators/voxels.o
clean:
	rm -f *.o generators/*.o utils/*.o meshers/*.o cubes
.PHONY: all clean
