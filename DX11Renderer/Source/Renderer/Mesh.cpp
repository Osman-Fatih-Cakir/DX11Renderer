#include "Mesh.h"
#include "../Utils/Utils.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"

namespace DX11Renderer
{
	Mesh::Mesh()
	{
		m_worldMatrix = XMMatrixIdentity();
	}

	bool Mesh::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* textureFilename)
	{
		bool result;

		// Initialize the vertex and index buffers.
		result = InitBuffers(device, deviceContext);
		if (!result)
		{
			return false;
		}

		result = LoadTexture(device, deviceContext, textureFilename);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void Mesh::Shutdown()
	{
		ReleaseTexture();
		ShutdownBuffers();
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

	bool Mesh::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename)
	{
		// Create and initialize the texture object.
		m_texture = new Texture();

		bool result = m_texture->Init(device, deviceContext, filename);
		if (!result)
		{
			return false;
		}

		return true;
	}

	bool Mesh::InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
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

		vertices[0].position = XMFLOAT3(1.0f, -1.0f, 0.0f);  // Bottom left.
		vertices[0].texCoord = XMFLOAT2(1.0f, 0.0f);

		vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f);  // Top middle.
		vertices[1].texCoord = XMFLOAT2(0.5f, 1.0f);

		vertices[2].position = XMFLOAT3(-1.0f, -1.0f, 0.0f);  // Bottom right.
		vertices[2].texCoord = XMFLOAT2(0.0f, 0.0f);

		// TODO remove
		for (int i = 0; i < 3; ++i)
		{
			vertices[i].position.z += -5.0f;
		}
		//

		indices[0] = 0;
		indices[1] = 1;
		indices[2] = 2;

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

	void Mesh::ReleaseTexture()
	{
		if (m_texture)
		{
			m_texture->Shutdown();
			Utils::SafeDel(m_texture);
		}
	}

	void Mesh::ShutdownBuffers()
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

	void GrassMesh::SetBuffers(ID3D11DeviceContext* deviceContext)
	{
		// Set vertex buffer stride and offset.
		unsigned int stride = sizeof(GrassVertex);
		unsigned int offset = 0;

		// Set the vertex buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

		// Set the index buffer to active in the input assembler so it can be rendered.
		deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

	bool GrassMesh::InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		GrassVertex* vertices = nullptr;
		unsigned long* indices = nullptr;

		// Load grass mesh
		if (!LoadGLTF(&vertices, &indices, device, deviceContext))
		{
			return false;
		}

		HRESULT result;
		/*
		m_vertexCount = 5;
		m_indexCount = 18;

		vertices = new GrassVertex[m_vertexCount];
		if (!vertices)
		{
			return false;
		}

		indices = new unsigned long[m_indexCount];
		if (!indices)
		{
			return false;
		}

		vertices[0].position = XMFLOAT3( 0.05f,	 0.0f,	 0.05f);  // front bottom left
		vertices[1].position = XMFLOAT3( 0.0f,	 1.0f,	 0.0f);		// top middle
		vertices[2].position = XMFLOAT3(-0.05f,	-0.0f,	 0.05f);  // front bottom right
		vertices[3].position = XMFLOAT3( 0.05f,	-0.0f,	-0.05f);  // back bottom left
		vertices[4].position = XMFLOAT3(-0.05f,	-0.0f,	-0.05f);  // back bottom right

		vertices[0].color =	XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		vertices[1].color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
		vertices[2].color = XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
		vertices[3].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
		vertices[4].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

		// TODO remove
		for (int i = 0; i < 5; ++i)
		{
			vertices[i].position.y += -0.5f;
			vertices[i].position.z += -2.0f;
		}
		//

		indices[0] =	0; // front
		indices[1] =	1;
		indices[2] =	2;
		indices[3] =	2; // right
		indices[4] =	1;
		indices[5] =	4;
		indices[6] =	4; // back
		indices[7] =	1;
		indices[8] =	3;
		indices[9] =	3; // left
		indices[10] = 1;
		indices[11] = 0;
		indices[12] = 2; // bottom down
		indices[13] = 3;
		indices[14] = 0;
		indices[15] = 2; // bottom up
		indices[16] = 4;
		indices[17] = 3;
		*/

		// Set up the desc of static vertex buffer.
		D3D11_BUFFER_DESC vertexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(GrassVertex) * m_vertexCount;
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
		Utils::SafeDelArray(vertices);
		Utils::SafeDelArray(indices);

		return true;
	}

	bool GrassMesh::LoadGLTF(GrassVertex** vertices, unsigned long** indices, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		tinygltf::Model model;
		tinygltf::TinyGLTF gltf_ctx;
		std::string err;
		std::string warn;
		//std::string input_filename("..\\..\\DX11Renderer\\Resources\\GLTF\\single_grass\\scene.gltf");
		std::string input_filename("..\\..\\DX11Renderer\\Resources\\GLTF\\new_grass\\scene.gltf");

		//stbi_set_flip_vertically_on_load(true);
		gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, input_filename.c_str());
		//stbi_set_flip_vertically_on_load(false);

		// Vertex attributes
		for (std::pair<std::string, int> attribute : model.meshes[0].primitives[0].attributes)
		{
			std::string name = attribute.first;
			int attributeIndex = attribute.second;

			size_t byteOffset = model.bufferViews[attributeIndex].byteOffset;
			size_t byteLength = model.bufferViews[attributeIndex].byteLength;
			int target = model.bufferViews[attributeIndex].target;
			size_t componentCountPerElement = tinygltf::GetNumComponentsInType(model.accessors[attributeIndex].type);
			size_t componentSizeInBytes = tinygltf::GetComponentSizeInBytes(model.accessors[attributeIndex].componentType);
			size_t elementCount = model.accessors[attributeIndex].count;

			for (size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
			{
				size_t byteIndex = byteOffset + componentCountPerElement * componentSizeInBytes * elementIndex;

				if (target == TINYGLTF_TARGET_ARRAY_BUFFER)
				{
					if (*vertices == nullptr)
					{
						// set vertex count
						m_vertexCount = (UINT)elementCount;
						*vertices = new GrassVertex[elementCount];
					}

					if (name == "POSITION")
					{
						// position compoenent type is float-32, vec3

						float element1 = *(float*)(model.buffers[0].data.data() + byteIndex + 0);
						float element2 = *(float*)(model.buffers[0].data.data() + byteIndex + 4);
						float element3 = *(float*)(model.buffers[0].data.data() + byteIndex + 8);
						(*vertices)[elementIndex].position = { element1, element2, element3 };
					}
					else if (name == "NORMAL")
					{
						// normal compoenent type is float-32, vec3

						float element1 = *(float*)(model.buffers[0].data.data() + byteIndex + 0);
						float element2 = *(float*)(model.buffers[0].data.data() + byteIndex + 4);
						float element3 = *(float*)(model.buffers[0].data.data() + byteIndex + 8);
						(*vertices)[elementIndex].normal = { element1, element2, element3 };
					}
					else if (name == "TEXCOORD_0")
					{
						// texCoord compoenent type is uint16, vec2

						float element1 = *(float*)(model.buffers[0].data.data() + byteIndex + 0);
						float element2 = *(float*)(model.buffers[0].data.data() + byteIndex + 4);
						(*vertices)[elementIndex].texCoord = { element1, element2 };
					}
					else
					{
						assert(0 && "Unkown primitive attribute");
					}
				}
				else
				{
					assert(0 && "Elemental array buffer is not supported. Indices are loading below.");
				}
			}
		}

		// Indices
		size_t byteOffset = model.bufferViews.back().byteOffset;
		size_t byteLength = model.bufferViews.back().byteLength;
		int target = model.bufferViews.back().target;
		size_t componentSizeInBytes = tinygltf::GetComponentSizeInBytes(model.accessors.back().componentType);
		size_t elementCount = model.accessors.back().count;
		if (*indices == nullptr)
		{
			// set index count
			m_indexCount = (UINT)elementCount;
			*indices = new unsigned long[elementCount];
		}

		if (target != TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER)
		{
			assert(0 && "Indices should be on element array buffer?!");
		}

		for (size_t elementIndex = 0; elementIndex < elementCount; ++elementIndex)
		{
			size_t byteIndex = byteOffset +  componentSizeInBytes * elementIndex;
			(*indices)[elementIndex] = (unsigned long)(*(unsigned short*)(model.buffers[0].data.data() + byteIndex));
		}

		// Load texture
		m_texture = new Texture();
		tinygltf::Image& image = model.images[0];
		unsigned char* ptr = image.image.data();
		bool result = m_texture->Init(device, deviceContext, image.name.c_str()/* careful, no file extension in this name */, image.height, image.width, ptr);
		if (!result)
		{
			return false;
		}

		return true;
	}
}
