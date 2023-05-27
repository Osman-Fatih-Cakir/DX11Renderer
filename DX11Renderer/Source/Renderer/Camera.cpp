#include "Camera.h"
#include "../Globals.h"
#include "../InputManager.h"
#include "../Keys.h"

namespace DX11Renderer
{
	void Camera::Init(float fovY, float aspectRatio, float _near, float _far)
	{
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fovY, aspectRatio, _near, _far);

		LookAt({ 0.0f, 0.0f, 5.0f }, { 0.0f, 0.0f, 0.0f });
	}

	void Camera::Update()
	{
		// TODO get deltaTime as argument
		int deltaX = g_inputManager->DeltaMouseX();
		int deltaY = g_inputManager->DeltaMouseY();

		static XMMATRIX rotMat = XMMatrixInverse(nullptr, m_viewMatrix);

		if (deltaY != 0)
		{
			const float multiplier = 0.1f;
			m_yaw -= deltaX * multiplier;
			m_pitch -= deltaY * multiplier;

			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;

			const float radianYaw = XMConvertToRadians(m_yaw);
			const float radianPitch = XMConvertToRadians(m_pitch);

			m_front = XMVector3Normalize({ cosf(radianYaw) * cosf(radianPitch), sinf(radianPitch), sinf(radianYaw) * cosf(radianPitch) });
			m_right = XMVector3Normalize(XMVector3Cross(m_front, m_worldUp));
			m_up = XMVector3Normalize(XMVector3Cross(m_right, m_front));

			m_viewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&m_position), XMVectorAdd(XMLoadFloat3(&m_position), m_front), m_up);
		}
	}

	void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& focus)
	{
		m_position = pos;

		XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
		XMVECTOR focusPos = { focus.x, focus.y, focus.z, 0.0f };
		XMVECTOR eye = { pos.x, pos.y, pos.z, 0.0f };
		m_viewMatrix = XMMatrixLookAtLH(eye, focusPos, up);
	}
}
	