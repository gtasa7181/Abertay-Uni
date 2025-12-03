// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	mesh = nullptr;
	shader = nullptr;

	for (int i = 0; i < bnumLights; i++)
	{
		lights[i] = nullptr;
	}
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);

	// Create Mesh object and shader object
	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	textureMgr->loadTexture(L"brick", L"res/brick1.dds");
	shader = new LightShader(renderer->getDevice(), hwnd);

	// Configure point light.
	lights[0] = new Light();
	lights[0]->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	lights[0]->setDiffuseColour(2.0f, 2.0f, 2.0f, 1.0f);
	lights[0]->setSpecularColour(1.0f, 1.0f, 1.0f, 1.0f);
	lights[0]->setSpecularPower(64.0f);
	lights[0]->setDirection(0.0f, -1.0f, 0.0f);
	lights[0]->setPosition(0.0f, 0.0f, 0.0f);
	//light->setDirection(1.0f, 0.0f, 0.0f);

	lights[1] = new Light();
	lights[1]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setDiffuseColour(1.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setSpecularColour(1.0f, 0.0f, 0.0f, 1.0f);
	lights[1]->setSpecularPower(32.0f);
	lights[1]->setPosition(-50.0f, 10.0f, 50.0f);


	lights[2] = new Light();
	lights[2]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
	lights[2]->setDiffuseColour(0.0f, 1.0f, 0.0f, 1.0f);
	lights[2]->setSpecularColour(0.0f, 1.0f, 0.0f, 1.0f);
	lights[2]->setSpecularPower(32.0f);
	lights[2]->setPosition(50.0f, 10.0f, -50.0f);
}


App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the light object.

	for (int i = 0; i < bnumLights; i++)
	{
		if (lights[i])
		{
			delete lights[i];
			lights[i] = 0;
		}
	}

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	// Send geometry data, set shader parameters, render object with shader
	mesh->sendData(renderer->getDeviceContext());
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"brick"), lights, bnumLights, camera);
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());

	// Render GUI
	gui();

	// Swap the buffers
	renderer->endScene();

	return true;
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
	ImGui::Text("Press E to raise camera \nto see the sphere being rendered");

	ImGui::Text("Light Controls");
	for (int i = 0; i < bnumLights; ++i)
	{
		Light* light = lights[i];

		ImGui::PushID(i);

		const char* light_name = (i == 0) ? "Light 0 (Directional - White)" : (i == 1) ? "Light 1 (Point - Red)" : "Light 2 (Point - Blue)";
		{
			if (ImGui::CollapsingHeader(light_name))
			{
				XMFLOAT4 ambient = light->getAmbientColour();
				XMFLOAT4 diffuse = light->getDiffuseColour();
				XMFLOAT4 specular = light->getSpecularColour();

				XMFLOAT3 lightVec;
				if (i == 0) lightVec = light->getDirection();
				else lightVec = light->getPosition();

				float specularPower = light->getSpecularPower();

				if (ImGui::ColorEdit4("Ambient", (float*)&ambient))
				{
					light->setAmbientColour(ambient.x, ambient.y, ambient.z, ambient.w);
				}

				if (ImGui::ColorEdit4("Diffuse", (float*)&diffuse))
				{
					light->setDiffuseColour(diffuse.x, diffuse.y, diffuse.z, diffuse.w);
				}

				if (ImGui::ColorEdit4("Specular", (float*)&specular))
				{
					light->setSpecularColour(specular.x, specular.y, specular.z, specular.w);
				}

				if (i == 0) {
					if (ImGui::SliderFloat3("Direction", (float*)&lightVec, -1.0f, 1.0f))
					{
						XMVECTOR dirVec = XMVector3Normalize(XMLoadFloat3(&lightVec));
						XMStoreFloat3(&lightVec, dirVec);
						light->setDirection(lightVec.x, lightVec.y, lightVec.z);
					}
				}
				else
				{
					if (ImGui::SliderFloat3("Position", (float*)&lightVec, -50.0f, 50.0f))
					{
						light->setPosition(lightVec.x, lightVec.y, lightVec.z);
					}
				}

				if (ImGui::SliderFloat("Specular Power", &specularPower, 1.0f, 128.0f))
				{
					light->setSpecularPower(specularPower);
				}
			}
		}
		ImGui::PopID();
	}
	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}