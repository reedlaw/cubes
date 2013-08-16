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

GLuint program, window;
GLint attribute_coord3d, attribute_v_normal, uniform_m, uniform_v, uniform_p, uniform_m_3x3_inv_transp;
GLuint vbo_cube_vertices, vbo_cube_colors, vbo_cube_texcoords, ibo_cube_elements, normalbuffer, elementbuffer;
std::vector<GLushort> indices;
int screen_width=800, screen_height=600;
int last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
int arcball_on = false;
glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -25.0));
int PreviousClock, Clock, deltaT;

struct PackedVertex{
  glm::vec3 pos;
  // glm::vec2 uv;
  glm::vec3 normal;
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
              std::vector<glm::vec3> & normals,

              std::vector<unsigned short> & indices,
              std::vector<glm::vec3> & indexed_verts,
              // std::vector<glm::vec2> & indexed_uvs,
              std::vector<glm::vec3> & indexed_normals)
{
  std::map<PackedVertex, unsigned short> VertexIndex;

  for (int i=0; i<verts.size(); i++){
    PackedVertex packed = {verts[i], normals[i]}; // , uvs[i]
    unsigned short index;
    bool found = get_similar_vertex(packed, VertexIndex, index);

    if (found){
      indices.push_back(index);
    } else {
      indexed_verts.push_back(verts[i]);
      // indexed_uvs.push_back(uvs[i]);
      indexed_normals.push_back(normals[i]);
      unsigned short newindex = (unsigned short)indexed_verts.size() - 1;
      indices.push_back(newindex);
      VertexIndex[packed] = newindex;
    }
  }
}


bool findVoxel(int i, int j, int k, int *volume, int *dimensions) {
  return volume[i + dimensions[0] * (j + dimensions[1] * k)]==1;
}

