#pragma once

#include <directxmath.h>
using namespace DirectX;

namespace DX11Renderer
{
	class Camera
	{
	public:
		void Init(float fovY, float aspectRatio, float near, float far);
		void Update();
		void LookAt(const XMFLOAT3& pos, const XMFLOAT3& focus);

		inline const XMFLOAT3 GetPosition()
		{
			return m_position;
		}

		inline void GetProjectionMatrix(XMMATRIX& projectionMatrix)
		{
			projectionMatrix = m_projectionMatrix;
		}

		inline void GetViewMatrix(XMMATRIX& viewMatrix)
		{
			viewMatrix = m_viewMatrix;
		}

	private:
		XMFLOAT3 m_position;
		XMFLOAT3 m_focusPos;
		XMMATRIX m_projectionMatrix;
		XMMATRIX m_viewMatrix;
	};
}
