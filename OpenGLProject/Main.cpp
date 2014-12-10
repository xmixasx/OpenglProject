//#include <glut.h>
//#include <vector>
//#include <iostream>
//#include <fstream>
//#include <cstdlib>
//#include <stdlib.h>
//#include <iomanip>
//#include <sstream>
//#include <stdio.h>
//#include <GL\glew.h>
//#include <Windows.h>
//#include <glm\glm\glm.hpp>
//
//
//int screen_width = 800, screen_height = 800;
//GLuint program;
//GLuint attribute_v_coord;
//GLuint attribute_v_normal;
//GLuint uniform_m, uniform_v, uniform_p;
//
//
//struct Mesh{
//std::vector<glm::vec4> vertices;
//std::vector<glm::vec3> normals;
//std::vector<GLushort> elements;
//glm::mat4 odjtoworld;
//}
//mesh;
//using namespace std;
//
//void load_obj(const char* filename,std::vector<glm::vec4> &vertices, std::vector<glm::vec3> &normals, std::vector<GLushort> &elements) {
//  std::ifstream in(filename, std::ios::in);
//  if (!in) { std::cerr << "Cannot open " << filename << std::endl; exit(1); }
// 
//  std::string line;
//  while (getline(in, line)) {
//    if (line.substr(0,2) == "v ") {
//      istringstream s(line.substr(2));
//      glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0f;
//      vertices.push_back(v);
//    }  else if (line.substr(0,2) == "f ") {
//      istringstream s(line.substr(2));
//      GLushort a,b,c;
//      s >> a; s >> b; s >> c;
//      a--; b--; c--;
//      elements.push_back(a); elements.push_back(b); elements.push_back(c);
//    }
//    else if (line[0] == '#') { /* ignoring this line */ }
//    else { /* ignoring this line */ }
//  }
// 
//  normals.resize(vertices.size(), glm::vec3(0.0, 0.0, 0.0));
//  for (int i = 0; i < elements.size(); i+=3) {
//    GLushort ia = elements[i];
//    GLushort ib = elements[i+1];
//    GLushort ic = elements[i+2];
//    glm::vec3 normal = glm::normalize(glm::cross(
//      glm::vec3(vertices[ib]) - glm::vec3(vertices[ia]),
//      glm::vec3(vertices[ic]) - glm::vec3(vertices[ia])));
//    normals[ia] = normals[ib] = normals[ic] = normal;
//  }
//}
//
//int main(int argc, char **argv){
//	load_obj("suzanne.obj", vertices, normals, elements);
//	 glEnableVertexAttribArray(attribute_v_coord);
//	 glEnableVertexAttribArrayARB(1);
//  // Describe our vertices array to OpenGL (it can't guess its format automatically)
//  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_vertices);
//  glVertexAttribPointer(
//    attribute_v_coord,  // attribute
//    4,                  // number of elements per vertex, here (x,y,z,w)
//    GL_FLOAT,           // the type of each element
//    GL_FALSE,           // take our values as-is
//    0,                  // no extra data between each position
//    0                   // offset of first element
//  );
// 
//  glBindBuffer(GL_ARRAY_BUFFER, vbo_mesh_normals);
//  glVertexAttribPointer(
//    attribute_v_normal, // attribute
//    3,                  // number of elements per vertex, here (x,y,z)
//    GL_FLOAT,           // the type of each element
//    GL_FALSE,           // take our values as-is
//    0,                  // no extra data between each position
//    0                   // offset of` first element
//  );
// 
//  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_mesh_elements);
//  int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);  
//  glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
//  glutInitWindowSize( 600, 600 );
//	glutInit( &argc, argv );
//	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
//	 glMatrixMode( GL_PROJECTION );
//	glutCreateWindow( argv[ 0 ] );
//	init();
//	glutReshapeFunc( reshape );
//	glutMouseFunc(mouse);
//	glutSpecialFunc(keyboard);
//	glutDisplayFunc( display );
//	glutMotionFunc(Action);
//	glutMainLoop();
//	return 0;
//}

/**
 * This file is in the public domain.
 * Contributors: Sylvain Beucler
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <glut.h>
/* GLM */
// #define GLM_MESSAGES
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
 
int screen_width=800, screen_height=600;
GLuint program;
GLint attribute_v_coord;
GLint attribute_v_normal;
GLint uniform_m, uniform_v, uniform_p;
GLint uniform_m_3x3_inv_transp, uniform_v_inv;
using namespace std;
 