void greedyMesh(int *volume, int *dimensions, std::vector<glm::vec3> & vertices, std::vector<glm::vec3> & normals)
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
    for(i=0; i<dimensions[v]*dimensions[v]; i++) {
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
          if(mask[n]==1) {
            //Compute width
            for(w=1; mask[n+w] && i+w<dimensions[u]; w++) {}

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
                if(!mask[n+k+h*dimensions[u]]) {
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
            p1 = glm::vec3(x[0], x[1], x[2]);
            p2 = glm::vec3(x[0]+du[0], x[1]+du[1], x[2]+du[2]);
            p3 = glm::vec3(x[0]+du[0]+dv[0], x[1]+du[1]+dv[1], x[2]+du[2]+dv[2]);
            p4 = glm::vec3(x[0] +dv[0], x[1] +dv[1], x[2] +dv[2]);

            if(flip_normal){
              vertices.push_back(p3);
              vertices.push_back(p2);
              vertices.push_back(p1);
              vertices.push_back(p3);
              vertices.push_back(p1);
              vertices.push_back(p4);
            } else {
              vertices.push_back(p1);
              vertices.push_back(p2);
              vertices.push_back(p3);
              vertices.push_back(p4);
              vertices.push_back(p1);
              vertices.push_back(p3);
            }

            vecU = p2 - p1;
            vecV = p3 - p1;
            normal.x = (vecU.y * vecV.z) - (vecU.z * vecV.y);
            normal.y = (vecU.z * vecV.x) - (vecU.x * vecV.z);
            normal.z = (vecU.x * vecV.y) - (vecU.y * vecV.x);
            // }
            for(int o=0; o<6; o++) {
              normals.push_back(normal);
            }
            //Zero-out mask
            for(l=0; l<h; l++) {
              for(k=0; k<w; k++) {
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
}

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

void makeVoxels(int *l, int *h, int (*f)(int, int, int), int *volume) {
  int n = 0;
  for(int k=l[2]; k<h[2]; k++) {
    for(int j=l[1]; j<h[1]; j++) {
      for(int i=l[0]; i<h[0]; i++) {
        volume[n] = (*f)(i,j,k);
        n++;
      }
    }
  }
}

int cube_func(int i, int j, int k) {
  return 1;
}

int hole_func(int i, int j, int k) {
  if (abs(i-7) > 3 || abs(j-7) > 3)
    return 1;
}

int hill_func(int i, int j, int k) {
  if (j <= 16 * exp(-(i*i + k*k) / 64.0))
    return 1;
}

int anim_hill_func(int i, int j, int k) {
  float angle = glutGet(GLUT_ELAPSED_TIME) / 1000.0 * 15;  // base 15° per second
  if (j <= 16 * exp(-(i*i + k*k) / angle))
    return 1;
}


int init_resources(void)
{
  PreviousClock = glutGet(GLUT_ELAPSED_TIME);

  // cube
  int l[] = { 0, 0, 0 };
  int h[] = { 2, 2, 2 };

  // hole
  // int l[] = { 0, 0, 0 };
  // int h[] = { 16, 16, 1 };

  // hill
  // int l[] = { -16, 0, -16 };
  // int h[] = { 16, 16, 16 };

  int d[] = { (h[0]-l[0]), (h[1]-l[1]), (h[2]-l[2]) };
  int size = d[0]*d[1]*d[2];
  int volume[size];

  makeVoxels(l, h, cube_func, volume);

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  greedyMesh(volume, d, vertices, normals);
  std::vector<glm::vec3> indexed_vertices;
  std::vector<glm::vec3> indexed_normals;

  indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

  for(int i=0; i<indexed_normals.size(); i++) {
    fprintf(stderr, "v %f %f %f\n", indexed_vertices[i].x, indexed_vertices[i].y, indexed_vertices[i].z);
  }

  for(int i=0; i<indexed_normals.size(); i++) {
    fprintf(stderr, "vn %f %f %f\n", indexed_normals[i].x, indexed_normals[i].y, indexed_normals[i].z);
  }

  for(int i=0; i<indices.size(); i=i+3) {
    fprintf(stderr, "f %i//%i %i//%i %i//%i\n", indices[i]+1, indices[i]+1, indices[i+1]+1, indices[i+1]+1, indices[i+2]+1, indices[i+2]+1);
  }

  glGenBuffers(1, &vbo_cube_vertices);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(indexed_vertices[0]), &indexed_vertices[0], GL_STATIC_DRAW);

  glGenBuffers(1, &normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(indexed_normals[0]), &indexed_normals[0], GL_STATIC_DRAW);

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

  attribute_name = "v_normal";
  attribute_v_normal = glGetAttribLocation(program, attribute_name);
  if (attribute_v_normal == -1) {
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
  glm::mat4 view = glm::lookAt(glm::vec3(0.0, 2.0, 0.0), glm::vec3(0.0, 0.0, -10.0), glm::vec3(0.0, 1.0, 0.0));
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

  glUseProgram(program);

  // Clock = glutGet(GLUT_ELAPSED_TIME);
  // deltaT = Clock - PreviousClock;
  // if (deltaT < 35) {
  // } else {
  //   PreviousClock = Clock;
  //   // cube
  //   // int l[] = { 0, 0, 0 };
  //   // int h[] = { 16, 16, 16 };

  //   // hole
  //   // int l[] = { 0, 0, 0 };
  //   // int h[] = { 16, 16, 1 };

  //   // hill
  //   int l[] = { -16, 0, -16 };
  //   int h[] = { 16, 16, 16 };

  //   int d[] = { (h[0]-l[0]), (h[1]-l[1]), (h[2]-l[2]) };
  //   int size = d[0]*d[1]*d[2];
  //   int volume[size];

  //   makeVoxels(l, h, anim_hill_func, volume);

  //   std::vector<glm::vec3> vertices;
  //   std::vector<glm::vec3> normals;
  //   greedyMesh(volume, d, vertices, normals);
  //   std::vector<glm::vec3> indexed_vertices;
  //   std::vector<glm::vec3> indexed_normals;
  //   indices.clear();

  //   indexVBO(vertices, normals, indices, indexed_vertices, indexed_normals);

  //   glBindBuffer(GL_ARRAY_BUFFER, vbo_cube_vertices);
  //   glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size() * sizeof(indexed_vertices[0]), &indexed_vertices[0], GL_STATIC_DRAW);

  //   glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  //   glBufferData(GL_ARRAY_BUFFER, indexed_normals.size() * sizeof(indexed_normals[0]), &indexed_normals[0], GL_STATIC_DRAW);

  //   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  //   glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLushort), &indices[0], GL_STATIC_DRAW);
  // }

  glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(model)));
  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
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

  glEnableVertexAttribArray(attribute_v_normal);
  glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
  glVertexAttribPointer(
                        attribute_v_normal,
                        3,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
                        0
                        );

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
  int size; glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);

  glDisableVertexAttribArray(attribute_coord3d);
  glDisableVertexAttribArray(attribute_v_normal);
  glutSwapBuffers();
}

void free_resources()
{
  glDeleteProgram(program);
  glDeleteBuffers(1, &vbo_cube_vertices);
  glDeleteBuffers(1, &elementbuffer);
  glDeleteBuffers(1, &normalbuffer);
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
