#include <fstream>
#include <iostream>
#include <sstream>


#include "Mesh.h"

  Mesh::~Mesh() {
    if (vbo_vertices != 0)
      glDeleteBuffers(1, &vbo_vertices);
    if (vbo_normals != 0)
      glDeleteBuffers(1, &vbo_normals);
    if (ibo_elements != 0)
      glDeleteBuffers(1, &ibo_elements);
  }

  void Mesh::loadFromFile(char* model_filename) {

  	ifstream in(model_filename, ios::in);
	 
	if (!in) { cerr << "Cannot open " << model_filename << endl; exit(1); }
	vector<int> nb_seen;

	string line;
	while (getline(in, line)) {
		if (line.substr(0,2) == "v ") {
			istringstream s(line.substr(2));
			glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
			this->vertices.push_back(v);
		}  
		else if (line.substr(0,3) == "vn ") {
			istringstream s(line.substr(2));
			glm::vec3 v; s >> v.x; s >> v.y; s >> v.z;			
			this->normals.push_back(v); 
		}
		 else if (line.substr(0,2) == "f ") {
			istringstream s(line.substr(2));
			string a;
			while (s >> a){
			int g = abs(atoi(a.c_str()));
			this->elements.push_back(g-1); }
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	}
	if (this->normals.size() == 0){
	this->normals.resize(this->vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	nb_seen.resize(this->vertices.size(), 0);
	for (unsigned int i = 0; i < this->elements.size()-3; i+=3) {
		GLushort ia = this->elements[i];
		GLushort ib = this->elements[i+1];
		GLushort ic = this->elements[i+2];
		glm::vec3 normal = glm::normalize(glm::cross(
			glm::vec3(this->vertices[ib]) - glm::vec3(this->vertices[ia]),
			glm::vec3(this->vertices[ic]) - glm::vec3(this->vertices[ia])));

		int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
		for (int j = 0; j < 3; j++) {
			GLushort cur_v = v[j];
			nb_seen[cur_v]++;
			if (nb_seen[cur_v] == 1) {
				this->normals[cur_v] = normal;
			} else {
				// average
				this->normals[cur_v].x = this->normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
				this->normals[cur_v].y = this->normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
				this->normals[cur_v].z = this->normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
				this->normals[cur_v] = glm::normalize(this->normals[cur_v]);
			}
		}
	}
	}
	upload();
  }

/**
   * Store object vertices, normals and/or elements in graphic card
   * buffers
   */
  void Mesh::upload() {
    if (this->vertices.size() > 0) {
      glGenBuffers(1, &this->vbo_vertices);
      glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
      glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(this->vertices[0]),
           this->vertices.data(), GL_STATIC_DRAW);
    }
    
    if (this->normals.size() > 0) {
      glGenBuffers(1, &this->vbo_normals);
      glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
      glBufferData(GL_ARRAY_BUFFER, this->normals.size() * sizeof(this->normals[0]),
           this->normals.data(), GL_STATIC_DRAW);
    }
    
    if (this->elements.size() > 0) {
      glGenBuffers(1, &this->ibo_elements);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->elements.size() * sizeof(this->elements[0]),
           this->elements.data(), GL_STATIC_DRAW);
    }
  }
 
  /**
   * Draw the object
   */
  void Mesh::draw() {
    //if (this->vbo_vertices != 0) {
    //  glEnableVertexAttribArray(attribute_v_coord);
    //  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_vertices);
    //  glVertexAttribPointer(
    //    attribute_v_coord,  // attribute
    //    4,                  // number of elements per vertex, here (x,y,z,w)
    //    GL_FLOAT,           // the type of each element
    //    GL_FALSE,           // take our values as-is
    //    0,                  // no extra data between each position
    //    0                   // offset of first element
    //  );
    //}
 
    //if (this->vbo_normals != 0) {
    //  glEnableVertexAttribArray(attribute_v_normal);
    //  glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
    //  glVertexAttribPointer(
    //    attribute_v_normal, // attribute
    //    3,                  // number of elements per vertex, here (x,y,z)
    //    GL_FLOAT,           // the type of each element
    //    GL_FALSE,           // take our values as-is
    //    0,                  // no extra data between each position
    //    0                   // offset of first element
    //  );
    //}
    //
	 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);

	glBindBuffer(GL_ARRAY_BUFFER,this->vbo_vertices);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(float)*4, 0 );

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo_normals);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(float)*3, 0 );


	glDrawElements(GL_TRIANGLES, this->elements.size()*sizeof(float) , GL_UNSIGNED_INT, 0);


    /* Apply object's transformation matrix */
    glUniformMatrix4fv(uniform_m, 1, GL_FALSE, glm::value_ptr(this->object2world));
    /* Transform normal vectors with transpose of inverse of upper left
       3x3 model matrix (ex-gl_NormalMatrix): */
    glm::mat3 m_3x3_inv_transp = glm::transpose(glm::inverse(glm::mat3(this->object2world)));
    glUniformMatrix3fv(uniform_m_3x3_inv_transp, 1, GL_FALSE, glm::value_ptr(m_3x3_inv_transp));
    
    /* Push each element in buffer_vertices to the vertex shader */
    if (this->ibo_elements != 0) {
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo_elements);
      int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
      glDrawElements(GL_TRIANGLES, size/sizeof(GLushort), GL_UNSIGNED_SHORT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, this->vertices.size());
    }
 
  
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
 
  /**
   * Draw object bounding box
   */
 