enum MODES { MODE_OBJECT, MODE_CAMERA, MODE_LIGHT, MODE_LAST } view_mode;
int rotY_direction = 0, rotX_direction = 0, transZ_direction = 0;
glm::mat4 transforms[MODE_LAST];
int last_ticks = 0;
 
struct mesh {
  vector<glm::vec4> vertices;
  vector<glm::vec3> normals;
  vector<GLushort> elements;
  vector<GLulong> elements_normals;
  vector<GLushort> elements_textures;
  glm::mat4 object2world;
} mesh;
 
/**
 * Store all the file's contents in memory, useful to pass shaders
 * source code to OpenGL
 */
char* file_read(const char* filename)
{
  FILE* in = fopen(filename, "rb");
  if (in == NULL) return NULL;
 
  int res_size = BUFSIZ;
  char* res = (char*)malloc(res_size);
  int nb_read_total = 0;
 
  while (!feof(in) && !ferror(in)) {
    if (nb_read_total + BUFSIZ > res_size) {
      if (res_size > 10*1024*1024) break;
      res_size = res_size * 2;
      res = (char*)realloc(res, res_size);
    }
    char* p_res = res + nb_read_total;
    nb_read_total += fread(p_res, 1, BUFSIZ, in);
  }
  
  fclose(in);
  res = (char*)realloc(res, nb_read_total + 1);
  res[nb_read_total] = '\0';
  return res;
}
 
/**
 * Display compilation errors from the OpenGL shader compiler
 */
void print_log(GLuint object)
{
  GLint log_length = 0;
  if (glIsShader(object))
    glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else if (glIsProgram(object))
    glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
  else {
    fprintf(stderr, "printlog: Not a shader or a program\n");
    return;
  }
 
  char* log = (char*)malloc(log_length);
 
  if (glIsShader(object))
    glGetShaderInfoLog(object, log_length, NULL, log);
  else if (glIsProgram(object))
    glGetProgramInfoLog(object, log_length, NULL, log);
 
  fprintf(stderr, "%s", log);
  free(log);
}
 
/**
 * Compile the shader from file 'filename', with error handling
 */
GLint create_shader(const char* filename, GLenum type)
{
  const GLchar* source = file_read(filename);
  if (source == NULL) {
    fprintf(stderr, "Error opening %s: ", filename); perror("");
    return 0;
  }
  GLuint res = glCreateShader(type);
  glShaderSource(res, 1, &source, NULL);
  free((void*)source);
 
  glCompileShader(res);
  GLint compile_ok = GL_FALSE;
  glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
  if (compile_ok == GL_FALSE) {
    fprintf(stderr, "%s:", filename);
    print_log(res);
    glDeleteShader(res);
    return 0;
  }
 
  return res;
}
 
void load_obj(const char* filename, struct mesh* mesh) {
  ifstream in(filename, ios::in);
  if (!in) { cerr << "Cannot open " << filename << endl; exit(1); }
 
  string line;
  while (getline(in, line)) {
    if (line.substr(0,2) == "v ") {
      istringstream s(line.substr(2));
      glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
      mesh->vertices.push_back(v);
 //   }  else if (line.substr(0,3) == "vn ") {
 //     istringstream s(line.substr(3));
 //     GLushort a,b,c;
 //     glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;

 //     /*s >> a; s >> b; s >> c;
 //     a--; b--; c--;*/
 //     mesh->normals.push_back(v); 
	}else if (line.substr(0,2) == "f ") {
      istringstream s(line.substr(2));
       string a,b,c;
	   int g;
      while(s >> a) {
		  
		  g = abs(atoi(a.c_str()));
     // a--; b--; c--;
      mesh->elements.push_back(g-1); 
	  }
    }
	
    else if (line[0] == '#') { /* ignoring this line */ }
    else { /* ignoring this line */ }
  }
 
  mesh->normals.reserve(mesh->vertices.size());
  
  for (int i = 0; i < mesh->elements.size()-4; i+=3) {
	  GLulong ia = mesh->elements[i];
      GLulong ib = mesh->elements[i+1];
      GLulong ic = mesh->elements[i+2];
      glm::vec3 normal = glm::normalize(glm::cross(
      glm::vec3(mesh->vertices[ib]) - glm::vec3(mesh->vertices[ia]),
      glm::vec3(mesh->vertices[ic]) - glm::vec3(mesh->vertices[ia])));
     // mesh->normals[ia] = mesh->normals[ib] = mesh->normals[ic] = normal;
  }
}
 
