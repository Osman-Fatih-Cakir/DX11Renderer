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

		m_initialized = true;
		return true;
	}

	void App::Shutdown()
	{
		if (m_initialized)
		{
			m_renderer->Shutdown();
			SafeDel(m_renderer);
		}
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

		// Present the rendered scene to screen
		m_renderer->EndScene();

		return true;
	}
}
