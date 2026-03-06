#include "WindowsWindow.h"

namespace Win
{
	void WindowsWindow::Create()
	{
		// # Create the SDL3 window
		pWin = SDL_CreateWindow(
			GetWindowTitle().c_str(),
			1440, 720,
			SDL_WINDOW_VULKAN
		);
	}

}