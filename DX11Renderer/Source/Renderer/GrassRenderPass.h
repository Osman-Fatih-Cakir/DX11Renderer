#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
#include "Texture.h"
using namespace DirectX;

namespace DX11Renderer
{
	class GrassRenderPass
	{
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd);
		void Shutdown();
		bool Render(ID3D11DeviceContext* deviceContext, UINT indexCount, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, const XMUINT2& tileCoord, const XMFLOAT4& tilePos, UINT time,
			const XMFLOAT2& mouseXZ, const XMFLOAT3& camPos, UINT windType, ID3D11ShaderResourceView* windTextureView);

	private:
		bool InitShaders(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename);
		bool InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		bool InitNoiseTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

		bool SetParameters(ID3D11DeviceContext* deviceContext, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, const XMUINT2& tileCoord, const XMFLOAT4& tilePos, UINT time,
			const XMFLOAT2& mouseXZ, const XMFLOAT3& camPos, UINT windType, ID3D11ShaderResourceView* windTextureView);
		void DrawCall(ID3D11DeviceContext* deviceContext, UINT indexCount);

	private:
		struct PerFrameData
		{
			XMMATRIX view;
			XMMATRIX projection;

			XMFLOAT4 tilePos;
			XMFLOAT2 mouseWorldPosXZ;
			XMUINT2 tileCoord;
			UINT time;
			UINT windType;
		};

		// NOTE: change grassRender_vs.hlsl constant buffer size too if u change this
		static constexpr UINT m_grassCount = 400;
		
		struct PerSceneData
		{
			XMMATRIX world[m_grassCount];
		};
		PerSceneData m_perSceneData;

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11InputLayout* m_layout = nullptr;
		ID3D11Buffer* m_cbPerFrame = nullptr;
		ID3D11Buffer* m_cbPerScene = nullptr;
		ID3D11SamplerState* m_samplerState = nullptr;

		Texture* m_noiseTexture = nullptr;
	};
}
