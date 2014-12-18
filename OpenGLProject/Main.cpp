#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "ShaderLoader.h"
#include "ViewObjectHandler.h"

int screen_width=800, screen_height=600;

Mesh *main_object = new Mesh();
ViewObjectHandler *obj_handler = new ViewObjectHandler();

void init_view() {
	main_object->object2world = glm::mat4(1);
}

void onSpecial(int key, int x, int y) {obj_handler->onSpecial( key,  x,  y);}

void onSpecialUp(int key, int x, int y) {obj_handler->onSpecialUp( key,  x,  y);}

void onDisplay() {obj_handler->onDisplay();}

void onMouse(int button, int state, int x, int y) {obj_handler->onMouse( button,  state,  x,  y);}

void onMotion(int x, int y) {obj_handler->onMotion(x,y);}

void onReshape(int width, int height) {obj_handler->onReshape(width,  height);}

void free_resources()
{
	glDeleteProgram(program);
	delete main_object;
	delete obj_handler;
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA|GLUT_ALPHA|GLUT_DOUBLE|GLUT_DEPTH);
	glutInitWindowSize(screen_width, screen_height);
	glutCreateWindow("OBJ viewer");

	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		fprintf(stderr, "Error: %s\n", glewGetErrorString(glew_status));
		return 1;
	}

	if (!GLEW_VERSION_2_0) {
		fprintf(stderr, "Error: your graphic card does not support OpenGL 2.0\n");
		return 1;
	}

	ShaderLoader simple_shader;
	simple_shader.loadShader("Vertex_i.glsl", "Fragment2.glsl");

	main_object->loadFromFile("some.obj");
	obj_handler->setObject(main_object);

	init_view();
	glutDisplayFunc(onDisplay);
	glutSpecialFunc(onSpecial);
	glutSpecialUpFunc(onSpecialUp);
	glutMouseFunc(onMouse);
	glutMotionFunc(onMotion);
	glutReshapeFunc(onReshape);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_DEPTH_TEST);
	glutMainLoop();
	free_resources();
	return 0;
}