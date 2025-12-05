// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"	// include dxframework
#include "TextureShader.h"
#include "ShadowShader.h"
#include "DepthShader.h"
#include "WaterShader.h"


class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	void updateTimeOfDay();
	bool render();
	void depthPass();
	void finalPass();
	void gui();

private:
	TextureShader* textureShader;
	PlaneMesh* mesh;

	Light* light;
	AModel* model;
	ShadowShader* shadowShader;
	DepthShader* depthShader;

	ShadowMap* shadowMap;

	// Ocean water
	WaterShader* waterShader;
	PlaneMesh* oceanMesh;
	float oceanTime;

	// Wave parameters (for UI control later)
	float waveAmplitude;
	float waveFrequency;
	float waveSpeed;
	// Time-of-day system
	float timeOfDay;           // Current time (0.0 = midnight, 12.0 = noon, 24.0 = midnight)
	float timeSpeed;           // How fast time advances (0.0 = paused, 1.0 = normal)
	bool autoAdvanceTime;      // Whether time advances automatically

};

#endif