#include <stdio.h>

#include "Log/Log.h"

#ifdef WIN32
#include "Window/windows/WindowsWindow.h"
#endif

#include "GPU/VK/VK_Backend.h"

int main(int argc, char* argv[])
{
	Log::Init();

	Win::Window* pAppWin = nullptr;

#ifdef WIN32
	pAppWin = Win::CreateWindowsWindow(
		"RHI Editor ( 3D )",
		1440, 720
	);
#endif
	pAppWin->Create();

	GPU::GPU_Backend* gpu_backend = GPU::CreateVulkanBackend();
	gpu_backend->Backend_Init();

	// For test
	SDL_Event ev;
	bool pRun = true;
	while (pRun)
	{
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				pRun = false;
		}

		gpu_backend->RenderBegin();

		gpu_backend->RenderEnd();
	}

	return 0;
}