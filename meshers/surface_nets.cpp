#include "surface_nets.h"

int cube_edges[24] = { 0, 1, 0, 2, 0, 4, 1, 3, 1, 5, 2, 3, 2, 6, 3, 7, 4, 5, 4, 6, 5, 7, 6, 7 };
int edge_table[256] = { 0, 7, 25, 30, 98, 101, 123, 124, 168, 175, 177, 182, 202, 205, 211, 212, 772, 771, 797, 794, 870, 865, 895, 888, 940, 939, 949, 946, 974, 969, 983, 976, 1296, 1303, 1289, 1294, 1394, 1397, 1387, 1388, 1464, 1471, 1441, 1446, 1498, 1501, 1475, 1476, 1556, 1555, 1549, 1546, 1654, 1649, 1647, 1640, 1724, 1723, 1701, 1698, 1758, 1753, 1735, 1728, 2624, 2631, 2649, 2654, 2594, 2597, 2619, 2620, 2792, 2799, 2801, 2806, 2698, 2701, 2707, 2708, 2372, 2371, 2397, 2394, 2342, 2337, 2367, 2360, 2540, 2539, 2549, 2546, 2446, 2441, 2455, 2448, 3920, 3927, 3913, 3918, 3890, 3893, 3883, 3884, 4088, 4095, 4065, 4070, 3994, 3997, 3971, 3972, 3156, 3155, 3149, 3146, 3126, 3121, 3119, 3112, 3324, 3323, 3301, 3298, 3230, 3225, 3207, 2, 3200, 3207, 3225, 3230, 3298, 3301, 3323, 3324, 3112, 3119, 3121, 3126, 3146, 3149, 3155, 3156, 3972, 3971, 3997, 3994, 4070, 4065, 4095, 4088, 3884, 3883, 3893, 3890, 3918, 3913, 3927, 3920, 2448, 2455, 2441, 2446, 2546, 2549, 2539, 2540, 2360, 2367, 2337, 2342, 2394, 2397, 2371, 2372, 2708, 2707, 2701, 2698, 2806, 2801, 2799, 2792, 2620, 2619, 2597, 2594, 2654, 2649, 2631, 2624, 1728, 1735, 1753, 1758, 1698, 1701, 1723, 1724, 1640, 1647, 1649, 1654, 1546, 1549, 1555, 1556, 1476, 1475, 1501, 1498, 1446, 1441, 1471, 1464, 1388, 1387, 1397, 1394, 1294, 1289, 1303, 1296, 976, 983, 969, 974, 946, 949, 939, 940, 888, 895, 865, 870, 794, 797, 771, 772, 212, 211, 205, 202, 182, 177, 175, 168, 124, 123, 101, 98, 30, 25, 7, 0 };

