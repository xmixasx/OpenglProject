#include "LoadMesh.h"

LoadMesh::LoadMesh(const char* filename, Mesh& mesh)
{
		 ifstream in(filename, ios::in);
	  if (!in) 
	  { 
		  cerr << "Cannot open " << filename << endl; 
		  exit(1); 
	  }
	  vector<int> nb_seen;
 
	  string line;
	  while (getline(in, line)) {
		if (line.substr(0,2) == "v ") {
		  istringstream s(line.substr(2));
		  glm::vec4 v; s >> v.x; s >> v.y; s >> v.z; v.w = 1.0;
		  mesh.vertices.push_back(v);
		}  else if (line.substr(0,2) == "f ") {
		  istringstream s(line.substr(2));
		  GLushort a,b,c;
		  s >> a; s >> b; s >> c;
		  a--; b--; c--;
		  mesh.elements.push_back(a); mesh.elements.push_back(b); mesh.elements.push_back(c);
		}
		else if (line[0] == '#') { /* ignoring this line */ }
		else { /* ignoring this line */ }
	  }
 
	  mesh.normals.resize(mesh.vertices.size(), glm::vec3(0.0, 0.0, 0.0));
	  nb_seen.resize(mesh.vertices.size(), 0);
	  for (unsigned int i = 0; i < mesh.elements.size(); i+=3) {
		GLushort ia = mesh.elements[i];
		GLushort ib = mesh.elements[i+1];
		GLushort ic = mesh.elements[i+2];
		glm::vec3 normal = glm::normalize(glm::cross(
		  glm::vec3(mesh.vertices[ib]) - glm::vec3(mesh.vertices[ia]),
		  glm::vec3(mesh.vertices[ic]) - glm::vec3(mesh.vertices[ia])));
 
		int v[3];  v[0] = ia;  v[1] = ib;  v[2] = ic;
		for (int j = 0; j < 3; j++) {
		  GLushort cur_v = v[j];
		  nb_seen[cur_v]++;
		  if (nb_seen[cur_v] == 1) {
		mesh.normals[cur_v] = normal;
		  } else {
		// average
		mesh.normals[cur_v].x = mesh.normals[cur_v].x * (1.0 - 1.0/nb_seen[cur_v]) + normal.x * 1.0/nb_seen[cur_v];
		mesh.normals[cur_v].y = mesh.normals[cur_v].y * (1.0 - 1.0/nb_seen[cur_v]) + normal.y * 1.0/nb_seen[cur_v];
		mesh.normals[cur_v].z = mesh.normals[cur_v].z * (1.0 - 1.0/nb_seen[cur_v]) + normal.z * 1.0/nb_seen[cur_v];
		mesh.normals[cur_v] = glm::normalize(mesh.normals[cur_v]);
		  }
		}
	  }
	  mesh.upload();
}

int LoadMesh::init_resources(Mesh* ground, Mesh * light_bbox, Mesh* obj_mesh){
	
  // mesh position initialized in init_view()
 
  for (int i = -GROUND_SIZE/2; i < GROUND_SIZE/2; i++) {
    for (int j = -GROUND_SIZE/2; j < GROUND_SIZE/2; j++) {
      ground->vertices.push_back(glm::vec4(i,   0.0,  j+1, 1.0));
      ground->vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
      ground->vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
      ground->vertices.push_back(glm::vec4(i,   0.0,  j,   1.0));
      ground->vertices.push_back(glm::vec4(i+1, 0.0,  j+1, 1.0));
      ground->vertices.push_back(glm::vec4(i+1, 0.0,  j,   1.0));
      for (unsigned int k = 0; k < 6; k++)
    ground->normals.push_back(glm::vec3(0.0, 1.0, 0.0));
    }
  }
 
  glm::vec3 light_position = glm::vec3(0.0,  1.0,  2.0);
  light_bbox->vertices.push_back(glm::vec4(-0.1, -0.1, -0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4( 0.1, -0.1, -0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4( 0.1,  0.1, -0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4(-0.1,  0.1, -0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4(-0.1, -0.1,  0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4( 0.1, -0.1,  0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4( 0.1,  0.1,  0.1, 0.0));
  light_bbox->vertices.push_back(glm::vec4(-0.1,  0.1,  0.1, 0.0));
  light_bbox->object2world = glm::translate(glm::mat4(1), light_position);
 
  obj_mesh.upload();
  ground->upload();
  light_bbox->upload();
}
