#include "InputManager.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Utils/Utils.h"

namespace DX11Renderer
{
	void InputManager::Init(int centerX, int centerY)
	{
		m_centerMouseX = m_mouseX = centerX;
		m_centerMouseY = m_mouseY = centerY;

		for (int i = 0; i < 256; i++)
		{
			m_keys[i] = false;
		}
	}

	void InputManager::Update()
	{
		m_deltaMouseX = m_mouseX - m_centerMouseX;
		m_deltaMouseY = m_mouseY - m_centerMouseY;

		if (KeyDown(Key::F))
			m_centerMouse = !m_centerMouse;

		if (m_centerMouse)
		{
			// lock mouse to the middle
			SetCursorPos(m_centerMouseX, m_centerMouseY);
			m_mouseX = m_centerMouseX;
			m_mouseY = m_centerMouseY;
		}
	}
}
