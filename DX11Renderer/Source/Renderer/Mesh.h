#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include "Texture.h"
using namespace DirectX; // TODO remove this line

namespace DX11Renderer
{
	class Mesh
	{
	public:
		Mesh();
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename);
		void Shutdown();
		void SetBuffers(ID3D11DeviceContext* deviceContext);

		inline int GetIndexCount()
		{
			return m_indexCount;
		}

		inline void GetWorldMatrix(XMMATRIX& worldMatrix)
		{
			worldMatrix = m_worldMatrix;
		}

		inline Texture* GetTexture()
		{
			return m_texture;
		}

	private:
		bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);
		bool InitBuffers(ID3D11Device* device);
		void ReleaseTexture();
		void ShutdownBuffers();

	private:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT2 texCoord;
		};

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		int m_vertexCount = 0;
		int m_indexCount = 0;

		XMMATRIX m_worldMatrix;

		Texture* m_texture = nullptr; // Material would be better, but no time to that :)
	};
}