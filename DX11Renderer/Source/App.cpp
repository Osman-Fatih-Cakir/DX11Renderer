#include "App.h"
#include "Utils/Utils.h"

#include "InputManager.h"
#include "Globals.h"

namespace DX11Renderer
{
	App::App()
	{
	}

	bool App::Init(int screenWidth, int screenHeight, HWND hwnd)
	{
		m_renderer = new Renderer();

		bool result = m_renderer->Init(screenWidth, screenHeight, hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize Directx 11", L"Error", MB_OK);
			return false;
		}

		result = InitRenderer(hwnd, screenWidth, screenHeight);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void App::Shutdown()
	{
		if (m_forwardRenderPass)
		{
			m_forwardRenderPass->Shutdown();
			Utils::SafeDel(m_forwardRenderPass);
		}

		if (m_grassRenderPass)
		{
			m_grassRenderPass->Shutdown();
			Utils::SafeDel(m_grassRenderPass);
		}

		if (m_grassMesh)
		{
			m_grassMesh->Shutdown();
			Utils::SafeDel(m_grassMesh);
		}

		if (m_camera)
		{
			Utils::SafeDel(m_camera);
		}

		m_renderer->Shutdown();
		Utils::SafeDel(m_renderer);
	}

	bool App::InitRenderer(HWND hwnd, int screenWidth, int screenHeight)
	{
		bool result;

		m_camera = new Camera();
		m_camera->Init(PI / 4.0f, (float)screenWidth / screenHeight, 0.3f, 1000.0f);

		m_grassMesh = new GrassMesh();
		result = m_grassMesh->Init(m_renderer->GetDevice(), m_renderer->GetDeviceContext(), nullptr);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}

		m_forwardRenderPass = new ForwardRenderPass();
		result = m_forwardRenderPass->Init(m_renderer->GetDevice(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the forward render pass.", L"Error", MB_OK);
			return false;
		}

		m_grassRenderPass = new GrassRenderPass();
		result = m_grassRenderPass->Init(m_renderer->GetDevice(), m_renderer->GetDeviceContext(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the grass render pass.", L"Error", MB_OK);
			return false;
		}

		return true;
	}

	bool App::Frame(float deltaTime)
	{
		if (g_inputManager->KeyClick(Key::F))
		{
			m_freeCameraActive = !m_freeCameraActive;
		}

		if (g_inputManager->KeyClick(Key::Q))
		{
			m_windType = (m_windType + 1) % 2;
		}

		bool result = Render(deltaTime);
		if (!result)
		{
			return false;
		}

		m_totalTime += (UINT)(deltaTime * 10.0f);

		return true;
	}

	bool App::Render(float deltaTime)
	{
		// Clear buffers
		m_renderer->BeginScene();

		XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

		// Generate the view matrix based on the camera's position.
		if (m_freeCameraActive)
			m_camera->Update(deltaTime);

		m_grassMesh->GetWorldMatrix(worldMatrix);
		m_camera->GetViewMatrix(viewMatrix);
		m_camera->GetProjectionMatrix(projectionMatrix);

		m_grassMesh->SetBuffers(m_renderer->GetDeviceContext());

		if (!m_freeCameraActive)
			m_lastMouseXY = { g_inputManager->RelMouseX(), g_inputManager->RelMouseY() };
		else
			m_lastMouseXY = m_initRelMousePos;

		// Draw 9 tiles
		for (int i = 0; i < 5; ++i)
		{
			for (int j = 0; j < 5; ++j)
			{
				const XMFLOAT4 tilePos = { -8.0f + i * 4, 0.0f, -8.0f + j * 4, 0.0f };
				const XMUINT2 tileCoord = { (UINT)i , (UINT)j };

				bool result = m_grassRenderPass->Render(m_renderer->GetDeviceContext(), m_grassMesh->GetIndexCount(), viewMatrix, projectionMatrix, tileCoord, tilePos, m_totalTime, m_lastMouseXY, m_camera->GetPosition(), m_windType);
				if (!result)
				{
					return false;
				}
			}
		}

		// Present the rendered scene to screen
		m_renderer->EndScene();

		return true;
	}
}