int init_resources(char* model_filename, char* vshader_filename, char* fshader_filename)
{
  GLint link_ok = GL_FALSE;
 
  load_obj(model_filename, &mesh);
 
  GLuint vs, fs;
  if ((vs = create_shader(vshader_filename, GL_VERTEX_SHADER))   == 0) return 0;
  if ((fs = create_shader(fshader_filename, GL_FRAGMENT_SHADER)) == 0) return 0;
 
  program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    return 0;
  }
 
  const char* attribute_name;
  attribute_name = "v_coord";
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
  /*uniform_name = "v_inv";
  uniform_v_inv = glGetUniformLocation(program, uniform_name);
  if (uniform_v_inv == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }*/
 
  return 1;
}
 
void init_view() {
  mesh.object2world = glm::mat4(1.0);
  transforms[MODE_CAMERA] = glm::inverse(glm::lookAt(
    glm::vec3(0.0,  0.0, 4.0),   // eye
    glm::vec3(0.0,  0.0, 0.0),   // direction
    glm::vec3(0.0,  1.0, 0.0)));  // up
}
 
void onSpecial(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_F1:
    view_mode = MODE_OBJECT;
    break;
  case GLUT_KEY_F2:
    view_mode = MODE_CAMERA;
    break;
  case GLUT_KEY_F3:
    view_mode = MODE_LIGHT;
    break;
  case GLUT_KEY_LEFT:
    rotY_direction = 1;
    break;
  case GLUT_KEY_RIGHT:
    rotY_direction = -1;
    break;
  case GLUT_KEY_UP:
    rotX_direction = -1;
    break;
  case GLUT_KEY_DOWN:
    rotX_direction = 1;
    break;
  case GLUT_KEY_PAGE_UP:
    transZ_direction = 1;
    break;
  case GLUT_KEY_PAGE_DOWN:
    transZ_direction = -1;
    break;
  case GLUT_KEY_HOME:
    init_view();
    break;
  }
}
 
void onSpecialUp(int key, int x, int y) {
  switch (key) {
  case GLUT_KEY_LEFT:
  case GLUT_KEY_RIGHT:
    rotY_direction = 0;
    break;
  case GLUT_KEY_UP:
  case GLUT_KEY_DOWN:
    rotX_direction = 0;
    break;
  case GLUT_KEY_PAGE_UP:
  case GLUT_KEY_PAGE_DOWN:
    transZ_direction = 0;
    break;
  }
}
 
void idle() {
  int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
  last_ticks = glutGet(GLUT_ELAPSED_TIME);
  float delta_rotY = rotY_direction * delta_t / 1000.0 * 120;  // 120° per second
  float delta_rotX = rotX_direction * delta_t / 1000.0 * 120;  // 120° per second;
  float delta_transZ = transZ_direction * delta_t / 1000.0 * 3;  // 3 units per second;
  
  if (view_mode == MODE_OBJECT) {
    mesh.object2world = glm::rotate(mesh.object2world, delta_rotY, glm::vec3(0.0, 1.0, 0.0));
    mesh.object2world = glm::rotate(mesh.object2world, delta_rotX, glm::vec3(1.0, 0.0, 0.0));
    mesh.object2world = glm::translate(mesh.object2world, glm::vec3(0.0, 0.0, delta_transZ));
  } else if (view_mode == MODE_CAMERA) {
    transforms[MODE_CAMERA] = glm::rotate(transforms[MODE_CAMERA], delta_rotY, glm::vec3(0.0, 1.0, 0.0));
    transforms[MODE_CAMERA] = glm::rotate(transforms[MODE_CAMERA], delta_rotX, glm::vec3(1.0, 0.0, 0.0));
    transforms[MODE_CAMERA] = glm::translate(transforms[MODE_CAMERA], glm::vec3(0.0, 0.0, delta_transZ));
  }
 
  // Model
  // Set in onDisplay() - cf. mesh.object2world
 
  // View
  glm::mat4 world2camera = glm::inverse(transforms[MODE_CAMERA]);
 
  // Projection
  glm::mat4 camera2screen = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 100.0f);
 
  glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(world2camera));
  glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(camera2screen));
 
  glm::mat4 v_inv = glm::inverse(world2camera);
  glUniformMatrix3fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));
  glutPostRedisplay();
}
 
