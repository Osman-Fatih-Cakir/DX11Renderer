#pragma once

#include <windows.h>

namespace DX11Renderer
{
	class Utils
	{
	public:

		template<typename T>
		inline static void SafeDel(T ptr)
		{
			delete ptr;
			ptr = nullptr;
		}

		template<typename T>
		inline static void SafeDelArray(T ptr)
		{
			delete[] ptr;
			ptr = nullptr;
		}

		// This function is writing to the Output window of Visual Studio
		///////////////////////////////////////////////////
		
		// Logs max 128 character
		static void Log(const char* str)
		{
			wchar_t wString[128];
			MultiByteToWideChar(CP_ACP, 0, str, -1, wString, 128);
			OutputDebugString(wString);
			OutputDebugString(L"\n");
		}

		// Logs max 16 digits
		static void Log(int val)
		{
			wchar_t buffer[16];
			wsprintfW(buffer, L"%d", val);
			OutputDebugString(buffer);
			OutputDebugString(L"\n");
		}
	};
}
