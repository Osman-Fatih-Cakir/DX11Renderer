#pragma once

#include <d3d11.h>
#include <stdio.h>
#include <string>

namespace DX11Renderer
{
	class Texture
	{
	public:
		bool Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename);
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
}