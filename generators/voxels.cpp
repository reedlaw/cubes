#include "voxels.h"

void makeVoxels(int *l, int *h, int (*f)(int, int, int), int *volume) {
  int n = 0;
  for(int k=l[2]; k<h[2]; k++) {
    for(int j=l[1]; j<h[1]; j++) {
      for(int i=l[0]; i<h[0]; i++) {
        volume[n] = (*f)(i,j,k);
        n++;
      }
    }
  }
}

int cube_func(int i, int j, int k) {
  if (i<2) {
    return 1;
  } else {
    return 2;
  }
}

int hole_func(int i, int j, int k) {
  if (abs(i-4) > 2 || abs(j-4) > 2) {
    return 1;
  } else {
    return false;
  }
}

int sphere_func(int i, int j, int k) {
  return i*i+j*j+k*k <= 2*2 ? 2 : 0;
}

int hill_func(int i, int j, int k) {
  if (j <= 32 * exp(-(i*i + k*k) / 512.0)) {
    return 1;
  } else {
    return 0;
  }
}

int hilly_terrain_func(int i, int j, int k) {
  float h0 = 3.0 * sin(3.1415 * i / 12.0 - 3.1415 * k * 0.1) + 27;
  if(j > h0+1) {
    return 0;
  }
  if(h0 <= j) {
    return 1;
  }
  float h1 = 2.0 * sin(3.1415 * i * 0.25 - 3.1415 * k * 0.3) + 20;
  if(h1 <= j) {
    return 2;
  }
  return 3;
}

int anim_hill_func(int i, int j, int k) {
  float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 15;  // base 15° per second
  if (j <= 16 * exp(-(i*i + k*k) / angle))
    return 1;
}

// cube
// int l[] = { 0, 0, 0 };
// int h[] = { 6, 6, 6 };

// hole
// int l[] = { 0, 0, 0 };
// int h[] = { 8, 8, 2 };

// hill
// int l[] = { 0, 0, 0 };
// int h[] = { 32, 32, 32 };

// sphere
// int l[] = { -2, -2, -2 };
// int h[] = { 2, 2, 2 };

// int d[] = { (h[0]-l[0]), (h[1]-l[1]), (h[2]-l[2]) };
// int size = d[0]*d[1]*d[2];
// int volume[size]; // = {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
