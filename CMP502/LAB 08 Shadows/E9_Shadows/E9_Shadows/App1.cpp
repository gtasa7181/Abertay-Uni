// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	// Initialize ocean parameters
	oceanTime = 0.0f;
	waveAmplitude = 1.5f;     // Really tall waves
	waveFrequency = 1.0f;     // More wave peaks
	waveSpeed = 1.0f;         // Fast moving
	// Initialize time-of-day system
	timeOfDay = 12.0f;         // Start at noon
	timeSpeed = 0.0f;          // Start paused so you can control it
	autoAdvanceTime = false;   // Manual control initially


}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 50);  // Match ocean size
	model = new AModel(renderer->getDevice(), "res/teapot.obj");
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");

	// Create ocean shader and mesh
	waterShader = new WaterShader(renderer->getDevice(), hwnd);
	oceanMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext(), 50); // small ocean plane

	// initial shaders
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);

	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// This is your shadow map
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Configure directional light
	light = new Light();
	light->setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
	light->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	light->setDirection(0.0f, -0.7f, 0.7f);
	light->setPosition(0.f, 0.f, -10.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.

}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Update ocean animation time
	oceanTime += timer->getTime();

	// Update time-of-day lighting
	updateTimeOfDay();


	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

void App1::updateTimeOfDay()
{
	// Auto-advance time if enabled
	if (autoAdvanceTime)
	{
		timeOfDay += timer->getTime() * timeSpeed;

		// Wrap time to 0-24 range
		if (timeOfDay >= 24.0f)
			timeOfDay -= 24.0f;
		else if (timeOfDay < 0.0f)
			timeOfDay += 24.0f;
	}

	// Calculate light color and intensity based on time
	XMFLOAT4 lightColor;
	XMFLOAT4 ambientColor;
	float lightIntensity = 1.0f;

	// Time periods:
	// 0-5: Night (dark blue)
	// 5-7: Dawn (orange/pink)
	// 7-17: Day (bright white/yellow)
	// 17-19: Dusk (orange/red)
	// 19-24: Night (dark blue)

	if (timeOfDay >= 0.0f && timeOfDay < 5.0f)
	{
		// Night - Dark blue light
		lightColor = XMFLOAT4(0.2f, 0.2f, 0.4f, 1.0f);
		ambientColor = XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f);
		lightIntensity = 0.3f;
	}
	else if (timeOfDay >= 5.0f && timeOfDay < 7.0f)
	{
		// Dawn - Orange/pink transition
		float t = (timeOfDay - 5.0f) / 2.0f;  // 0 to 1
		lightColor = XMFLOAT4(
			0.2f + t * 0.8f,  // 0.2 -> 1.0
			0.2f + t * 0.6f,  // 0.2 -> 0.8
			0.4f + t * 0.2f,  // 0.4 -> 0.6
			1.0f
		);
		ambientColor = XMFLOAT4(
			0.05f + t * 0.20f,
			0.05f + t * 0.15f,
			0.1f + t * 0.15f,
			1.0f
		);
		lightIntensity = 0.3f + t * 0.7f;
	}
	else if (timeOfDay >= 7.0f && timeOfDay < 17.0f)
	{
		// Day - Bright white/yellow
		lightColor = XMFLOAT4(1.0f, 1.0f, 0.9f, 1.0f);
		ambientColor = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
		lightIntensity = 1.0f;
	}
	else if (timeOfDay >= 17.0f && timeOfDay < 19.0f)
	{
		// Dusk - Orange/red sunset
		float t = (timeOfDay - 17.0f) / 2.0f;  // 0 to 1
		lightColor = XMFLOAT4(
			1.0f,
			0.8f - t * 0.6f,  // 0.8 -> 0.2
			0.6f - t * 0.2f,  // 0.6 -> 0.4
			1.0f
		);
		ambientColor = XMFLOAT4(
			0.25f - t * 0.20f,
			0.20f - t * 0.15f,
			0.25f - t * 0.15f,
			1.0f
		);
		lightIntensity = 1.0f - t * 0.7f;
	}
	else  // 19-24: Night
	{
		// Night - Dark blue light
		lightColor = XMFLOAT4(0.2f, 0.2f, 0.4f, 1.0f);
		ambientColor = XMFLOAT4(0.05f, 0.05f, 0.1f, 1.0f);
		lightIntensity = 0.3f;
	}

	// Apply the calculated colors to the light
	light->setAmbientColour(ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
	light->setDiffuseColour(lightColor.x * lightIntensity,
		lightColor.y * lightIntensity,
		lightColor.z * lightIntensity,
		lightColor.w);
}

