//#ifndef MESH_H
//#define MESH_H

#include <vector>

#include "OpenglIncludes.h"

using namespace std;

extern GLint attribute_v_coord;
extern GLint attribute_v_normal;
extern GLint uniform_m;
extern GLint uniform_v;
extern GLint uniform_p;
extern GLint uniform_m_3x3_inv_transp;
extern GLint uniform_v_inv ;

class Mesh {
private:
  GLuint vbo_vertices, vbo_normals, ibo_elements;
public:
  vector<glm::vec4> vertices;
  vector<glm::vec3> normals;
  vector<GLushort> elements;
  glm::mat4 object2world;
 
  Mesh() : vbo_vertices(0), vbo_normals(0), ibo_elements(0), object2world(glm::mat4(1)) {}
  ~Mesh();
 
  void loadFromFile(char* model_filename);

  /**
   * Store object vertices, normals and/or elements in graphic card
   * buffers
   */
  void upload();
 
  /**
   * Draw the object
   */
  void draw();
 
  /**
   * Draw object bounding box
   */
  void draw_bbox();
};

//#endif