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


	void InputManager::SetKeyDown(unsigned int input)
	{
		m_keys[input] = true;
	}


	void InputManager::SetKeyUp(unsigned int input)
	{
		m_keys[input] = false;
	}


	bool InputManager::KeyDown(Key key)
	{
		return m_keys[(unsigned int)key];
	}
}