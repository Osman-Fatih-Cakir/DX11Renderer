#pragma once

#include <directxmath.h>
#include <random>
using namespace DirectX;

namespace DX11Renderer
{
	inline float RandomFloat(float min, float max)
	{
		static thread_local std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		static thread_local std::mt19937 generator;
		return min + distribution(generator) * (max - min);
	}


	inline void TranslationMatrix(float x, float y, float z, XMMATRIX& matrix)
	{
		matrix = XMMatrixTranslation(x, y, z);
	}
}
