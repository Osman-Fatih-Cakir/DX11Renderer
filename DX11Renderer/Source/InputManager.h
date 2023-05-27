#pragma once

#include "Keys.h"

namespace DX11Renderer
{
	class InputManager
	{
		friend class Main;

	public:
		void Init();
		void LateUpdate();

		inline bool KeyDown(Key key)
		{
			return m_keys[(unsigned int)key];
		}

		inline int DeltaMouseX()
		{
			if (m_firstDelta && m_mouseX != 0)
			{
				m_firstDelta = false;
				m_lastMouseX = m_mouseX;
				return 0;
			}
			else
			{
				return m_mouseX - m_lastMouseX;
			}
		}

		inline int DeltaMouseY()
		{
			if (m_firstDelta && m_mouseY != 0)
			{
				m_firstDelta = false;
				m_lastMouseY = m_mouseY;
				return 0;
			}
			else
			{
				return m_mouseY - m_lastMouseY;
			}
		}

	private:
		inline void SetKeyDown(unsigned int key)
		{
			m_keys[key] = true;
		}

		inline void SetKeyUp(unsigned int key)
		{
			m_keys[key] = false;
		}

	private:
		int m_mouseX = 0;
		int m_mouseY = 0;
		int m_lastMouseX = 0;
		int m_lastMouseY = 0;

		bool m_keys[256];

		bool m_firstDelta = true; // TODO fix this
	};
}
