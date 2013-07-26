#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader_utils.h"

GLuint program;
GLint attribute_coord3d, attribute_v_color, attribute_texcoord, uniform_mvp;
GLuint vbo_cube_vertices, vbo_cube_colors, vbo_cube_texcoords, ibo_cube_elements;
int screen_width=800, screen_height=600;
int sizeOfMesh = 0;

std::vector<glm::vec4> stupidMesh(int *volume, int *dimensions)
{
  std::vector<glm::vec4> vertices;
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
              vertices.push_back(glm::vec4(t[0], t[1], t[2], 1.0));
              vertices.push_back(glm::vec4(t[0]+u[0], t[1]+u[1], t[2]+u[2], 1.0));
              vertices.push_back(glm::vec4(t[0]+u[0]+v[0], t[1]+u[1]+v[1], t[2]+u[2]+v[2], 1.0));
              vertices.push_back(glm::vec4(t[0]+v[0], t[1]+v[1], t[2]+v[2], 1.0));
            }
          }
        }
      }
    }
  }
  return vertices;
}

int init_resources(void)
{
  // int volume[] = { 1, 1, 1, 1, 1, 1, 1, 1 };
  // int dimensions[] = { 2, 2, 2 };
  int volume[] = { 1 };
  int dimensions[] = { 1, 1, 1 };
  std::vector<glm::vec4> cube_vertices = stupidMesh(volume, dimensions);
  sizeOfMesh = cube_vertices.size();
  glGenBuffers(1, &vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, cube_vertices.size() * sizeof(cube_vertices[0]), cube_vertices.data(), GL_STATIC_DRAW);

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
  glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 1.0f, 10.0f);
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
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        0
                        );

  glDrawArrays(GL_TRIANGLES, 0, sizeOfMesh);

  glDisableVertexAttribArray(attribute_coord3d);
  glutSwapBuffers();
}

void free_resources()
{
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_vertices);
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
  return EXIT_SUCCESS;
}
