
#include "WindComputePass.h"
#include "../Utils/Utils.h"
#include <string>
#include <fstream>

namespace DX11Renderer
{
	bool WindComputePass::Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, HWND hwnd)
	{
		std::wstring csFilename = std::wstring(L"..\\..\\DX11Renderer\\Resources\\Shaders\\WindCompute.hlsl");

		bool result = InitShader(device, hwnd, csFilename.c_str());
		if (!result)
		{
			return false;
		}

		result = InitWindTextures(device, deviceContext);
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

	void WindComputePass::Shutdown()
	{
		if (m_computeShader)
		{
			m_computeShader->Release();
			m_computeShader = nullptr;
		}

		if (m_noiseTexture)
		{
			m_noiseTexture->Shutdown();
			Utils::SafeDel(m_noiseTexture);
		}

		if (m_windFlowMap)
		{
			m_windFlowMap->Shutdown();
			Utils::SafeDel(m_windFlowMap);
		}
	}

	// each call for this function will swap the texture that is being read and written
	bool WindComputePass::ExecuteComputation(ID3D11DeviceContext* deviceContext, const XMFLOAT2& mouseXZ, UINT totalTime, UINT windType)
	{
		bool result;

		result = SetParameters(deviceContext, mouseXZ, totalTime, windType);
		if (!result)
		{
			return false;
		}

		result = Compute(deviceContext);
		if (!result)
		{
			return false;
		}

		m_readTex1WriteTex2 = !m_readTex1WriteTex2;

		return true;
	}

	bool WindComputePass::InitShader(ID3D11Device* device, HWND hwnd, const WCHAR* csFilename)
	{
		HRESULT result;
		ID3D10Blob* errorMessage = nullptr;
		ID3D10Blob* computeShaderBuffer = nullptr;

		// Compile the vertex shader code.
		result = D3DCompileFromFile(csFilename, NULL, NULL, "Main", "cs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &computeShaderBuffer, &errorMessage);
		if (FAILED(result))
		{
			if (errorMessage)
			{
				OutputShaderErrorMessage(errorMessage, csFilename);
			}
			else
			{
				MessageBox(hwnd, csFilename, L"Missing Shader File", MB_OK);
			}

			return false;
		}

		result = device->CreateComputeShader(computeShaderBuffer->GetBufferPointer(), computeShaderBuffer->GetBufferSize(), nullptr, &m_computeShader);
		if (FAILED(result))
		{
			return false;
		}

		computeShaderBuffer->Release();
		computeShaderBuffer = nullptr;

		return true;
	}

	bool WindComputePass::InitWindTextures(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		bool result;

		m_windFlowMap = new GPUTexture();
		m_noiseTexture = new Texture();

		constexpr int textureWidth = 64;
		constexpr int textureHeight = 64;
		constexpr int textureDepth = 16;

		result = m_windFlowMap->Init(device, deviceContext, textureWidth, textureHeight, textureDepth);
		if (!result)
		{
			return false;
		}
		result = m_noiseTexture->Init(device, deviceContext, "..\\..\\DX11Renderer\\Resources\\Textures\\perlinNoise256x256RGB.tga");
		if (!result)
		{
			return false;
		}

		return true;
	}

	bool WindComputePass::InitBuffers(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		HRESULT result;

		// constant buffer
		D3D11_BUFFER_DESC constantBufferDesc;
		constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		constantBufferDesc.ByteWidth = sizeof(CBuffer);
		constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constantBufferDesc.MiscFlags = 0;
		constantBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&constantBufferDesc, NULL, &m_cBuffer);
		if (FAILED(result))
		{
			return false;
		}

		return true;
	}

	void WindComputePass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, const WCHAR* shaderFilename)
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

	bool WindComputePass::SetParameters(ID3D11DeviceContext* deviceContext, const XMFLOAT2& mouseXZ, UINT totalTime, UINT windType)
	{
		HRESULT result;

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		result = deviceContext->Map(m_cBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(result))
		{
			return false;
		}

		CBuffer* dataPtr = (CBuffer*)mappedResource.pData;

		dataPtr->mouseXZ = mouseXZ;
		dataPtr->time = totalTime;
		dataPtr->windType = windType;

		// Unlock the constant buffer.
		deviceContext->Unmap(m_cBuffer, 0);

		ID3D11Buffer* constBuffers[1] = { m_cBuffer };
		deviceContext->CSSetConstantBuffers(0, 1, constBuffers);

		ID3D11ShaderResourceView* noiseTexView = m_noiseTexture->GetTextureView();
		ID3D11ShaderResourceView* srvs[1] = { noiseTexView };
		deviceContext->CSSetShaderResources(0, 1, srvs);

		ID3D11UnorderedAccessView* writeTexView = m_windFlowMap->GetUAV();
		deviceContext->CSSetUnorderedAccessViews(0, 1, &writeTexView, nullptr);

		return true;
	}

	bool WindComputePass::Compute(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->CSSetShader(m_computeShader, NULL, 0);

		// dispatch call
		//todo deviceContext->Dispatch(4, 4, 16);
		deviceContext->Dispatch(4, 4, 1);

		// deattach the resources from gpu
		ID3D11ShaderResourceView* srvNull[1] = { nullptr };
		deviceContext->CSSetShaderResources(0, 1, srvNull);

		ID3D11UnorderedAccessView* uavNull[1] = { nullptr };
		deviceContext->CSSetUnorderedAccessViews(0, 1, uavNull, nullptr);

		ID3D11Buffer* cBNull[1] = { nullptr };
		deviceContext->CSSetConstantBuffers(0, 1, cBNull);

		return true;
	}
}
