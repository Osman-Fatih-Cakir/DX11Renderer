#include "ForwardRenderPass.h"
#include "../Utils/Utils.h"

namespace DX11Renderer
{
  bool ForwardRenderPass::Init(ID3D11Device* device, HWND hwnd)
  {
		std::wstring vsFilename = std::wstring(L"..\\..\\DX11Renderer\\Resources\\Shaders\\forwardRender_vs.hlsl");
		std::wstring psFilename = std::wstring(L"..\\..\\DX11Renderer\\Resources\\Shaders\\forwardRender_ps.hlsl");

		bool result = InitShader(device, hwnd, vsFilename.c_str(), psFilename.c_str());
		if (!result)
		{
			return false;
		}

		return true;
  }

	void ForwardRenderPass::Shutdown()
	{
		if (m_mvpBuffer)
		{
			m_mvpBuffer->Release();
			m_mvpBuffer = 0;
		}

		if (m_layout)
		{
			m_layout->Release();
			m_layout = 0;
		}

		if (m_pixelShader)
		{
			m_pixelShader->Release();
			m_pixelShader = 0;
		}

		if (m_vertexShader)
		{
			m_vertexShader->Release();
			m_vertexShader = 0;
		}
	}

	bool ForwardRenderPass::Render(ID3D11DeviceContext* deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
	{
		bool result = SetParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix);
		if (!result)
		{
			return false;
		}

		DrawCall(deviceContext, indexCount);

		return true;
	}

	bool ForwardRenderPass::InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* vsFilename, const WCHAR* psFilename)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* vertexShaderBuffer = nullptr;
		ID3D10Blob* pixelShaderBuffer = nullptr;
		D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
		D3D11_BUFFER_DESC matrixBufferDesc;

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
		polygonLayout[0].SemanticName = "POSITION";
		polygonLayout[0].SemanticIndex = 0;
		polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		polygonLayout[0].InputSlot = 0;
		polygonLayout[0].AlignedByteOffset = 0;
		polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[0].InstanceDataStepRate = 0;

		polygonLayout[1].SemanticName = "COLOR";
		polygonLayout[1].SemanticIndex = 0;
		polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		polygonLayout[1].InputSlot = 0;
		polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		polygonLayout[1].InstanceDataStepRate = 0;

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

		// constant buffer
		matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixBufferDesc.ByteWidth = sizeof(MVPMatrixBuffer);
		matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixBufferDesc.MiscFlags = 0;
		matrixBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_mvpBuffer);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	void ForwardRenderPass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
	{
		char* compileErrors = (char*)(errorMessage->GetBufferPointer());

		unsigned long long bufferSize = errorMessage->GetBufferSize();

		std::ofstream fout;
		fout.open("shader-error.txt");
		for (unsigned long long i = 0; i < bufferSize; i++)
		{
			fout << compileErrors[i];
		}
		fout.close();

		// Release the error message.
		errorMessage->Release();
		errorMessage = nullptr;
	}

	bool ForwardRenderPass::SetParameters(ID3D11DeviceContext* deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix)
	{
		HRESULT result;

		// TODO remove the transpose!
		// TODO make the matrix arguments const reference!
		worldMatrix = XMMatrixTranspose(worldMatrix);
		viewMatrix = XMMatrixTranspose(viewMatrix);
		projectionMatrix = XMMatrixTranspose(projectionMatrix);

		// Lock the constant buffer so it can be written to.
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		result = deviceContext->Map(m_mvpBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		// Get a pointer to the data in the constant buffer.
		MVPMatrixBuffer* dataPtr = (MVPMatrixBuffer*)mappedResource.pData;

		// Copy the matrices into the constant buffer.
		dataPtr->world = worldMatrix;
		dataPtr->view = viewMatrix;
		dataPtr->projection = projectionMatrix;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_mvpBuffer, 0);

		// Set the position of the constant buffer in the vertex shader.
		unsigned int bufferNumber = 0;

		// Finanly set the constant buffer in the vertex shader with the updated values.
		deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_mvpBuffer);

		return true;
	}

	void ForwardRenderPass::DrawCall(ID3D11DeviceContext* deviceContext, int indexCount)
	{
		// Input layout
		deviceContext->IASetInputLayout(m_layout);

		deviceContext->VSSetShader(m_vertexShader, NULL, 0);
		deviceContext->PSSetShader(m_pixelShader, NULL, 0);

		deviceContext->DrawIndexed(indexCount, 0, 0);
	}
}
