#include "cells.h"
#include "noise.h"

void makeCellComplex(std::vector<float> & volume) {
  int res[3];
  float d[3] = { -1.0, 1.0, 0.25 };
  float *dimensions[3] = { d, d, d };
  for(int i=0; i<3; ++i) {
    res[i] = 2 + ceil((dimensions[i][1] - dimensions[i][0]) / dimensions[i][2]);
  }
  float z=dimensions[2][0]-dimensions[2][2];
  for(int k=0; k<res[2]; ++k) {
    float y=dimensions[1][0]-dimensions[1][2];
    for(int j=0; j<res[1]; ++j) {
      float x=dimensions[0][0]-dimensions[0][2];
      for(int i=0; i<res[0]; ++i) {
        float v = x*x + y*y + z*z - 1.0;
        // float v = y + noise(x*2+5,y*2+3,z*2+0.6);
        volume.push_back(v);
        x+=dimensions[0][2];
      }
      y+=dimensions[1][2];
    }
    z+=dimensions[2][2];
  }
}
