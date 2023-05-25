#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include "App.h"
#include "InputManager.h"

namespace DX11Renderer
{
	class Main
	{
	public:
		bool Init();
		void Shutdown();
		void Run();

		LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam);

	private:
		bool Frame();
		void InitWindows(int& screenWidth, int& screenHeight);
		void ShutdownWindows();

	public:
		InputManager* m_inputManager = nullptr;

	private:
		LPCWSTR m_applicationName = L"DX11Renderer";
		HWND m_hwnd;
		HINSTANCE m_hInstance;

		App* m_app = nullptr;

		bool m_fullscreen = false;
	};

	static LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam);

	// Global
	static Main* MAIN = nullptr;
}
