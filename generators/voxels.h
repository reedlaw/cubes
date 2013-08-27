#include "../cubes.h"

void makeVoxels(int *l, int *h, int (*f)(int, int, int), int *volume);
int cube_func(int i, int j, int k);
int hill_func(int i, int j, int k);
