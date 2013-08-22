#ifndef _cubesh_
#define _cubesh_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex{
  glm::vec3 pos;
  glm::vec2 uv;
  glm::vec3 color;
  glm::vec3 normal;
  bool operator<(const Vertex that) const{
    return memcmp((void*)this, (void*)&that, sizeof(Vertex))>0;
  };
};

bool is_near(float v1, float v2);
bool get_similar_vertex(Vertex & vertex, std::map<Vertex, unsigned short> & VertexIndex, unsigned short & result);
void indexVBO(std::vector<glm::vec3> & verts,
              std::vector<glm::vec2> & uvs,
              std::vector<glm::vec3> & colors,
              std::vector<glm::vec3> & normals,
              std::vector<unsigned short> & indices,
              std::vector<glm::vec3> & indexed_verts,
              std::vector<glm::vec3> & indexed_colors,
              std::vector<glm::vec3> & indexed_normals);

#endif
