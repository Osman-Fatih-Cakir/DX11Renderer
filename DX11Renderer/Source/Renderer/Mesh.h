#pragma once

#include <d3d11.h>
#include <directxmath.h>
using namespace DirectX; // TODO remove this line

namespace DX11Renderer
{
	class Mesh
	{
	public:
		bool Init(ID3D11Device* device);
		void Shutdown();
		void SetBuffers(ID3D11DeviceContext* deviceContext);

		inline int GetIndexCount()
		{
			return m_indexCount;
		}

	private:
		bool InitBuffers(ID3D11Device* device);

	private:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT4 color;
		};

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		int m_vertexCount = 0;
		int m_indexCount = 0;
	};
}