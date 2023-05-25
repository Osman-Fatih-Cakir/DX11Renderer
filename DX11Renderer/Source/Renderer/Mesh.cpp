#include "Mesh.h"
#include "../Utils/Utils.h"

namespace DX11Renderer
{
	bool Mesh::Init(ID3D11Device* device)
	{
		bool result;

		// Initialize the vertex and index buffers.
		result = InitBuffers(device);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void Mesh::Shutdown()
	{
		if (m_indexBuffer)
		{
			m_indexBuffer->Release();
			m_indexBuffer = nullptr;
		}

		if (m_vertexBuffer)
		{
			m_vertexBuffer->Release();
			m_vertexBuffer = nullptr;
		}
	}

	void Mesh::SetBuffers(ID3D11DeviceContext* deviceContext)
	{
		// Set vertex buffer stride and offset.
		unsigned int stride = sizeof(Vertex);
		unsigned int offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	bool Mesh::InitBuffers(ID3D11Device* device)
	{
		HRESULT result;

		Vertex* vertices;
		unsigned long* indices;

		m_vertexCount = 3;
		m_indexCount = 3;

		vertices = new Vertex[m_vertexCount];
		if (!vertices)
		{
			return false;
		}

		indices = new unsigned long[m_indexCount];
		if (!indices)
		{
			return false;
		}

		vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom left.
		vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
		vertices[1].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom right.
		vertices[2].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		indices[0] = 0;  // Bottom left.
		indices[1] = 1;  // Top middle.
		indices[2] = 2;  // Bottom right.

		// Set up the desc of static vertex buffer.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		D3D11_SUBRESOURCE_DATA vertexData;
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now create the vertex buffer.
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Set up the desc of static index buffer.
		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		D3D11_SUBRESOURCE_DATA indexData;
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Note: Deletion is done since we do not need the data "for now".
		Utils::SafeDel(vertices);
		Utils::SafeDel(indices);

		return true;
	}
}
