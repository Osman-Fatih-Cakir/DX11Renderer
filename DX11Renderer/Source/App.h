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
		App();
		bool Init(int screenWidth, int screenHeight, HWND hwnd);
		void Shutdown();
		bool Frame(float deltaTime);

	private:
		bool Render(float deltaTime);
		bool InitRenderer(HWND hwnd, int screenWidth, int screenHeight);
		
	private:
		Renderer* m_renderer = nullptr;

		Camera* m_camera = nullptr;
		GrassMesh* m_grassMesh = nullptr;
		ForwardRenderPass* m_forwardRenderPass = nullptr;
		GrassRenderPass* m_grassRenderPass = nullptr;

		const XMINT2 m_initRelMousePos = { 640, 320 };
		XMINT2 m_lastMouseXY = m_initRelMousePos;
		bool m_freeCameraActive = false;

		UINT m_totalTime = 0;
	};
}
