#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/ForwardRenderPass.h"

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
		bool InitRenderer(HWND hwnd);
		
	private:
		Renderer* m_renderer = nullptr;

		bool m_initialized = false;
		bool m_fullscreen = false;

		Camera* m_camera = nullptr;
		Mesh* m_mesh = nullptr;
		ForwardRenderPass* m_forwardRenderPass = nullptr;
	};
}
