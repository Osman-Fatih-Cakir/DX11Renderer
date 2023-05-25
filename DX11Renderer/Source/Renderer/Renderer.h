#pragma once

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX; // TODO remove this line

namespace DX11Renderer
{
	struct RenderSettings
	{
		bool FULL_SCREEN = false;
		bool VSYNC_ENABLED = true;
	};

	class Renderer
	{
	public:
		bool Init(unsigned int screenWidth, unsigned int screenHeight, HWND hwnd, const RenderSettings& renderSettings);
		void Shutdown();

		void BeginScene();
		void EndScene();
		void GetVideoCardInfo(char* cardName, int& memory);


		inline ID3D11Device* GetDevice()
		{
			return m_device;
		}

		inline ID3D11DeviceContext* GetDeviceContext()
		{
			return m_deviceContext;
		}

	private:
		RenderSettings m_renderSettings;

		int m_videoCardMemory;
		char m_videoCardDescription[128];
		IDXGISwapChain* m_swapChain = nullptr;
		ID3D11Device* m_device = nullptr;
		ID3D11DeviceContext* m_deviceContext = nullptr;
		ID3D11RenderTargetView* m_renderTargetView = nullptr;
		ID3D11Texture2D* m_depthStencilBuffer = nullptr;
		ID3D11DepthStencilState* m_depthStencilState = nullptr;
		ID3D11DepthStencilView* m_depthStencilView = nullptr;
		ID3D11RasterizerState* m_rasterState = nullptr;
	};
}
