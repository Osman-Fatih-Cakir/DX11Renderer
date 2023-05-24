#pragma once

namespace DX11Renderer
{
	template<typename T>
	void SafeDel(T ptr)
	{
		delete ptr;
		ptr = nullptr;
	}

	template<typename T>
	void SafeDelArray(T ptr)
	{
		delete[] ptr;
		ptr = nullptr;
	}
}