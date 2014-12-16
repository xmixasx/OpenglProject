#include "Rendering.h"

Rendering::Rendering()
{
}
void Rendering::Draw()
{
	mDraw.Draw(mShaderLoader);
}
Rendering::~Rendering()
{
}