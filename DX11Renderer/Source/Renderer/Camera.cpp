#include "Camera.h"
#include "../Globals.h"
#include "../InputManager.h"
#include "../Keys.h"

namespace DX11Renderer
{
	void Camera::Init(float fovY, float aspectRatio, float near, float far)
	{
		m_projectionMatrix = XMMatrixPerspectiveFovLH(fovY, aspectRatio, near, far);
	}

	void Camera::Update()
	{
		if (g_inputManager->KeyDown(Key::W))
		{
			int y = 5;
		}
	}

	void Camera::LookAt(const XMFLOAT3& pos, const XMFLOAT3& focus)
	{
		m_position = pos;
		m_focusPos = focus;

		static constexpr XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
		XMVECTOR focusPos = { focus.x, focus.y, focus.z, 0.0f };
		XMVECTOR eye = { pos.x, pos.y, pos.z, 0.0f };
		m_viewMatrix = XMMatrixLookAtLH(eye, focusPos, up);
	}
}