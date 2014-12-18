#include "OpenglIncludes.h"

extern GLuint program;
extern GLint attribute_v_coord;
extern GLint attribute_v_normal;
extern GLint uniform_m;
extern GLint uniform_v;
extern GLint uniform_p;
extern GLint uniform_m_3x3_inv_transp;
extern GLint uniform_v_inv ;

class ShaderLoader {
public:
	int loadShader(char* vshader_filename, char* fshader_filename);
private:
	GLint create_shader(const char* filename, GLenum type);
	char* file_read(const char* filename);
	glm::vec3 LightPos[1];
	glm::vec3 LightColor[1];
	
};