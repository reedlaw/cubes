#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <map>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_utils.h"

GLuint program;
GLint attribute_coord3d, attribute_v_color, attribute_texcoord, uniform_mvp;
GLuint vbo_cube_vertices, vbo_cube_colors, vbo_cube_texcoords, ibo_cube_elements, elementbuffer;
std::vector<GLushort> indices;
int screen_width=800, screen_height=600;
int sizeOfMesh = 0;

struct PackedVertex{
  glm::vec3 pos;
  // glm::vec2 uv;
  // glm::vec3 normal;
  bool operator<(const PackedVertex that) const{
    return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
  };
};

bool is_near(float v1, float v2){
  return fabs(v1-v2) < 0.01f;
}

bool get_similar_vertex(PackedVertex & packed,
                       std::map<PackedVertex, unsigned short> & VertexIndex,
                       unsigned short & result)
{
  std::map<PackedVertex, unsigned short>::iterator it = VertexIndex.find(packed);
  if (it == VertexIndex.end()){
    return false;
  } else {
    result = it->second;
    return true;
  }
}

void indexVBO(std::vector<glm::vec3> & verts,
              // std::vector<glm::vec2> & uvs,
              // std::vector<glm::vec3> & normals,

              std::vector<unsigned short> & indices,
              std::vector<glm::vec3> & indexed_verts)
              // std::vector<glm::vec2> & indexed_uvs,
              // std::vector<glm::vec3> & indexed_normals)
{
  std::map<PackedVertex, unsigned short> VertexIndex;

  for (int i=0; i<verts.size(); i++){
    PackedVertex packed = {verts[i]}; // , uvs[i], normals[i]
    unsigned short index;
    bool found = get_similar_vertex(packed, VertexIndex, index);

    if (found){
      indices.push_back(index);
    } else {
      indexed_verts.push_back(verts[i]);
      // indexed_uvs.push_back(uvs[i]);
      // indexed_normals.push_back(normals[i]);
      unsigned short newindex = (unsigned short)indexed_verts.size() - 1;
      indices.push_back(newindex);
      VertexIndex[packed] = newindex;
    }
  }
}

void stupidMesh(int *volume, int *dimensions, std::vector<glm::vec3> & vertices)
{
  int x[] = { 0, 0, 0 };
  int n = 0;
  for(x[2]=0; x[2]<dimensions[2]; x[2]++) {
    for(x[1]=0; x[1]<dimensions[1]; x[1]++) {
      for(x[0]=0; x[0]<dimensions[0]; x[0]++) {
        if(volume[n] == 1) {

          for(int d=0; d < 3; d++) {
            int t[] = { x[0], x[1], x[2], };
            int u[] = { 0, 0, 0 };
            int v[] = { 0, 0, 0 };
            u[(d+1)%3] = 1;
            v[(d+2)%3] = 1;
            for(int s=0; s<2; s++) {
              t[d] = x[d] + s;
              vertices.push_back(glm::vec3(t[0], t[1], t[2]));
              vertices.push_back(glm::vec3(t[0]+u[0], t[1]+u[1], t[2]+u[2]));
              vertices.push_back(glm::vec3(t[0]+u[0]+v[0], t[1]+u[1]+v[1], t[2]+u[2]+v[2]));
              vertices.push_back(glm::vec3(t[0], t[1], t[2]));
              vertices.push_back(glm::vec3(t[0]+v[0], t[1]+v[1], t[2]+v[2]));
              vertices.push_back(glm::vec3(t[0]+u[0]+v[0], t[1]+u[1]+v[1], t[2]+u[2]+v[2]));
            }
          }
        }
        n++;
      }
    }
  }
}

void makeVoxels(int *l, int *h, int (*f)(int, int, int), int *volume) {
  int n = 0;
  for(int k=l[2]; k<h[2]; k++) {
    for(int j=l[1]; j<h[1]; j++) {
      for(int i=l[0]; i<h[0]; i++) {
        volume[n] = (*f)(i,j,k);
        // fprintf(stderr, "n: %i, func: %i", n, volume[n]);
        n++;
      }
    }
  }
}

int cube_func(int i, int j, int k) {
  return 1;
}

int hole_func(int i, int j, int k) {
  return abs(i-7) > 3 || abs(j-7) > 3;
}

int hill_func(int i, int j, int k) {
  if (j <= 16 * exp(-(i*i + k*k) / 64))
    return 1;
}

int init_resources(void)
{
  // cube
  // int l[] = { 0, 0, 0 };
  // int h[] = { 16, 16, 16 };

  // hole
  // int l[] = { 0, 0, 0 };
  // int h[] = { 16, 16, 1 };

  // hill
  int l[] = { -16, 0, -16 };
  int h[] = { 16, 16, 16 };

  int d[] = { (h[0]-l[0]), (h[1]-l[1]), (h[2]-l[2]) };
  int size = d[0]*d[1]*d[2];
  int volume[size];
  makeVoxels(l, h, hill_func, volume);

  std::vector<glm::vec3> vertices;
  stupidMesh(volume, d, vertices);
  std::vector<glm::vec3> indexed_vertices;
  indexVBO(vertices, indices, indexed_vertices);

  glGenBuffers(1, &vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(indexed_vertices[0]), &indexed_vertices[0], GL_STATIC_DRAW);

  // for(int i=0; i<indices.size(); i++)
  //   fprintf(stderr, "Index: %d, x: %f, y: %f, z: %f\n", indices[i], indexed_vertices[indices[i]].x, indexed_vertices[indices[i]].y, indexed_vertices[indices[i]].z);

  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);

  GLint link_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader("cube.v.glsl", GL_VERTEX_SHADER)) == 0) return 0;
  if ((fs = create_shader("cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;
 
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    print_log(program);
    return 0;
  }

  const char* attribute_name = "coord3d";
  attribute_coord3d = glGetAttribLocation(program, attribute_name);
  if (attribute_coord3d == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  const char* uniform_name;
  uniform_name = "mvp";
  uniform_mvp = glGetUniformLocation(program, uniform_name);
  if (uniform_mvp == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  return 1;
}

void onIdle() {
  float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 15;
  glm::mat4 anim = \
    glm::rotate(glm::mat4(1.0f), angle*3.0f, glm::vec3(1, 0, 0)) *
    glm::rotate(glm::mat4(1.0f), angle*2.0f, glm::vec3(0, 1, 0)) *
    glm::rotate(glm::mat4(1.0f), angle*4.0f, glm::vec3(0, 0, 1));
  glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 1.0f, 20.0f);
  glm::mat4 mvp = projection * view * model * anim;

  glUseProgram(program);
  glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
  glutPostRedisplay();
}

void onReshape(int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, screen_width, screen_height);
}

void onDisplay()
{
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

  glUseProgram(program);
  glEnableVertexAttribArray(attribute_coord3d);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glVertexAttribPointer(
                        attribute_coord3d,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        0
                        );

  // glDrawArrays(GL_TRIANGLES, 0, sizeOfMesh);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

  glDisableVertexAttribArray(attribute_coord3d);
  glutSwapBuffers();
}

void free_resources()
{
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_vertices);
  glDeleteBuffers(1, &elementbuffer);
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow("Cube Engine");

  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK)
    {
      return EXIT_FAILURE;
    }

  if (1 == init_resources())
    {
      glutDisplayFunc(onDisplay);
      glutIdleFunc(onIdle);
      glutReshapeFunc(onReshape);
      glEnable(GL_BLEND);
      glEnable(GL_DEPTH_TEST);
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glutMainLoop();
    }

  free_resources();
}
