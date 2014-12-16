#include "ShaderLoader.h"


ShaderLoader::ShaderLoader(){

GLint attribute_v_coord = -1;
	 attribute_v_normal = -1;
	 uniform_m = -1;
	 uniform_v = -1;
	 uniform_p = -1;
	 uniform_m_3x3_inv_transp = -1;
	 uniform_v_inv = -1;
	 link_ok = GL_FALSE;
	 validate_ok = GL_FALSE;
}

GLuint ShaderLoader:: create_shader(const char* filename, GLenum type)
	{
	  const GLchar* source = file_read(filename);
	  if (source == NULL) {
		fprintf(stderr, "Error opening %s: ", filename); perror("");
		return 0;
	  }
	  GLuint shader = glCreateShader(type);
	  glShaderSource(shader, 1, &source, NULL);
	  free((void*)source);
 
	  glCompileShader(shader);
	  GLint compile_ok = GL_FALSE;
	  glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_ok);
	  if (compile_ok == GL_FALSE) {
		fprintf(stderr, "%s:", filename);
		//print_log(res);
		glDeleteShader(shader);
		return 0;
	  }
 
	  return shader;
	}
char* ShaderLoader::file_read(const char* filename)
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

bool ShaderLoader::shader_init(char* vshader, char* fshader){
	if ((vs = create_shader(vshader, GL_VERTEX_SHADER))   == 0) return 0;
	if ((fs = create_shader(fshader, GL_FRAGMENT_SHADER)) == 0) return 0;
	program = glCreateProgram();
  glAttachShader(program, vs);
  glAttachShader(program, fs);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
  if (!link_ok) {
    fprintf(stderr, "glLinkProgram:");
    //print_log(program);
    return 0;
  }
  glValidateProgram(program);
  glGetProgramiv(program, GL_VALIDATE_STATUS, &validate_ok);
  if (!validate_ok) {
    fprintf(stderr, "glValidateProgram:");
   // print_log(program);
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
  uniform_name = "v_inv";
  uniform_v_inv = glGetUniformLocation(program, uniform_name);
  if (uniform_v_inv == -1) {
    fprintf(stderr, "Could not bind uniform %s\n", uniform_name);
    return 0;
  }
 
  //fps_start = glutGet(GLUT_ELAPSED_TIME);
 
  return 1;
}
