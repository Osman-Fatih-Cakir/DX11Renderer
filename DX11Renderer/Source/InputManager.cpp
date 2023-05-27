#include "InputManager.h"

namespace DX11Renderer
{
	void InputManager::Init()
	{
		for (int i = 0; i < 256; i++)
		{
			m_keys[i] = false;
		}
	}

	void InputManager::LateUpdate()
	{
		m_lastMouseX = m_mouseX;
		m_lastMouseY = m_mouseY;
	}
}