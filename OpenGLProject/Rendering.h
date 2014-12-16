#include "Transform.h"
#include "Mesh.h"
#include "Drawer.h"
#include "ShaderLoader.h"

class Rendering
{
public:
	Rendering();
	~Rendering();
	void Draw();

private:
	//Drawer mDraw;
	//Transform mTransform;
	ShaderLoader mShaderLoader;
};

