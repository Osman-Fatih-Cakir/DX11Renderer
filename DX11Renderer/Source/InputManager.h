#pragma once

namespace DX11Renderer
{
	class InputManager
	{
	public:
		void Init();

		bool KeyDown(unsigned int key);

		void SetKeyDown(unsigned int key);
		void SetKeyUp(unsigned int key);

	private:
		bool m_keys[256];
	};
}
