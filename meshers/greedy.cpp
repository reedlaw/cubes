#include "greedy.h"

bool is_near(float v1, float v2){
  return fabs(v1-v2) < 0.01f;
}

int findVoxel(int i, int j, int k, int *volume, int *dimensions) {
  return volume[i + dimensions[0] * (j + dimensions[1] * k)];
}

void greedyMesh(int *volume, int *dimensions, std::vector<Vertex> & vertices, std::vector<GLushort> & indices)
{
  glm::vec3 p1, p2, p3, p4, vecU, vecV, normal;
  //Sweep over 3-axes
  for(int d=0; d<3; d++) {
    int i, j, k, l, w, h;
    int u = (d+1)%3;
    int v = (d+2)%3;
    int x[] = { 0, 0, 0 };
    int q[] = { 0, 0, 0 };
    int mask[dimensions[u] * dimensions[v]];
    int normals_mask[dimensions[u] * dimensions[v]];
    q[d] = 1;
    //Set normal mask to negative ones indicating not yet on
    for(i=0; i<dimensions[u]*dimensions[v]; i++) {
      normals_mask[i] = -1;
    }

    for(x[d]=-1; x[d]<dimensions[d]; ) {
      //Compute mask
      int n = 0;
      for(x[v]=0; x[v]<dimensions[v]; x[v]++) {
        for(x[u]=0; x[u]<dimensions[u]; x[u]++) {
          mask[n++] = (0 <= x[d] ? findVoxel(x[0], x[1], x[2], volume, dimensions) : false) != (x[d] < dimensions[d]-1 ? findVoxel(x[0]+q[0], x[1]+q[1], x[2]+q[2], volume, dimensions) : false);
        }
      }

      x[d]++;
      //Generate mesh for mask using lexicographic ordering
      n = 0;
      for(j=0; j<dimensions[v]; j++) {
        for(i=0; i<dimensions[u]; ) {
          int c = mask[n];
          if(c != 0) {
            //Compute width
            for(w=1; c == mask[n+w] && i+w<dimensions[u]; w++) {}

            //Flip normal bit
            bool flip_normal = false;
            if(normals_mask[n] == 1 || normals_mask[n] == -1) {
              flip_normal = true;
            } else {
              normals_mask[n] = 1;
            }

            //Compute height (this is slightly awkward
            bool done = false;
            for(h=1; j+h<dimensions[v]; h++) {
              for(k=0; k<w; k++) {
                if(c != mask[n+k+h*dimensions[u]]) {
                  done = true;
                  break;
                }
              }
              if(done) {
                break;
              }
            }
            //Add quad
            x[u] = i; x[v] = j;
            int du[] = {0,0,0}; du[u] = w;
            int dv[] = {0,0,0}; dv[v] = h;
            p1 = glm::vec3(x[0]-dimensions[0]/2, x[1]-dimensions[1]/2, x[2]-dimensions[2]/2);
            p2 = glm::vec3(x[0]+du[0]-dimensions[0]/2, x[1]+du[1]-dimensions[1]/2, x[2]+du[2]-dimensions[2]/2);
            p3 = glm::vec3(x[0]+du[0]+dv[0]-dimensions[0]/2, x[1]+du[1]+dv[1]-dimensions[1]/2, x[2]+du[2]+dv[2]-dimensions[2]/2);
            p4 = glm::vec3(x[0] +dv[0] -dimensions[0]/2, x[1] +dv[1]-dimensions[1]/2, x[2] +dv[2]-dimensions[2]/2);
            float ao = 1.0;

            vecU = p2 - p1;
            vecV = p3 - p1;
            normal.x = (vecU.y * vecV.z) - (vecU.z * vecV.y);
            normal.y = (vecU.z * vecV.x) - (vecU.x * vecV.z);
            normal.z = (vecU.x * vecV.y) - (vecU.y * vecV.x);

            if(flip_normal){
              Vertex vertex1 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex1);
              Vertex vertex2 = { p2.x, p2.y, p2.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex2);
              Vertex vertex3 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex3);
              Vertex vertex4 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex4);
              Vertex vertex5 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex5);
              Vertex vertex6 = { p4.x, p4.y, p4.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex6);
            } else {
              Vertex vertex1 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex1);
              Vertex vertex2 = { p2.x, p2.y, p2.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex2);
              Vertex vertex3 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex3);
              Vertex vertex4 = { p4.x, p4.y, p4.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex4);
              Vertex vertex5 = { p1.x, p1.y, p1.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex5);
              Vertex vertex6 = { p3.x, p3.y, p3.z, 1.0, normal.x, normal.y, normal.z, ao, 1.0, 0.0 };
              indices.push_back(vertices.size());
              vertices.push_back(vertex6);
            }

            //Zero-out mask
            for(int l=0; l<h; l++) {
              for(int k=0; k<w; k++) {
                mask[n+k+l*dimensions[u]] = false;
                normals_mask[n+k+l*dimensions[u]] = false;
              }
            }
            //Increment counters and continue
            i += w; n += w;
          } else {
            if(normals_mask[n] == 1) {
              normals_mask[n] = false;
            }
            i++; n++;
          }
        }
      }
    }
  }
  // for(int i=0; i<vertices.size(); i++) {
  //   fprintf(stderr, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
  // }

  // for(int i=0; i<vertices.size(); i++) {
  //   fprintf(stderr, "vn %f %f %f\n", vertices[i].nx, vertices[i].ny, vertices[i].nz);
  // }

  // for(int i=0; i<indices.size(); i=i+3) {
  //   fprintf(stderr, "f %i//%i %i//%i %i//%i\n", indices[i]+1, indices[i]+1, indices[i+1]+1, indices[i+1]+1, indices[i+2]+1, indices[i+2]+1);
  // }
}
