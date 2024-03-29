#include "Camera.h"
#include "../Globals.h"
#include "../InputManager.h"
#include "../Keys.h"

namespace DX11Renderer
{
	void Camera::Init(float fovY, float aspectRatio, float _near, float _far)
	{
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fovY, aspectRatio, _near, _far);

		LookAt({ -11.0f, 5.0f, 0.0f }, { 0.0f, -1.0f, 0.0f });
		//LookAt({ -1.0f, 30.0f, 0.0f }, { 0.0f, 0.0f, 0.0f });
	}

	void Camera::Update(float deltaTime)
	{
		const float trMultiplier = 0.01f * deltaTime;
		const float rotMultiplier = 1.0f * deltaTime;

		// TODO get deltaTime as argument
		int deltaX = g_inputManager->DeltaMouseX();
		int deltaY = g_inputManager->DeltaMouseY();

		static XMMATRIX rotMat = XMMatrixInverse(nullptr, m_viewMatrix);

		if (deltaY != 0 || deltaX != 0)
		{
			// Calculate rotation

			m_yaw -= deltaX * rotMultiplier;
			m_pitch -= deltaY * rotMultiplier;

			if (m_pitch > 89.0f)
				m_pitch = 89.0f;
			if (m_pitch < -89.0f)
				m_pitch = -89.0f;

			const float radianYaw = XMConvertToRadians(m_yaw);
			const float radianPitch = XMConvertToRadians(m_pitch);

			m_front = XMVector3Normalize({ cosf(radianYaw) * cosf(radianPitch), sinf(radianPitch), sinf(radianYaw) * cosf(radianPitch) });
			m_right = XMVector3Normalize(XMVector3Cross(m_front, m_worldUp));
			m_up = XMVector3Normalize(XMVector3Cross(m_right, m_front));
		}

		// Calculate translation

		XMVECTOR pos = XMLoadFloat3(&m_position);
		if (g_inputManager->KeyDown(Key::W))
		{
			pos += m_front * trMultiplier;
		}
		if (g_inputManager->KeyDown(Key::S))
		{
			pos -= m_front * trMultiplier;
		}
		if (g_inputManager->KeyDown(Key::A))
		{
			pos += m_right * trMultiplier;
		}
		if (g_inputManager->KeyDown(Key::D))
		{
			pos -= m_right * trMultiplier;
		}
		XMStoreFloat3(&m_position, pos);

		// Calculate view matrix
		m_viewMatrix = XMMatrixLookAtLH(pos, XMVectorAdd(pos, m_front), m_up);
		
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
