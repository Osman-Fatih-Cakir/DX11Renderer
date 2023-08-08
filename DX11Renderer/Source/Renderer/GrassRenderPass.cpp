#include "GrassRenderPass.h"
#include "../Utils/Utils.h"
#include "MathUtils.h"

namespace DX11Renderer
{
	bool GrassRenderPass::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
	{
		std::wstring vsFilename = std::wstring(L"..\\..\\DX11Renderer\\Resources\\Shaders\\grassRender_vs.hlsl");
		std::wstring psFilename = std::wstring(L"..\\..\\DX11Renderer\\Resources\\Shaders\\grassRender_ps.hlsl");

		bool result = InitShaders(device, hwnd, vsFilename.c_str(), psFilename.c_str());
		if (!result)
		{
			return false;
		}

		result = InitBuffers(device, deviceContext);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void GrassRenderPass::Shutdown()
	{
		if (m_cbPerFrame)
		{
			m_cbPerFrame->Release();
			m_cbPerFrame = nullptr;
		}

		if (m_cbPerScene)
		{
			m_cbPerScene->Release();
			m_cbPerScene = nullptr;
		}

		if (m_layout)
		{
			m_layout->Release();
			m_layout = nullptr;
		}

		if (m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = nullptr;
		}

		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = nullptr;
		}
	}

	bool GrassRenderPass::Render(ID3D11DeviceContext* deviceContext, UINT indexCount, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, const XMFLOAT4& tilePos)
	{
		bool result = SetParameters(deviceContext, viewMatrix, projectionMatrix, tilePos);
		if (!result)
		{
			return false;
		}

		DrawCall(deviceContext, indexCount);

		return true;
	}

	bool GrassRenderPass::InitShaders(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* vertexShaderBuffer = nullptr;
		ID3D10Blob* pixelShaderBuffer = nullptr;

		// Compile the vertex shader code.
		result = D3DCompileFromFile(vsFilename, NULL, NULL, "Main", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
			}
			else
			{
				MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK);
			}

			return false;
		}

		// Compile the pixel shader code.
		result = D3DCompileFromFile(psFilename, NULL, NULL, "Main", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
			}
			else
			{
				MessageBox(hwnd, psFilename, L"Missing Shader File", MB_OK);
			}

			return false;
		}

		// Create the vertex shader from the buffer.
		result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
		if (FAILED(result))
		{
			return false;
		}

		// Create the pixel shader from the buffer.
		result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
		if (FAILED(result))
		{
			return false;
		}

		// Create the vertex input layout description.
		D3D11_INPUT_ELEMENT_DESC polygonLayout[1];
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		// Get a count of the elements in the layout.
		unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

		// Create the vertex input layout.
		result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout);
		if (FAILED(result))
		{
			return false;
		}

		// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
		vertexShaderBuffer->Release();
		vertexShaderBuffer = nullptr;

		pixelShaderBuffer->Release();
		pixelShaderBuffer = nullptr;

		return true;
	}

	bool GrassRenderPass::InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		HRESULT result;

		// per frame constant buffer
		D3D11_BUFFER_DESC perFrameBufferDesc;
		perFrameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		perFrameBufferDesc.ByteWidth = sizeof(PerFrameData);
		perFrameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		perFrameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		perFrameBufferDesc.MiscFlags = 0;
		perFrameBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&perFrameBufferDesc, NULL, &m_cbPerFrame);
		if (FAILED(result))
		{
			return false;
		}

		// Per scene data
		for (UINT i = 0; i < m_grassCount; ++i)
		{
				float randX = RandomFloat(-2.0f, 2.0f);
				float randZ = RandomFloat(-2.0f, 2.0f);
				m_perSceneData.world[i] = XMMatrixScaling(20.0f, 20.0f, 20.0f); // if u change scaling on y axis, change grassRender_ps.hlsl height variable calc too
				m_perSceneData.world[i] = m_perSceneData.world[i] * XMMatrixTranslation(randX, 0.0f, randZ);
		}

		// per scene constant buffer
		D3D11_BUFFER_DESC perSceneBufferDesc;
		perSceneBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		perSceneBufferDesc.ByteWidth = sizeof(PerSceneData);
		perSceneBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		perSceneBufferDesc.CPUAccessFlags = 0;
		perSceneBufferDesc.MiscFlags = 0;
		perSceneBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&perSceneBufferDesc, NULL, &m_cbPerScene);
		if (FAILED(result))
		{
			return false;
		}

		deviceContext->UpdateSubresource(m_cbPerScene, 0, NULL, &m_perSceneData, 0, 0);

		return true;
	}

	void GrassRenderPass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
	{
		char* compileErrors = (char*)(errorMessage->GetBufferPointer());

		unsigned long long bufferSize = errorMessage->GetBufferSize();

		std::ofstream fout;
		fout.open("grass-shader-error.txt");
		for (unsigned long long i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = nullptr;
	}

	bool GrassRenderPass::SetParameters(ID3D11DeviceContext* deviceContext, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, const XMFLOAT4& tilePos)
	{
		HRESULT result;

		// Lock the constant buffer so it can be written to.
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		result = deviceContext->Map(m_cbPerFrame, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		PerFrameData* dataPtr = (PerFrameData*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;
		dataPtr->tilePos = tilePos;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_cbPerFrame, 0);

		ID3D11Buffer* vsConstBuffers[2] = { m_cbPerFrame, m_cbPerScene };
		deviceContext->VSSetConstantBuffers(0, 2, vsConstBuffers);

		return true;
	}

	void GrassRenderPass::DrawCall(ID3D11DeviceContext* deviceContext, UINT indexCount)
	{
		// Input layout
		deviceContext->IASetInputLayout(m_layout);

		deviceContext->VSSetShader(m_vertexShader, NULL, 0);
		deviceContext->PSSetShader(m_pixelShader, NULL, 0);

		deviceContext->DrawIndexedInstanced(indexCount, m_grassCount, 0, 0, 0);
	}
}
