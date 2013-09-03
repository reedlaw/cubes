#include "stupid.h"

int vertexAO(int side1, int side2, int center) {
  if (side1 && side2) {
    return 1;
  }
  return 4 - (side1 + side2 + center);
}

void stupidMesh(int *volume, int *dimensions, std::vector<Vertex> & vertices, std::vector<GLushort> & indices)
{
  int x[] = { 0, 0, 0 };
  int n = 0;
  int side1, side2, center;
  glm::vec3 p1, p2, p3, p4, vecU, vecV, normal; // Vertex positions
  int ao1, ao2, ao3, ao4;  // Ambient occlusion values for each vertex of one cube side

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
              int q[] = { t[0]+dimensions[0]-2, t[1]+dimensions[1]-2, t[2]+dimensions[2]-2 }; // query present location
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

              ao1 = 4; // bottom left
              ao2 = 4; // top left
              ao3 = 4; // top right
              ao4 = 4; // bottom right

              // Check which side we're on and create normal
              if (d*2+s == 0) { // left

                normal = glm::vec3(1.0, 0.0, 0.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1], q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1], q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1]+1, q[2], volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1], q[2]+1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1], q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              } else if(d*2+s == 1) { // right

                normal = glm::vec3(0.0, 1.0, 0.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1], q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1], q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1]-1, q[2], volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1], q[2]-1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1], q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              } else if(d*2+s == 2) { // bottom

                normal = glm::vec3(0.0, 0.0, 1.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1]-1, q[2], volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0], q[1]-1, q[2]+1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2], volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              } else if(d*2+s == 3) { // top

                normal = glm::vec3(-1.0, 0.0, 0.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1]+1, q[2], volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0], q[1]+1, q[2]-1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2], volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              } else if(d*2+s == 4) { // back
                normal = glm::vec3(0.0, -1.0, 0.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]-1, q[1], q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0], q[1]-1, q[2]-1, volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1], q[2]-1, volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1], q[2]-1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]-1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]-1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1], q[2]-1, volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              } else if(d*2+s == 5) { // front

                normal = glm::vec3(0.0, 0.0, -1.0);

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0]+1, q[1], q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0], q[1]+1, q[2]+1, volume, dimensions)) { side2 = true; }
                ao1 = vertexAO(side1, side2, center); // 4

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]+1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]+1, q[1], q[2]+1, volume, dimensions)) { side2 = true; }
                ao2 = vertexAO(side1, side2, center); // 2

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]-1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]-1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1], q[2]+1, volume, dimensions)) { side2 = true; }
                ao3 = vertexAO(side1, side2, center); // 1

                side1 = false; side2 = false; center = false;
                if (findVoxel(q[0], q[1]+1, q[2]+1, volume, dimensions)) { side1 = true; }
                if (findVoxel(q[0]-1, q[1]+1, q[2]+1, volume, dimensions)) { center = true; }
                if (findVoxel(q[0]-1, q[1], q[2]+1, volume, dimensions)) { side2 = true; }
                ao4 = vertexAO(side1, side2, center); // 3

              }

              // if (q[0] = 1 && q[1] == 0 && q[2] == 1) {
              //     ao1 = 4; ao2 = 2; ao3 = 1; ao4 = 3;
              //   }

              Vertex vertex1 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao3 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex1);
              Vertex vertex2 = { p2.x, p2.y, p2.z, 1.0, normal.x, normal.y, normal.z, ao2 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex2);
              Vertex vertex3 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao1 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex3);

              Vertex vertex4 = { p4.x, p4.y, p4.z, 1.0, normal.x, normal.y, normal.z, ao4 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex4);
              Vertex vertex5 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao3 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex5);
              Vertex vertex6 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao1 };
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
  //   fprintf(stderr, "vn %f %f %f %f\n", vertices[i].nx, vertices[i].ny, vertices[i].nz, vertices[i].nw);
  // }

  // for (int i=0; i<indices.size(); i=i+3) {
  //   fprintf(stderr, "f %i//%i %i//%i %i//%i\n", indices[i]+1, indices[i]+1, indices[i+1]+1, indices[i+1]+1, indices[i+2]+1, indices[i+2]+1);
  // }

}