void surfaceNets(std::vector<float> & volume, int *dimensions, std::vector<Vertex> & vertices, std::vector<GLushort> & indices)
{
  GLfloat color_table[3][3] = { { 1.0, 0.0, 0.0 }, { 0.0, 1.0, 0.0 }, { 0.0, 0.0, 1.0 } };

  int n = 0;
  int x[3] = { 0, 0, 0 };
  int R[3] = { 1, (dimensions[0]+1), (dimensions[0]+1)*(dimensions[1]+1) };
  float grid[8];
  int buf_no = 1;
  int buffer[R[2]*2];

  // march over voxels
  for(x[2]=0; x[2]<(dimensions[2]-1); x[2]++, n+=dimensions[0], buf_no ^= 1, R[2]=-R[2]) {

    // location in buffer
    int m = 1 + (dimensions[0]+1) * (1 + buf_no * (dimensions[1]+1));
    for(x[1]=0; x[1]<(dimensions[1]-1); x[1]++, n++, m+=2) {
      for(x[0]=0; x[0]<(dimensions[0]-1); x[0]++, n++, m++) {
        int mask = 0;
        int g = 0;
        int index = n;

        for(int k=0; k<2; k++, index += dimensions[0]*(dimensions[1]-2)) {
          for(int j=0; j<2; j++, index += dimensions[0]-2) {
            for(int i=0; i<2; i++, g++, index++) {
              float p = volume[index];
              grid[g] = p;
              mask |= (p < 0.0) ? (1<<g) : 0;
            }
          }
        }

        // no intersection
        if(mask == 0 || mask == 0xff) {
          continue;
        }

        // sum up edge intersections
        int edge_mask = edge_table[mask];
        float v[3] = { 0.0, 0.0, 0.0 };
        int edge_count = 0;

        // for each cube edge
        for(int i=0; i<12; i++) {

          // check if it crosses
          if(!(edge_mask & (1<<i))) {
            continue;
          }

          edge_count++;

          // find the point of intersection
          int e0 = cube_edges[i<<1];
          int e1 = cube_edges[(i<<1)+1];
          float g0 = grid[e0];
          float g1 = grid[e1];
          float t = g0 - g1;
          if(fabs(t) > 1e-6) {
            t = g0 / t;
          } else {
            continue;
          }

          // Interpolate vertices
          for(int j=0, k=1; j<3; j++, k<<=1) {
            int a = e0 & k;
            int b = e1 & k;
            if(a != b) {
              v[j] += a ? 1.0 - t : t;
            } else {
              v[j] += a ? 1.0 : 0.0;
            }
          }
        }

        // Average edge intersections
        float s = 1.0 / (float)edge_count;
        for(int i=0; i<3; i++) {
          v[i] = x[i] + (v[i]*s);
        }

        // add vertex to buffer
        buffer[m] = vertices.size();

        struct Vertex vertex;
        vertex.x = v[0];
        vertex.y = v[1];
        vertex.z = v[2];
        vertex.nx = 1.0;
        vertex.ny = 1.0;
        vertex.nz = 0.0;
        vertex.r = color_table[1][0];
        vertex.g = color_table[1][1];
        vertex.b = color_table[1][2];
        vertex.a = 1.0;
        vertices.push_back(vertex);

        // make faces
        for(int i=0; i<3; i++) {
          if ( !(edge_mask & (1<<i)) ) {
            continue;
          }

          int iu = (i+1)%3;
          int iv = (i+2)%3;

          if ( x[iu]==0 || x[iv]==0 ) {
            continue;
          }

          int du = R[iu];
          int dv = R[iv];

          if (mask & 1) {
            indices.push_back(buffer[m]);
            indices.push_back(buffer[m-du]);
            indices.push_back(buffer[m-du-dv]);
            indices.push_back(buffer[m-dv]);
            indices.push_back(buffer[m]);
            indices.push_back(buffer[m-du-dv]);
          } else {
            indices.push_back(buffer[m]);
            indices.push_back(buffer[m-dv]);
            indices.push_back(buffer[m-du-dv]);
            indices.push_back(buffer[m-du]);
            indices.push_back(buffer[m]);
            indices.push_back(buffer[m-du-dv]);
          }
        }
      }
    }
  }
  for(int i=0; i<vertices.size(); i++) {
    fprintf(stderr, "v %f %f %f\n", vertices[i].x, vertices[i].y, vertices[i].z);
  }

  // for(int i=0; i<vertices.size(); i++) {
  //   fprintf(stderr, "vn %f %f %f\n", vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z);
  // }

  // for(int i=0; i<indices.size(); i=i+3) {
  //   fprintf(stderr, "f %i//%i %i//%i %i//%i\n", indices[i]+1, indices[i]+1, indices[i+1]+1, indices[i+1]+1, indices[i+2]+1, indices[i+2]+1);
  // }

  for(int i=0; i<indices.size(); i=i+3) {
    fprintf(stderr, "f %i %i %i\n", indices[i]+1, indices[i+1]+1, indices[i+2]+1);
  }

}
