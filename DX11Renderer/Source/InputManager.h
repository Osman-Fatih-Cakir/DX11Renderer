#pragma once

#include "Keys.h"

namespace DX11Renderer
{
	class InputManager
	{
		friend class Main;

	public:
		void Init(int centerX, int centerY);
		void Update();

		inline bool KeyDown(Key key)
		{
			return m_keys[(unsigned int)key];
		}

		inline int MouseX()
		{
			return m_mouseX;
		}

		inline int MouseY()
		{
			return m_mouseY;
		}

		inline int DeltaMouseX()
		{
			return m_deltaMouseX;
		}

		inline int DeltaMouseY()
		{
			return m_deltaMouseY;
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

		int m_centerMouseX = 0;
		int m_centerMouseY = 0;

		int m_deltaMouseX = 0;
		int m_deltaMouseY = 0;

		bool m_keys[256];

		bool m_centerMouse = false;
	};
}
