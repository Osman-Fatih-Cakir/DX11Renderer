#include "Main.h"
#include "Utils/Utils.h"
#include "Globals.h"
#include "Keys.h"

namespace DX11Renderer
{
	bool Main::Init()
	{
		// Get an external pointer to this object
		MAIN = this;

		int screenWidth, screenHeight;
		bool result;

		screenWidth = 0;
		screenHeight = 0;

		m_fullscreen = false;
		m_app = new App(m_fullscreen);

		g_inputManager = new InputManager();
		g_inputManager->Init();

		InitWindows(screenWidth, screenHeight);

		result = m_app->Init(screenWidth, screenHeight, m_hwnd);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void Main::Shutdown()
	{
		if (m_app)
		{
			m_app->Shutdown();
			Utils::SafeDel(m_app);
		}

		// Release the input class object.
		if (g_inputManager)
		{
			Utils::SafeDel(g_inputManager);
		}

		// Shutdown the window.
		ShutdownWindows();

		return;
	}

	void Main::Run()
	{
		MSG msg;

		ZeroMemory(&msg, sizeof(MSG));

		bool done = false;
		while (!done)
		{
			// Handle the windows messages.
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}

			// If windows signals to end the application then exit out.
			if (msg.message == WM_QUIT)
			{
				done = true;
			}
			else
			{
				bool result = Frame();
				if (!result)
				{
					done = true;
				}
			}
		}
	}

	bool Main::Frame()
	{
		if (g_inputManager->KeyDown(Key::Escape))
		{
			return false;
		}

		bool result = m_app->Frame();
		if (!result)
		{
			return false;
		}

		return true;
	}

	LRESULT CALLBACK Main::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		switch (umsg)
		{
		case WM_KEYDOWN:
		{
			g_inputManager->SetKeyDown((unsigned int)wparam);
			return 0;
		}

		case WM_KEYUP:
		{
			g_inputManager->SetKeyUp((unsigned int)wparam);
			return 0;
		}

		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
		}
	}

	void Main::InitWindows(int& screenWidth, int& screenHeight)
	{
		// Get the instance of this application.
		m_hInstance = GetModuleHandle(NULL);

		// Setup the windows class with default settings.
		WNDCLASSEX wc;
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WndProc;
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hInstance = m_hInstance;
		wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wc.hIconSm = wc.hIcon;
		wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.lpszMenuName = NULL;
		wc.lpszClassName = m_applicationName;
		wc.cbSize = sizeof(WNDCLASSEX);

		// Register the window class.
		RegisterClassEx(&wc);

		// Determine the resolution of the clients desktop screen.
		screenWidth = GetSystemMetrics(SM_CXSCREEN);
		screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
		DEVMODE dmScreenSettings;
		int posX, posY;
		if (m_fullscreen)
		{
			// If full screen set the screen to maximum size of the users desktop and 32bit.
			memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
			dmScreenSettings.dmSize = sizeof(dmScreenSettings);
			dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
			dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
			dmScreenSettings.dmBitsPerPel = 32;
			dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

			// Change the display settings to full screen.
			ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

			// Set the position of the window to the top left corner.
			posX = posY = 0;
		}
		else
		{
			// If windowed then set it to 800x600 resolution.
			screenWidth = 1280;
			screenHeight = 720;

			// Place the window in the middle of the screen.
			posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
			posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
		}

		// Create the window with the screen settings and get the handle to it.
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			posX, posY, screenWidth, screenHeight, NULL, NULL, m_hInstance, NULL);

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);

		// Hide the mouse cursor.
		ShowCursor(false);
	}

	void Main::ShutdownWindows()
	{
		// Show the mouse cursor.
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode.
		if (m_fullscreen)
		{
			ChangeDisplaySettings(NULL, 0);
		}

		// Remove the window.
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;

		// Remove the application instance.
		UnregisterClass(m_applicationName, m_hInstance);
		m_hInstance = NULL;

		// Release the pointer to this class.
		MAIN = nullptr;
	}

	LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
	{
		switch (umessage)
		{
			// Check if the window is being destroyed.
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// Check if the window is being closed.
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// All other messages pass to the message handler in the system class.
		default:
		{
			return MAIN->MessageHandler(hwnd, umessage, wparam, lparam);
		}
		}
	}
}
