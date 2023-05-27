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
		void LookAt(const XMFLOAT3& pos, const XMFLOAT3& focus);

	private:
		XMFLOAT3 m_position;
		XMMATRIX m_projectionMatrix;
		XMMATRIX m_viewMatrix;

		float m_yaw = -90.0f;
		float m_pitch = 0.0f;
		XMVECTOR m_front = { 0.0f, 0.0f, -1.0f, 0.0f };
		XMVECTOR m_right = { 1.0f, 0.0f, 0.0f, 0.0f };
		static constexpr XMVECTOR m_worldUp = { 0.0f, 1.0f, 0.0f, 0.0f };
		XMVECTOR m_up = m_worldUp;
	};
}
