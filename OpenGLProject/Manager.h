#include "LoadMesh.h"
#include "Rendering.h"


class Manager
{
public:
	Manager();
	void LoadMeneged(const char* aFilename);
	void Mouse();
	void Draw();
	~Manager();

private:
	Mesh mMesh;
	Rendering* mRendering;

};

Manager::Manager()
{
	mRendering = new Rendering();
}

Manager::~Manager()
{
}

void Manager::LoadMeneged(const char* aFilename)
{
	load_obj(aFilename, mMesh);
}

void  Manager::Draw()
{
	mRendering.Draw();
}