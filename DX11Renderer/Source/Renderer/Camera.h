#pragma once

#include <directxmath.h>
using namespace DirectX;

namespace DX11Renderer
{
	class Camera
	{
	public:
		void Update();
		void Translate(float x, float y, float z);
		void SetTranslation(float x, float y, float z);
		void LookAt(float x, float y, float z);

		inline const XMFLOAT3 GetPosition()
		{
			return m_position;
		}

		inline void GetViewMatrix(XMMATRIX& viewMatrix)
		{
			viewMatrix = m_viewMatrix;
		}

	private:
		XMFLOAT3 m_position; // Keep the last variable 0
		XMFLOAT3 m_focusPos; // Keep the last variable 0
		XMMATRIX m_viewMatrix;

		bool m_needUpdate = true;
	};
}
