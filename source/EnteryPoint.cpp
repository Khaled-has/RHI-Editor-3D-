#include <stdio.h>

#include "Log/Log.h"

#include "GPU/VK/VK_Device.h"

#include <SDL3/SDL_vulkan.h>
#include <SDL3/SDL.h>

int main(int argc, char* argv[])
{
	Log::Init();

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* pWin = SDL_CreateWindow("RHI Editor ( 3D )", 1440, 640, SDL_WINDOW_VULKAN);

	GPU::VK_Device test;
	test.Create(GPU::WinType::SDL3, pWin);

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
	}

	return 0;
}