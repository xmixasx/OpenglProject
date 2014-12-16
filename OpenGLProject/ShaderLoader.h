#include <GL\glew.h>
#include <iostream>


class ShaderLoader
{
public:
	ShaderLoader();
	GLuint create_shader(const char* filename, GLenum type);
	~ShaderLoader();
	char* file_read(const char* filename);
	bool shader_init(char* vshader, char* fshader);
	int getUniform_m(){return this->uniform_m;};
	int getUniform_v(){return this->uniform_v;}
	int getUniform_uniform_m_3x3_inv_transp(){return this->uniform_m_3x3_inv_transp;}
	GLuint program;

private:
	GLuint vs, fs;
	GLint attribute_v_coord ;
	GLint attribute_v_normal;
	GLint uniform_m , uniform_v, uniform_p;
	GLint uniform_m_3x3_inv_transp , uniform_v_inv ;
	GLint link_ok;
	GLint validate_ok;


};

ShaderLoader::ShaderLoader()
{
}

ShaderLoader::~ShaderLoader()
{
}

