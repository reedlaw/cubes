#include "../cubes.h"

bool is_near(float v1, float v2);
void greedyMesh(float *volume, int *dimensions, std::vector<Vertex> & vertices, std::vector<GLushort> & indices);
float findVoxel(int i, int j, int k, float *volume, int *dimensions);