void display()
{
  glUseProgram(program);
 
  glClearColor(0.45, 0.45, 0.45, 1.0);
  glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
 
  glEnableVertexAttribArray(attribute_v_coord);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glVertexAttribPointer(
    attribute_v_coord,  // attribute
    3,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
	sizeof(mesh.vertices[0]),  // size of each vertex
    mesh.vertices.data()       // pointer to the C array
  );
 
  glEnableVertexAttribArray(attribute_v_normal);
  // Describe our vertices array to OpenGL (it can't guess its format automatically)
  glVertexAttribPointer(
    attribute_v_normal, // attribute
    4,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    sizeof(mesh.normals[0]),  // size of each vertex
    mesh.normals.data()       // pointer to the C array
  );
 
  /* Apply object's transformation matrix */
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(mesh.object2world));
  glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(mesh.object2world)));
  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
 
  /* Push each element in buffer_vertices to the vertex shader */
  glDrawElements(GL_TRIANGLES_ADJACENCY, mesh.elements.size()*sizeof(unsigned short), GL_UNSIGNED_SHORT, mesh.elements.data());
 
 
  glm::vec4 ground_vertices[] = {
    glm::vec4(-2.0, 0.0, -2.0, 1.0),
    glm::vec4( 2.0, 0.0, -2.0, 1.0),
    glm::vec4(-2.0, 0.0, 2.0, 1.0),
    glm::vec4(-2.0, 0.0,  2.0, 1.0),
    glm::vec4( 2.0, 0.0, -2.0, 1.0),
    glm::vec4( 2.0, 0.0,  2.0, 1.0)
  };
  glm::vec3 ground_normals[] = {
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
    glm::vec3(0.0, 1.0, 0.0),
  };
  glVertexAttribPointer(
    attribute_v_coord,  // attribute
    4,                  // number of elements per vertex, here (x,y,z,w)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    sizeof(ground_vertices[0]),  // size of each vertex
    ground_vertices              // pointer to the C array
  );
  glVertexAttribPointer(
    attribute_v_normal, // attribute
    3,                  // number of elements per vertex, here (x,y,z)
    GL_FLOAT,           // the type of each element
    GL_FALSE,           // take our values as-is
    sizeof(ground_normals[0]),  // size of each vertex
    ground_normals              // pointer to the C array
  );
  glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0)));
  m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(1.0)));
  glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
  glDrawArrays(GL_TRIANGLES, 0, 6);
 
  glDisableVertexAttribArray(attribute_v_coord);
  glDisableVertexAttribArray(attribute_v_normal);
  glutSwapBuffers();
}
 
void onReshape(int width, int height) {
  screen_width = width;
  screen_height = height;
  glViewport(0, 0, screen_width, screen_height);
}
 
void free_resources()
{
  glDeleteProgram(program);
}
 
 
int main(int argc, char* argv[]) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
  glutInitWindowSize(screen_width, screen_height);
  glutCreateWindow("Rotating Suzanne");
 
  GLenum glew_status = glewInit();
  if (glew_status != GLEW_OK) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
    return 1;
  }
 
  /*if (argc != 4) {
    fprintf(stderr, "Usage: %s model.obj vertex_shader.v.glsl fragment_shader.f.glsl\n", argv[0]);
    exit(0);
  }*/
 
  init_resources("claudia.obj", "Vertex.glsl", "Fragment.glsl");
    init_view();
	cout<<"begin"<< endl;
	//for (int i = 0; i< mesh.elements.size(); i+=3)
	//	cout/*<<mesh.vertices[i].x <<" "<<mesh.vertices[i].y<<" "<<mesh.vertices[i].z<< " "<<*/<<mesh.elements[i]<<" "<<mesh.elements[i+1]<<" "<< mesh.elements[i+2]<<endl;
    glutDisplayFunc(display);
    glutSpecialFunc(onSpecial);
    glutSpecialUpFunc(onSpecialUp);
    glutReshapeFunc(onReshape);
    glutIdleFunc(idle);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    last_ticks = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();
  
 
  free_resources();
  return 0;
}