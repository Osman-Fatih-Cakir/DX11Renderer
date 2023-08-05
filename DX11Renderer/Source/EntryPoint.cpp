#include "Main.h"
#include "Utils/Utils.h"

/*
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
*/

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	DX11Renderer::Main* main;
	bool result;


	// Create the system object.
	main = new DX11Renderer::Main();

	// Initialize and run the system object.
	result = main->Init();
	if (result)
	{
		main->Run();
	}

	// Shutdown and release the system object.
	main->Shutdown();
	DX11Renderer::Utils::SafeDel(main);

	//_CrtDumpMemoryLeaks();

	return 0;
}