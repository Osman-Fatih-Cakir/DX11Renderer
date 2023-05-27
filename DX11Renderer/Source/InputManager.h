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
			return m_mouseX - m_lastMouseX;
		}

		inline int DeltaMouseY()
		{
			return m_mouseX - m_lastMouseX;
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
	};
}
