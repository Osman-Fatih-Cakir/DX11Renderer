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
		virtual ~Mesh() { }
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename);
		void Shutdown();
		virtual void SetBuffers(ID3D11DeviceContext* deviceContext);

		inline UINT GetIndexCount()
		{
			return m_indexCount;
		}

		inline void GetWorldMatrix(XMMATRIX& worldMatrix)
		{
			worldMatrix = m_worldMatrix;
		}

		inline virtual Texture* GetTexture()
		{
			return m_texture;
		}

	protected:
		virtual bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);
		virtual bool InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
		virtual void ReleaseTexture();
		void ShutdownBuffers();

	protected:
		struct Vertex
		{
			XMFLOAT3 position;
			XMFLOAT2 texCoord;
		};

		ID3D11Buffer* m_vertexBuffer = nullptr;
		ID3D11Buffer* m_indexBuffer = nullptr;
		UINT m_vertexCount = 0;
		UINT m_indexCount = 0;

		XMMATRIX m_worldMatrix;

		Texture* m_texture = nullptr; // Material would be better, but no time to that :)
	};

	class GrassMesh final : public Mesh
	{
	public:
		GrassMesh() { }
		virtual ~GrassMesh() { }

		void SetBuffers(ID3D11DeviceContext* deviceContext) override;

	protected:

		struct GrassVertex
		{
			XMFLOAT3 position;
			XMFLOAT3 normal;
			XMFLOAT2 texCoord;
		};

		// load the grass texture from the gltf
		virtual bool LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename) override { return true; }

		bool InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext) override;

		bool LoadGLTF(GrassVertex** vertices, unsigned long** indices, ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	};
}
