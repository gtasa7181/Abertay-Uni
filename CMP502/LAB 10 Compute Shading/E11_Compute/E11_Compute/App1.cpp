// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	plane = nullptr;
	teapot = nullptr;
	texShader = nullptr;
	horBlur = nullptr;
	vertBlur = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create geometry object
	plane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	teapot = new AModel(renderer->getDevice(), "res/teapot.obj");
	ortho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);
	
	// Load and initialise required shader handle objects.
	texShader = new TextureShader(renderer->getDevice(), hwnd);
	horBlur = new ComputeBlurHor(renderer->getDevice(), hwnd, screenWidth, screenHeight);
	vertBlur = new ComputeBlurVert(renderer->getDevice(), hwnd, screenWidth, screenHeight);

	// initialise RT objects. One needed to store unblurred version of scene.
	unblurredRT = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, 0.1f, 100.f);

	//Load textures.
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");


}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (plane)
	{
		delete plane;
		plane = 0;
	}
	if (teapot)
	{
		delete teapot;
		teapot = 0;
	}
	if (ortho)
	{
		delete ortho;
		ortho = 0;
	}
	if (texShader)
	{
		delete texShader;
		texShader = 0;
	}
	if (horBlur)
	{
		delete horBlur;
		horBlur = 0;
	}
	if (vertBlur)
	{
		delete vertBlur;
		vertBlur = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{
	// unblurred scene
	firstPass();

	// run compute blur
	computePass();

	// render result to screen
	finalPass();

	return true;
}

void App1::firstPass()
{
	// Set the render target to be the render to texture.
	unblurredRT->setRenderTarget(renderer->getDeviceContext());
	unblurredRT->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// get the world, view, and projection matrices from the camera and d3d objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	worldMatrix = XMMatrixTranslation(-50.f, 0.f, -10.f);
	// Render floor
	plane->sendData(renderer->getDeviceContext());
	texShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"));
	texShader->render(renderer->getDeviceContext(), plane->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();
	worldMatrix = XMMatrixTranslation(0.f, 7.f, 5.f);
	XMMATRIX scaleMatrix = XMMatrixScaling(0.5f, 0.5f, 0.5f);
	worldMatrix = XMMatrixMultiply(worldMatrix, scaleMatrix);
	// Render model
	teapot->sendData(renderer->getDeviceContext());
	texShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"));
	texShader->render(renderer->getDeviceContext(), teapot->getIndexCount());

	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::computePass()
{
	// horiontal pass using unblurred copy of the scene
	horBlur->setShaderParameters(renderer->getDeviceContext(), unblurredRT->getShaderResourceView());
	horBlur->compute(renderer->getDeviceContext(), ceil(sWidth / 256.f), sHeight, 1);
	horBlur->unbind(renderer->getDeviceContext());

	// Vertical blur using the horizontal blur result
	vertBlur->setShaderParameters(renderer->getDeviceContext(), horBlur->getSRV());
	vertBlur->compute(renderer->getDeviceContext(), sWidth, ceil((float)sHeight / 256.f), 1);
	vertBlur->unbind(renderer->getDeviceContext());
}

void App1::finalPass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();// ortho matrix for 2D rendering
	XMMATRIX baseViewMatrix = camera->getOrthoViewMatrix();

	renderer->setZBuffer(false);
	ortho->sendData(renderer->getDeviceContext());
	texShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, vertBlur->getSRV());
	texShader->render(renderer->getDeviceContext(), ortho->getIndexCount());
	renderer->setZBuffer(true);

	// Render GUI
	gui();

	// Present the rendered scene to the screen.
	renderer->endScene();
}

void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

