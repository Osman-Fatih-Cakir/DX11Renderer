#pragma once

#include <directxmath.h>
using namespace DirectX;

namespace DX11Renderer
{
	inline void TranslationMatrix(float x, float y, float z, XMMATRIX& matrix)
	{
		matrix = XMMatrixTranslation(x, y, z);
	}
}
