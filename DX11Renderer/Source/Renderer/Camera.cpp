#include "Camera.h"

namespace DX11Renderer
{
	void Camera::Update()
	{
		if (!m_needUpdate)
			return;

		static constexpr XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
		XMVECTOR focusPos = { m_focusPos.x, m_focusPos.y, m_focusPos.z, 0.0f };
		XMVECTOR eye = { m_position.x, m_position.y, m_position.z, 0.0f };
		m_viewMatrix = XMMatrixLookAtLH(eye, focusPos, up);

		m_needUpdate = false;
	}

	void Camera::Translate(float x, float y, float z)
	{
		m_position.x += x;
		m_position.y += y;
		m_position.z += z;

		m_needUpdate = true;
	}

	void Camera::SetTranslation(float x, float y, float z)
	{
		m_position.x = x;
		m_position.y = y;
		m_position.z = z;

		m_needUpdate = true;
	}

	void Camera::LookAt(float x, float y, float z)
	{
		m_focusPos.x = x;
		m_focusPos.y = y;
		m_focusPos.z = z;

		m_needUpdate = true;
	}
}