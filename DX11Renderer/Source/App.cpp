#include "App.h"
#include "Utils/Utils.h"

namespace DX11Renderer
{
	App::App(bool fullscreen)
	{
		m_fullscreen = fullscreen;
	}

	bool App::Init(int screenWidth, int screenHeight, HWND hwnd)
	{
		m_renderer = new Renderer();

		RenderSettings rs;
		rs.FULL_SCREEN = m_fullscreen;
		bool result = m_renderer->Init(screenWidth, screenHeight, hwnd, rs);
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

		if (m_mesh)
		{
			m_mesh->Shutdown();
			Utils::SafeDel(m_mesh);
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
		m_camera->SetTranslation(0.0f, 0.0f, -5.0f);
		m_camera->LookAt(0.0f, 0.0f, 0.0f);

		m_mesh = new Mesh();
		result = m_mesh->Init(m_renderer->GetDevice(), m_renderer->GetDeviceContext(), "..\\..\\DX11Renderer\\Resources\\Textures\\stone01.tga");
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the model object.", L"Error", MB_OK);
			return false;
		}

		m_forwardRenderPass = new ForwardRenderPass();
		result = m_forwardRenderPass->Init(m_renderer->GetDevice(), hwnd);
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
			return false;
		}

		return true;
	}

	bool App::Frame()
	{
		bool result = Render();
		if (!result)
		{
			return false;
		}

		return true;
	}

	bool App::Render()
	{
		// Clear buffers
		m_renderer->BeginScene();

		XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

		// Generate the view matrix based on the camera's position.
		m_camera->Update();

		m_mesh->GetWorldMatrix(worldMatrix);
		m_camera->GetViewMatrix(viewMatrix);
		m_camera->GetProjectionMatrix(projectionMatrix);

		m_mesh->SetBuffers(m_renderer->GetDeviceContext());

		bool result = m_forwardRenderPass->Render(m_renderer->GetDeviceContext(), m_mesh->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_mesh->GetTexture()->GetTextureView());
		if (!result)
		{
			return false;
		}

		// Present the rendered scene to screen
		m_renderer->EndScene();

		return true;
	}
}
