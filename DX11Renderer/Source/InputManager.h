#pragma once

namespace DX11Renderer
{
	class InputManager
	{
		friend class Main;

	public:
		void Init();

		bool KeyDown(unsigned int key);

	private:
		void SetKeyDown(unsigned int key);
		void SetKeyUp(unsigned int key);

	private:
		bool m_keys[256];
	};
}
