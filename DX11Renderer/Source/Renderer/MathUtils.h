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

	inline XMFLOAT2 WorldXZFromScreenCoord(const XMINT2& xy, float height, const XMMATRIX& proj, const XMMATRIX& view)
	{
		XMFLOAT2 ndc = { xy.x / 1280.0f, xy.y / 720.0f };
		ndc.x = ndc.x * 2.0f - 1.0f;
		ndc.y = (1.0f - ndc.y) * 2.0f - 1.0f;
		XMMATRIX inversedProjView = DirectX::XMMatrixInverse(nullptr, view * proj);
		XMVECTOR screenPos1 = { ndc.x, ndc.y, 0.0f, 1.0f };
		XMVECTOR screenPos2 = { ndc.x, ndc.y, 1.0f, 1.0f };
		XMVECTOR worldPosWV1 = DirectX::XMVector4Transform(screenPos1, inversedProjView);
		XMVECTOR worldPosWV2 = DirectX::XMVector4Transform(screenPos2, inversedProjView);
		XMFLOAT4 worldPosW1;
		XMFLOAT4 worldPosW2;
		XMStoreFloat4(&worldPosW1, worldPosWV1);
		XMStoreFloat4(&worldPosW2, worldPosWV2);
		XMFLOAT3 worldPos1 = { worldPosW1.x / worldPosW1.w, worldPosW1.y / worldPosW1.w, worldPosW1.z / worldPosW1.w };
		XMFLOAT3 worldPos2 = { worldPosW2.x / worldPosW2.w, worldPosW2.y / worldPosW2.w, worldPosW2.z / worldPosW2.w };
		XMVECTOR worldPosV1 = XMLoadFloat3(&worldPos1);
		XMVECTOR worldPosV2 = XMLoadFloat3(&worldPos2);

		XMVECTOR intersectPointV = XMPlaneIntersectLine({ 0.0f, 1.0f, 0.0f, -height }, worldPosV1, worldPosV2);
		XMFLOAT3 intersectPoint;
		XMStoreFloat3(&intersectPoint, intersectPointV);

		return { intersectPoint.x, intersectPoint.z };
	}
}
