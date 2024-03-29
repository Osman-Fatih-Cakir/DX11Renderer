#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include "Texture.h"

using namespace DirectX;

namespace DX11Renderer
{
	class WindComputePass
	{
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
		void Shutdown();
		bool ExecuteComputation(ID3D11DeviceContext* deviceContext, const XMFLOAT2& mouseXZ, const XMFLOAT2& deltaMouseXZ, UINT deltaTimeInMicroseconds, UINT totalTime, UINT windType);
		inline GPUTexture* GetWindFlowMap()
		{
			return m_readTex1WriteTex2 ? m_windFlowMap1 : m_windFlowMap2;
		}

	private:
		bool InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* csFilename);
		bool InitWindTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		bool InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void OutputShaderErrorMessage(ID3D10Blob* errorMessage, const WCHAR* shaderFilename);

		bool SetParameters(ID3D11DeviceContext* deviceContext, const XMFLOAT2& mouseXZ, XMFLOAT2 deltaMouseXZ, UINT deltaTimeInMicroseconds, UINT totalTime, UINT windType);
		bool Compute(ID3D11DeviceContext* deviceContext);

	private:

		struct CBuffer
		{
			XMFLOAT2 mouseXZ;
			XMFLOAT2 deltaMouseXZ;
			UINT time;
			UINT deltaTime;
			UINT windType;
		};

		ID3D11ComputeShader* m_computeShader = nullptr;
		GPUTexture* m_windFlowMap1 = nullptr;
		GPUTexture* m_windFlowMap2 = nullptr;
		Texture* m_noiseTexture = nullptr;

		ID3D11Buffer* m_cBuffer= nullptr;

		bool m_readTex1WriteTex2 = true;

		XMFLOAT2 m_lastDeltaMouseXZ = { 1.0f, 1.0f };
	};
}
