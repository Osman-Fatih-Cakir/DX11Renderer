#pragma once

#include "Renderer/Renderer.h"

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
		bool Render();
		
	private:
		Renderer* m_renderer = nullptr;

		bool m_initialized = false;
		bool m_fullscreen = false;
	};
}
