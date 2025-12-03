// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "textureShader.h"
#include "ComputeBlurHor.h"
#include "ComputeBlurVert.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void firstPass();
	void computePass();
	void finalPass();
	void gui();

private:
	PlaneMesh* plane;
	AModel* teapot; 
	OrthoMesh* ortho;
	TextureShader* texShader;
	ComputeBlurHor* horBlur;
	ComputeBlurVert* vertBlur;
	RenderTexture* unblurredRT;
};

#endif