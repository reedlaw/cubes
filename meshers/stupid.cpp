#include "stupid.h"

void stupidMesh(int *volume, int *dimensions, std::vector<glm::vec3> & vertices, std::vector<glm::vec3> & normals)
{
  int x[] = { 0, 0, 0 };
  int n = 0;
  glm::vec3 p1, p2, p3, p4, vecU, vecV, normal;
  for(x[2]=-dimensions[2]/2; x[2]<dimensions[2]/2; x[2]++) {
    for(x[1]=-dimensions[1]/2; x[1]<dimensions[1]/2; x[1]++) {
      for(x[0]=-dimensions[0]/2; x[0]<dimensions[0]/2; x[0]++) {
        if(volume[n] == 1) {

          for(int d=0; d < 3; d++) {
            int t[] = { x[0], x[1], x[2], };
            int u[] = { 0, 0, 0 };
            int v[] = { 0, 0, 0 };
            u[(d+1)%3] = 1;
            v[(d+2)%3] = 1;
            for(int s=0; s<2; s++) {
              t[d] = x[d] + s;
              if(s==0){
                p1 = glm::vec3(t[0], t[1], t[2]);
                p2 = glm::vec3(t[0]+u[0], t[1]+u[1], t[2]+u[2]);
                p3 = glm::vec3(t[0]+u[0]+v[0], t[1]+u[1]+v[1], t[2]+u[2]+v[2]);
                p4 = glm::vec3(t[0]+v[0], t[1]+v[1], t[2]+v[2]);
              } else {
                p3 = glm::vec3(t[0], t[1], t[2]);
                p2 = glm::vec3(t[0]+u[0], t[1]+u[1], t[2]+u[2]);
                p1 = glm::vec3(t[0]+u[0]+v[0], t[1]+u[1]+v[1], t[2]+u[2]+v[2]);
                p4 = glm::vec3(t[0]+v[0], t[1]+v[1], t[2]+v[2]);
              }
              vertices.push_back(p1);
              vertices.push_back(p2);
              vertices.push_back(p3);
              vecU = p2 - p1;
              vecV = p3 - p1;
              normal.x = (vecU.y * vecV.z) - (vecU.z * vecV.y);
              normal.y = (vecU.z * vecV.x) - (vecU.x * vecV.z);
              normal.z = (vecU.x * vecV.y) - (vecU.y * vecV.x);
              for(int i=0; i < 6; i++)
                normals.push_back(normal);
              vertices.push_back(p1);
              vertices.push_back(p4);
              vertices.push_back(p3);
            }
          }
        }
        n++;
      }
    }
  }
}
