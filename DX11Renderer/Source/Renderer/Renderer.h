#pragma once

#include <windows.h>

namespace DX11Renderer
{
	struct RenderSettings
	{
		const bool FULL_SCREEN = false;
		const bool VSYNC_ENABLED = true;
		const float SCREEN_DEPTH = 1000.0f;
		const float SCREEN_NEAR = 0.3f;
	};

	class Renderer
	{
	public:
		Renderer(const RenderSettings& renderSettings);
		bool Init(int screenWidth, int screenHeight, HWND hwnd);
		void Shutdown();
		bool Frame();

	private:
		Renderer() {}
		bool Render();
	};
}