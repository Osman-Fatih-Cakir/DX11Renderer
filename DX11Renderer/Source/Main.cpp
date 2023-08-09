#include "Main.h"
#include "windowsx.h"
#include <chrono>
#include "Utils/Utils.h"
#include "Globals.h"
#include "Keys.h"

namespace DX11Renderer
{
	bool Main::Init()
	{
		// Get an external pointer to this object
		MAIN = this;

		bool result;

		m_app = new App();

		InitWindows();

		// Get the middle of the window
		RECT rect;
		GetWindowRect(m_hwnd, &rect);
		g_inputManager = new InputManager();
		g_inputManager->Init((rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2);

		result = m_app->Init(m_screenWidth, m_screenHeight, m_hwnd);
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
				static thread_local auto t_start = std::chrono::high_resolution_clock::now();
				static thread_local float elapsedTimeInMs = 0.0f;

				bool result = Frame(elapsedTimeInMs);

				const auto t_end = std::chrono::high_resolution_clock::now();
				elapsedTimeInMs = std::chrono::duration<float, std::milli>(t_end - t_start).count();
				t_start = t_end;
#if 0 // Log frame duration
				Utils::Log((int)elapsedTimeInMs);
#endif
				if (!result)
				{
					done = true;
				}
			}
		}
	}

	bool Main::Frame(float deltaTime)
	{
		g_inputManager->Update();

		if (g_inputManager->KeyDown(Key::Escape))
		{
			return false;
		}

		if (g_inputManager->KeyDown(Key::F))
		{
			// Hide the mouse cursor.
			m_showCursor = !m_showCursor;
			ShowCursor(m_showCursor);
		}

		bool result = m_app->Frame(deltaTime);
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
			break;
		}
		case WM_KEYUP:
		{
			g_inputManager->SetKeyUp((unsigned int)wparam);
			break;
		}
		case WM_MOUSEMOVE:
		{
			g_inputManager->m_mouseX = GET_X_LPARAM(lparam) + m_posX;
			g_inputManager->m_mouseY = GET_Y_LPARAM(lparam) + m_posY;
			g_inputManager->m_deltaMouseX = GET_X_LPARAM(lparam);
			g_inputManager->m_deltaMouseY = GET_Y_LPARAM(lparam);
			
			break;
		}
		case WM_LBUTTONDOWN:
		{
			// left mouse click
			break;
		}
		case WM_RBUTTONDOWN:
		{
			// right mouse click
			break;
		}
		default:
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
		}

		return 0;
	}

	void Main::InitWindows()
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

		// Place the window in the middle of the screen.
		m_posX = (GetSystemMetrics(SM_CXSCREEN) - m_screenWidth) / 2;
		m_posY = (GetSystemMetrics(SM_CYSCREEN) - m_screenHeight) / 2;

		// Create the window with the screen settings and get the handle to it.
		m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
			m_posX, m_posY, m_screenWidth, m_screenHeight, NULL, NULL, m_hInstance, NULL);

		// Bring the window up on the screen and set it as main focus.
		ShowWindow(m_hwnd, SW_SHOW);
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);
	}

	void Main::ShutdownWindows()
	{
		// Show the mouse cursor.
		ShowCursor(true);

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
