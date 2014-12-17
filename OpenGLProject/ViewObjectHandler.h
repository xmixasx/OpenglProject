 
#include "Mesh.h"

class ViewObjectHandler {
public:
	ViewObjectHandler();

	void setObject(Mesh *_input_mesh);

	//callbacks:
	void onSpecial(int key, int x, int y);

	void onSpecialUp(int key, int x, int y);

	void onDisplay();

	void onMouse(int button, int state, int x, int y);

	void onMotion(int x, int y);

	void onReshape(int width, int height);
private:

	/**
	* Get a normalized vector from the center of the virtual ball O to a
	* point P on the virtual ball surface, such that P is aligned on
	* screen's (X,Y) coordinates.  If (X,Y) is too far away from the
	* sphere, return the nearest point on the virtual ball surface.
	*/
	glm::vec3 get_arcball_vector(int x, int y);

	void logic() ;

	void draw();
private:

	Mesh *main_object;

	int last_mx; 
	int last_my; 
	int cur_mx; 
	int cur_my;

	bool arcball_on;

	enum MODES { MODE_OBJECT, MODE_CAMERA, MODE_LIGHT, MODE_LAST } view_mode;
	glm::mat4 transforms[MODE_LAST];

	int rotY_direction; 
	int rotX_direction; 
	int transZ_direction; 
	int strife;
	float speed_factor;
	int last_ticks;

	int screen_width, screen_height;
};