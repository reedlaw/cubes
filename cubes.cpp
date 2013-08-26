#include "cubes.h"
#include "generators/cells.h"
#include "utils/shader_utils.h"
#include "meshers/greedy.h"
#include "meshers/stupid.h"
#include "meshers/surface_nets.h"

GLuint program, window;
GLint attribute_v_coord, attribute_v_normal, attribute_v_color, uniform_m, uniform_v, uniform_p, uniform_v_inv, uniform_m_3x3_inv_transp;
GLuint ibo_elements, elementbuffer;
std::vector<ushort> indices;
std::vector<Vertex> vertices;
int screen_width=800, screen_height=600;
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int arcball_on = false;
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -25.0));
int PreviousClock, Clock, deltaT;

int init_resources(void)
{
  PreviousClock = glutGet(GLUT_ELAPSED_TIME);

  std::vector<float> volume;

  // makeVoxels(l, h, sphere_func, volume);

  struct timespec tsi, tsf;

  int dimensions[3] = { 6, 6, 6 };
  makeCellComplex(volume);

  clock_gettime(CLOCK_MONOTONIC, &tsi);
  surfaceNets(volume, dimensions, vertices, indices);
  clock_gettime(CLOCK_MONOTONIC, &tsf);

  double elaps_s = difftime(tsf.tv_sec, tsi.tv_sec);
  long elaps_ns = tsf.tv_nsec - tsi.tv_nsec;

  fprintf(stderr, "surfaceNets takes %lf s\n", elaps_s + ((double)elaps_ns) / 1.0e9);

  glGenBuffers(1, &ibo_elements);
  glBindBuffer(GL_ARRAY_BUFFER, ibo_elements);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &elementbuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(ushort), &indices[0], GL_STATIC_DRAW);

  GLint link_ok = GL_FALSE;

  GLuint vs, fs;
  if ((vs = create_shader("shaders/cube.v.glsl", GL_VERTEX_SHADER)) == 0) return 0;
  if ((fs = create_shader("shaders/cube.f.glsl", GL_FRAGMENT_SHADER)) == 0) return 0;

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

  const char* attribute_name = "v_coord";
  attribute_v_coord = glGetAttribLocation(program, attribute_name);
  if (attribute_v_coord == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  attribute_name = "v_normal";
  attribute_v_normal = glGetAttribLocation(program, attribute_name);
  if (attribute_v_normal == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  attribute_name = "v_color";
  attribute_v_color = glGetAttribLocation(program, attribute_name);
  if (attribute_v_color == -1) {
    fprintf(stderr, "Could not bind attribute %s\n", attribute_name);
    return 0;
  }

  const char* uniform_name;
  uniform_name = "m";
  uniform_m = glGetUniformLocation(program, uniform_name);
  if (uniform_m == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  uniform_name = "v";
  uniform_v = glGetUniformLocation(program, uniform_name);
  if (uniform_v == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  uniform_name = "p";
  uniform_p = glGetUniformLocation(program, uniform_name);
  if (uniform_p == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  uniform_name = "v_inv";
  uniform_v_inv = glGetUniformLocation(program, uniform_name);
  if (uniform_v_inv == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  uniform_name = "m_3x3_inv_transp";
  uniform_m_3x3_inv_transp = glGetUniformLocation(program, uniform_name);
  if (uniform_m_3x3_inv_transp == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }

  return 1;
}

glm::vec3 get_arcball_vector(int x, int y)
{
  glm::vec3 P = glm::vec3(1.0*x/screen_width*2 - 1.0,
                          1.0*y/screen_height*2 - 1.0,
                          0);
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1*1)
    P.z = sqrt(1*1 - OP_squared);
  else
    P = glm::normalize(P);
  return P;
}

void onIdle() {
  glm::mat4 scale = glm::scale(glm::mat4(1.0f),glm::vec3(0.5f));
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -16.0), glm::vec3(0.0, 1.0, 0.0));
  glm::mat4 projection = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 1.0f, 100.0f);

  if (cur_mx != last_mx || cur_my != last_my) {
    glm::vec3 va = get_arcball_vector(last_mx, last_my);
    glm::vec3 vb = get_arcball_vector(cur_mx, cur_my);
    float angle = acos(std::min(1.0f, glm::dot(va, vb)));
    glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
    glm::mat3 camera2object = glm::inverse(glm::mat3(view) * glm::mat3(model));
    glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
    model = glm::rotate(model, glm::degrees(angle), axis_in_object_coord);
    last_mx = cur_mx;
    last_my = cur_my;
  }

  glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(model)));
  glm::mat4 v_inv = glm::inverse(view);

  glUseProgram(program);

  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
  glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(model * scale));
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(projection));
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

  glBindBuffer(GL_ARRAY_BUFFER, ibo_elements);

  glVertexAttribPointer(
                        attribute_v_coord,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        BUFFER_OFFSET(0)
                        );
  glEnableVertexAttribArray(attribute_v_coord);

  glVertexAttribPointer(
                        attribute_v_normal,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        BUFFER_OFFSET(sizeof(float)*3)
                        );
  glEnableVertexAttribArray(attribute_v_normal);

  glVertexAttribPointer(
                        attribute_v_color,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        sizeof(Vertex),
                        BUFFER_OFFSET(sizeof(float)*6)
                        );
  glEnableVertexAttribArray(attribute_v_color);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

  glutSwapBuffers();
}

void free_resources()
{
  glDisableVertexAttribArray(attribute_v_coord);
  glDisableVertexAttribArray(attribute_v_normal);
  glDisableVertexAttribArray(attribute_v_color);
  glDeleteProgram(program);
  glDeleteBuffers(1, &ibo_elements);
  glDeleteBuffers(1, &elementbuffer);
}

void onMouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
    arcball_on = true;
    last_mx = cur_mx = x;
    last_my = cur_my = y;
  } else {
    arcball_on = false;
  }
}

void onMotion(int x, int y)
{
  if (arcball_on) {
    cur_mx = x;
    cur_my = y;
  }
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key)
    {
    case 27:
      free_resources();
      glutDestroyWindow(window);
      exit(0);

      break;
    }

  glutPostRedisplay();
}

int main(int argc, char* argv[])
{
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(screen_width, screen_height);
  window = glutCreateWindow("Cube Engine");

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
      // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      glutMouseFunc(onMouse);
      glutMotionFunc(onMotion);
      glutKeyboardFunc(keyboard);
      glutMainLoop();
    }

  free_resources();
}
