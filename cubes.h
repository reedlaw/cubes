#ifndef _cubesh_
#define _cubesh_

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

struct Vertex{
  float x, y, z, ao;        //Vertex
  float nx, ny, nz, type;     //Normal
  float r, g, b, a;     //Color
};

bool is_near(float v1, float v2);
bool get_similar_vertex(Vertex & vertex, std::map<Vertex, unsigned short> & VertexIndex, unsigned short & result);

#endif
