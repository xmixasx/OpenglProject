#include "ViewObjectHandler.h"	

extern GLuint program;
extern GLint attribute_v_coord;
extern GLint attribute_v_normal;
extern GLint uniform_m;
extern GLint uniform_v;
extern GLint uniform_p;
extern GLint uniform_m_3x3_inv_transp;
extern GLint uniform_v_inv ;

ViewObjectHandler::ViewObjectHandler() {
	
	last_mx = 0, last_my = 0, cur_mx = 0, cur_my = 0;
	arcball_on = false;

	rotY_direction = 0, rotX_direction = 0, transZ_direction = 0, strife = 0;
	speed_factor = 1;
	transforms[MODE_CAMERA] = glm::lookAt(
		glm::vec3(0.0,  0.0, 4.0),   // eye
		glm::vec3(0.0,  0.0, 0.0),   // direction
		glm::vec3(0.0,  1.0, 0.0));  // up
	
	screen_width=800, screen_height=600;

	last_ticks = glutGet(GLUT_ELAPSED_TIME);
}

void ViewObjectHandler::setObject(Mesh *_input_mesh) {
	main_object = _input_mesh;
}

void ViewObjectHandler::onSpecial(int key, int x, int y) {
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
		//init_view();
		break;
	}
}

void ViewObjectHandler::onSpecialUp(int key, int x, int y) {
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


void ViewObjectHandler::onDisplay()
{
	if (!main_object)
		return;

	logic();
	draw();
	glutSwapBuffers();
}

void ViewObjectHandler::onMouse(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		arcball_on = true;
		last_mx = cur_mx = x;
		last_my = cur_my = y;
	} else {
		arcball_on = false;
	}
}

void ViewObjectHandler::onMotion(int x, int y) {
	if (arcball_on) {  // if left button is pressed
		cur_mx = x;
		cur_my = y;
	}
}

void ViewObjectHandler::onReshape(int width, int height) {
	screen_width = width;
	screen_height = height;
	glViewport(0, 0, screen_width, screen_height);
}


/**
* Get a normalized vector from the center of the virtual ball O to a
* point P on the virtual ball surface, such that P is aligned on
* screen's (X,Y) coordinates.  If (X,Y) is too far away from the
* sphere, return the nearest point on the virtual ball surface.
*/
glm::vec3 ViewObjectHandler::get_arcball_vector(int x, int y) {
	glm::vec3 P = glm::vec3(1.0*x/screen_width*2 - 1.0,
		1.0*y/screen_height*2 - 1.0,
		0);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1*1)
		P.z = sqrt(1*1 - OP_squared);  // Pythagore
	else
		P = glm::normalize(P);  // nearest point
	return P;
}

void ViewObjectHandler::logic() {

	/* Handle keyboard-based transformations */
	int delta_t = glutGet(GLUT_ELAPSED_TIME) - last_ticks;
	last_ticks = glutGet(GLUT_ELAPSED_TIME);

	float delta_transZ = transZ_direction * delta_t / 1000.0 * 5 * speed_factor;  // 5 units per second
	float delta_transX = 0, delta_transY = 0, delta_rotY = 0, delta_rotX = 0;
	if (strife) {
		delta_transX = rotY_direction * delta_t / 1000.0 * 3 * speed_factor;  // 3 units per second
		delta_transY = rotX_direction * delta_t / 1000.0 * 3 * speed_factor;  // 3 units per second
	} else {
		delta_rotY =  rotY_direction * delta_t / 1000.0 * 120 * speed_factor;  // 120° per second
		delta_rotX = -rotX_direction * delta_t / 1000.0 * 120 * speed_factor;  // 120° per second
	}

	if (view_mode == MODE_OBJECT) {
		main_object->object2world = glm::rotate(main_object->object2world, glm::radians(delta_rotY), glm::vec3(0.0, 1.0, 0.0));
		main_object->object2world = glm::rotate(main_object->object2world, glm::radians(delta_rotX), glm::vec3(1.0, 0.0, 0.0));
		main_object->object2world = glm::translate(main_object->object2world, glm::vec3(0.0, 0.0, delta_transZ));
	} else if (view_mode == MODE_CAMERA) {
		// Camera is reverse-facing, so reverse Z translation and X rotation.
		// Plus, the View matrix is the inverse of the camera2world (it's
		// world->camera), so we'll reverse the transformations.
		// Alternatively, imagine that you transform the world, instead of positioning the camera.
		if (strife) {
			transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(delta_transX, 0.0, 0.0)) * transforms[MODE_CAMERA];
		} else {
			glm::vec3 y_axis_world = glm::mat3(transforms[MODE_CAMERA]) * glm::vec3(0.0, 1.0, 0.0);
			transforms[MODE_CAMERA] = glm::rotate(glm::mat4(1.0), glm::radians(-delta_rotY), y_axis_world) * transforms[MODE_CAMERA];
		}

		if (strife)
			transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(0.0, delta_transY, 0.0)) * transforms[MODE_CAMERA];
		else
			transforms[MODE_CAMERA] = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, delta_transZ)) * transforms[MODE_CAMERA];

		transforms[MODE_CAMERA] = glm::rotate(glm::mat4(1.0), glm::radians(delta_rotX), glm::vec3(1.0, 0.0, 0.0)) * transforms[MODE_CAMERA];
	}

	/* Handle arcball */
	if (cur_mx != last_mx || cur_my != last_my) {
		glm::vec3 va = get_arcball_vector(last_mx, last_my);
		glm::vec3 vb = get_arcball_vector( cur_mx,  cur_my);
		float angle = acos(min(1.0f, glm::dot(va, vb)));
		glm::vec3 axis_in_camera_coord = glm::cross(va, vb);
		glm::mat3 camera2object = glm::inverse(glm::mat3(transforms[MODE_CAMERA]) * glm::mat3(main_object->object2world));
		glm::vec3 axis_in_object_coord = camera2object * axis_in_camera_coord;
		main_object->object2world = glm::rotate(main_object->object2world, angle, axis_in_object_coord);
		last_mx = cur_mx*1.05;
		last_my = cur_my*1.05;
	}

	// Model
	// Set in onDisplay() - cf. main_object.object2world

	// View
	glm::mat4 world2camera = transforms[MODE_CAMERA];

	// Projection
	glm::mat4 camera2screen = glm::perspective(45.0f, 1.0f*screen_width/screen_height, 0.1f, 100.0f);

	glUseProgram(program);
	glUniformMatrix4fv(uniform_v, 1, GL_FALSE, glm::value_ptr(world2camera));
	glUniformMatrix4fv(uniform_p, 1, GL_FALSE, glm::value_ptr(camera2screen));

	glm::mat4 v_inv = glm::inverse(world2camera);
	glUniformMatrix4fv(uniform_v_inv, 1, GL_FALSE, glm::value_ptr(v_inv));

	glutPostRedisplay();
}

void ViewObjectHandler::draw() {
	glClearColor(0.45, 0.45, 0.45, 1.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);

	main_object->draw();
}