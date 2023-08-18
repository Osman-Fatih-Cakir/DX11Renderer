
#include "WindComputePass.h"
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

		return true;
	}

	void WindComputePass::Shutdown()
	{
		if (m_computeShader)
		{
			m_computeShader->Release();
			m_computeShader = nullptr;
		}
	}

	// each call for this function will swap the texture that is being read and written
	bool WindComputePass::ExecuteComputation(ID3D11DeviceContext* deviceContext)
	{
		bool result;

		result = SetParameters(deviceContext);
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
				OutputShaderErrorMessage(errorMessage, hwnd, csFilename);
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

		m_windComputeTexture1 = new GPUTexture();
		m_windComputeTexture2 = new GPUTexture();

		constexpr int textureWidth = 36;
		constexpr int textureHeight = 36;
		constexpr int textureDepth = 16;

		result = m_windComputeTexture1->Init(device, deviceContext, textureWidth, textureHeight, textureDepth);
		if (!result)
		{
			return false;
		}
		result = m_windComputeTexture2->Init(device, deviceContext, textureWidth, textureHeight, textureDepth);
		if (!result)
		{
			return false;
		}

		return true;
	}

	void WindComputePass::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const WCHAR* shaderFilename)
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

	bool WindComputePass::SetParameters(ID3D11DeviceContext* deviceContext)
	{
		GPUTexture* readTex = m_readTex1WriteTex2 ? m_windComputeTexture1 : m_windComputeTexture2;
		GPUTexture* writeTex = m_readTex1WriteTex2 ? m_windComputeTexture2 : m_windComputeTexture1;

		ID3D11ShaderResourceView* readTexView = readTex->GetSRV();
		deviceContext->CSSetShaderResources(0, 1, &readTexView);

		ID3D11UnorderedAccessView* writeTexView = writeTex->GetUAV();
		deviceContext->CSSetUnorderedAccessViews(0, 1, &writeTexView, nullptr);

		return true;
	}

	bool WindComputePass::Compute(ID3D11DeviceContext* deviceContext)
	{
		deviceContext->CSSetShader(m_computeShader, NULL, 0);

		// dispatch call
		//todo deviceContext->Dispatch(16, 1, 1);
		deviceContext->Dispatch(1, 1, 1);

		// deattach the resources from gpu
		ID3D11ShaderResourceView* srvNull[2] = { nullptr, nullptr };
		deviceContext->CSSetShaderResources(0, 2, srvNull);

		ID3D11UnorderedAccessView* uavNull[1] = { nullptr };
		deviceContext->CSSetUnorderedAccessViews(0, 1, uavNull, nullptr);

		return true;
	}
}
