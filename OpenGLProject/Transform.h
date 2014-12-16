#include <GL\glew.h>
#include <glut.h>

class Transform {
	static int rotY_direction = 0, rotX_direction = 0, transZ_direction = 0, strife = 0;
	static float speed_factor = 1;
	static void onSpecial(int key, int x, int y) {
	
	  int modifiers = glutGetModifiers();
	  if ((modifiers & GLUT_ACTIVE_ALT) == GLUT_ACTIVE_ALT)
		strife = 1;
	  else
		strife = 0;
 
	  if ((modifiers & GLUT_ACTIVE_SHIFT) == GLUT_ACTIVE_SHIFT)
		speed_factor = 0.1;
	  else
		speed_factor = 1;
 
	  switch (key) {  
	  case GLUT_KEY_LEFT:
		rotY_direction = 1;
		break;
	  case GLUT_KEY_RIGHT:
		rotY_direction = -1;
		break;
	  case GLUT_KEY_UP:
		transZ_direction = 1;
		break;
	  case GLUT_KEY_DOWN:
		transZ_direction = -1;
		break;
	  case GLUT_KEY_PAGE_UP:
		rotX_direction = -1;
		break;
	  case GLUT_KEY_PAGE_DOWN:
		rotX_direction = 1;
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
		transZ_direction = 0;
		break;
	  case GLUT_KEY_PAGE_UP:
	  case GLUT_KEY_PAGE_DOWN:
		rotX_direction = 0;
		break;
	  }
	}
}