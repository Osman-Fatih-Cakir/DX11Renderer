#pragma once

#include "Renderer/Renderer.h"
#include "Renderer/Camera.h"
#include "Renderer/Mesh.h"
#include "Renderer/ForwardRenderPass.h"
#include "Renderer/GrassRenderPass.h"
#include "Renderer/WindComputePass.h"

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
		WindComputePass* m_windComputePass = nullptr;

		const XMINT2 m_initRelMousePos = { 640, 320 };
		XMINT2 m_mouseXY = m_initRelMousePos;
		XMFLOAT2 m_mouseWorldXZ = { 0, 0 };
		XMFLOAT2 m_prevMouseWorldXZ = m_mouseWorldXZ;
		bool m_freeCameraActive = false;
		UINT m_windType = 0;

		UINT m_totalTime = 0;
	};
}
