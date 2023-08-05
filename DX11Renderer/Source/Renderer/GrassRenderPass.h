#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <fstream>
using namespace DirectX;

namespace DX11Renderer
{
	class GrassRenderPass
	{
	public:
		bool Init(ID3D11Device* device, HWND hwnd);
		void Shutdown();
		bool Render(ID3D11DeviceContext* deviceContext, UINT indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* textureView);

	private:
		bool InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename);
		void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename);

		bool SetParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* textureView);
		void DrawCall(ID3D11DeviceContext* deviceContext, UINT indexCount);

	private:
		struct MVPMatrixBuffer
		{
			XMMATRIX world;
			XMMATRIX view;
			XMMATRIX projection;
		};

		ID3D11VertexShader* m_vertexShader = nullptr;
		ID3D11PixelShader* m_pixelShader = nullptr;
		ID3D11InputLayout* m_layout = nullptr;
		ID3D11Buffer* m_mvpBuffer = nullptr;
		ID3D11SamplerState* m_samplerState = nullptr;
		ID3D11BlendState* m_blendState = nullptr;;
	};
}