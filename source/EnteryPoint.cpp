#include <stdio.h>

#include "Log/Log.h"

#ifdef WIN32
#include "Window/windows/WindowsWindow.h"
#endif

#include "Renderer/TestRenderer.h"

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

	TestRenderer pRenderer;
	
	// For test
	SDL_Event ev;
	bool pRun = true;
	while (pRun)
	{
		bool cl = false;
		while (SDL_PollEvent(&ev))
		{
			if (ev.type == SDL_EVENT_QUIT)
				pRun = false;
			else if (ev.key.key == SDLK_ESCAPE)
				pRun = false;
			else if (ev.key.key == SDLK_K)
				cl = true;
		}

		pRenderer.Update();
	}


	return 0;
}