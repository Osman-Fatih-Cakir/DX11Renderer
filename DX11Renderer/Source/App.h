#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/ForwardRenderPass.h"
#include "Renderer/GrassRenderPass.h"

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
		bool InitRenderer(HWND hwnd, int screenWidth, int screenHeight);
		
	private:
		Renderer* m_renderer = nullptr;

		bool m_fullscreen = false;

		Camera* m_camera = nullptr;
		GrassMesh* m_grassMesh = nullptr;
		ForwardRenderPass* m_forwardRenderPass = nullptr;
		GrassRenderPass* m_grassRenderPass = nullptr;
	};
}
