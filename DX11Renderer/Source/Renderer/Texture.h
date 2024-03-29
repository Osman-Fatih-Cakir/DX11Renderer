#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <string>

namespace DX11Renderer
{
	// TODO remove targa things
	class Texture
	{
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename, int height = 0, int width = 0, unsigned char* loadedData = nullptr);
		void Shutdown();

		inline ID3D11ShaderResourceView* GetTextureView()
		{
			return m_textureView;
		}

		inline const char* GetFileName()
		{
			return m_fileName.c_str();
		}

	private:
		bool LoadTarga32Bit(const char* filename, int& height, int& width, unsigned char** targaData);

	private:
		struct TargaHeader
		{
			unsigned char data1[12];
			unsigned short width;
			unsigned short height;
			unsigned char bpp;
			unsigned char data2;
		};

		std::string m_fileName;
		ID3D11Texture2D* m_texture = nullptr;
		ID3D11ShaderResourceView* m_textureView = nullptr;
	};

	/*
	* Empty gpu texture
	* 
	* Texture Desc:
	* R32 G32 B32 A32 - floating point
	* 3D texture
	* 
	* see details in Init function
	*/
	class GPUTexture // TODO rename as WindTexture
	{
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT width, UINT height, UINT depth);
		void Shutdown();

		inline ID3D11ShaderResourceView* GetSRV()
		{
			return m_srv;
		}

		inline ID3D11UnorderedAccessView* GetUAV()
		{
			return m_uav;
		}

	private:
		ID3D11Texture3D* m_texture = nullptr;
		ID3D11ShaderResourceView* m_srv = nullptr;
		ID3D11UnorderedAccessView* m_uav = nullptr;
	};
}
