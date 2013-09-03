#include "../cubes.h"

int findVoxel(int i, int j, int k, int *volume, int *dimensions);
void makeVoxels(int *l, int *h, int (*f)(int, int, int), int *volume);
int cube_func(int i, int j, int k);
int hill_func(int i, int j, int k);
int sphere_func(int i, int j, int k);
int hole_func(int i, int j, int k);
