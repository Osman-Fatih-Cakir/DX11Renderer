#pragma once

#include <windows.h>

namespace DX11Renderer
{
	class App
	{
	public:
		App(bool fullscreen);
		bool Init(int screenWidth, int screenHeight, HWND hwnd);
		void Shutdown();
		bool Frame();

	private:
		App() {}
		bool Render();
	};
}