bool App1::render()
{

	// Perform depth pass
	depthPass();
	// Render scene
	finalPass();

	return true;
}

void App1::depthPass()
{
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	mesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	model->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), model->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::finalPass()
{
	// Clear the scene
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);
	camera->update();

	// Get matrices
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	// === RENDER SEABED/OCEAN FLOOR (with underwater darkness) ===
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(10.0f, 1.0f, 10.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, -15.0f, 10.0f));

	// CREATE A DARKER UNDERWATER LIGHT for seabed
	Light* underwaterLight = new Light();
	underwaterLight->setAmbientColour(0.05f, 0.08f, 0.12f, 1.0f);  // Very dark blue ambient
	underwaterLight->setDiffuseColour(0.18f, 0.25f, 0.35f, 1.0f);  // Dim blue-tinted diffuse
	XMFLOAT3 mainLightDir = light->getDirection();
	XMFLOAT3 mainLightPos = light->getPosition();
	underwaterLight->setDirection(mainLightDir.x, mainLightDir.y, mainLightDir.z);  // Same direction as main light
	underwaterLight->setPosition(mainLightPos.x, mainLightPos.y, mainLightPos.z);  // Same position
	underwaterLight->generateOrthoMatrix(100.0f, 100.0f, 0.1f, 100.f);
	underwaterLight->generateViewMatrix();

	mesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"brick"), shadowMap->getDepthMapSRV(), underwaterLight);
	shadowShader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	delete underwaterLight;  // Clean up temporary light

	// RENDER OCEAN (keep your existing ocean rendering code)
	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixScaling(10.0f, 1.0f, 10.0f);
	worldMatrix = XMMatrixMultiply(worldMatrix, XMMatrixTranslation(0.0f, -5.0f, 10.0f));
	oceanMesh->sendData(renderer->getDeviceContext());
	waterShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix,
		textureMgr->getTexture(L"brick"), light, oceanTime, waveAmplitude, waveFrequency, waveSpeed);
	waterShader->render(renderer->getDeviceContext(), oceanMesh->getIndexCount());

	gui();
	renderer->endScene();
}


void App1::gui()
{
	// Shader setup
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build ALL UI first
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Time-of-Day Controls
	ImGui::Begin("Time of Day");

	// Time slider (0-24 hours)
	ImGui::SliderFloat("Time", &timeOfDay, 0.0f, 24.0f, "%.1f hours");

	// Auto-advance toggle
	ImGui::Checkbox("Auto Advance", &autoAdvanceTime);

	// Time speed control (only shown if auto-advance is enabled)
	if (autoAdvanceTime)
	{
		ImGui::SliderFloat("Time Speed", &timeSpeed, 0.0f, 5.0f, "%.2fx");
	}

	// Show current time period
	if (timeOfDay >= 5.0f && timeOfDay < 7.0f)
		ImGui::Text("Current: Dawn");
	else if (timeOfDay >= 7.0f && timeOfDay < 17.0f)
		ImGui::Text("Current: Day");
	else if (timeOfDay >= 17.0f && timeOfDay < 19.0f)
		ImGui::Text("Current: Dusk");
	else
		ImGui::Text("Current: Night");

	ImGui::End();

	// Wave Controls
	ImGui::Begin("Ocean Controls");
	ImGui::SliderFloat("Wave Amplitude", &waveAmplitude, 0.0f, 5.0f);
	ImGui::SliderFloat("Wave Frequency", &waveFrequency, 0.0f, 3.0f);
	ImGui::SliderFloat("Wave Speed", &waveSpeed, 0.0f, 3.0f);
	ImGui::End();

	// Render LAST - after all UI is built
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
