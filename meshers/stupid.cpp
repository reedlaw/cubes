#include "stupid.h"

void stupidMesh(int *volume, int *dimensions, std::vector<Vertex> & vertices, std::vector<GLushort> & indices)
{
  int x[] = { 0, 0, 0 };
  int n = 0;
  glm::vec3 p1, p2, p3, p4, vecU, vecV, normal;
  for (x[2]=-dimensions[2]/2; x[2]<dimensions[2]/2; x[2]++) {
    for (x[1]=-dimensions[1]/2; x[1]<dimensions[1]/2; x[1]++) {
      for (x[0]=-dimensions[0]/2; x[0]<dimensions[0]/2; x[0]++) {
        if(volume[n] == 1) {

          for (int d=0; d < 3; d++) {
            int t[] = { x[0], x[1], x[2], };
            int u[] = { 0, 0, 0 };
            int v[] = { 0, 0, 0 };
            u[(d+1)%3] = 1;
            v[(d+2)%3] = 1;
            for (int s=0; s<2; s++) {
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
              if (d*2+s == 0) {
                normal = glm::vec3(1.0, 0.0, 0.0);
              } else if(d*2+s == 1) {
                normal = glm::vec3(0.0, 1.0, 0.0);
              } else if(d*2+s == 2) {
                normal = glm::vec3(0.0, 0.0, 1.0);
              } else if(d*2+s == 3) {
                normal = glm::vec3(-1.0, 0.0, 0.0);
              } else if(d*2+s == 4) {
                normal = glm::vec3(0.0, -1.0, 0.0);
              } else if(d*2+s == 5) {
                normal = glm::vec3(0.0, 0.0, -1.0);
              }
              Vertex vertex1 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex1);
              Vertex vertex2 = { p2.x, p2.y, p2.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex2);
              Vertex vertex3 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex3);

              Vertex vertex4 = { p4.x, p4.y, p4.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex4);
              Vertex vertex5 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex5);
              Vertex vertex6 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, 100.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex6);
            }
          }
        }
        n++;
      }
    }
  }
  // for (int i=0; i<vertices.size(); i++) {
  //   fprintf(stderr, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
  // }

  // for (int i=0; i<vertices.size(); i++) {
  //   if (vertices[i].side == 0) {
  //     normal = glm::vec3(1.0, 0.0, 0.0);
  //   } else if(vertices[i].side == 1) {
  //     normal = glm::vec3(0.0, 1.0, 0.0);
  //   } else if(vertices[i].side == 2) {
  //     normal = glm::vec3(0.0, 0.0, 1.0);
  //   } else if(vertices[i].side == 3) {
  //     normal = glm::vec3(-1.0, 0.0, 0.0);
  //   } else if(vertices[i].side == 4) {
  //     normal = glm::vec3(0.0, -1.0, 0.0);
  //   } else if(vertices[i].side == 5) {
  //     normal = glm::vec3(0.0, 0.0, -1.0);
  //   }
  //   fprintf(stderr, "vn %f %f %f\n", normal.x, normal.y, normal.z);
  // }

  // for (int i=0; i<indices.size(); i=i+3) {
  //   fprintf(stderr, "f %i//%i %i//%i %i//%i\n", indices[i]+1, indices[i]+1, indices[i+1]+1, indices[i+1]+1, indices[i+2]+1, indices[i+2]+1);
  // }

}
