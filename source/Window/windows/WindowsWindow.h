#ifndef WINDOWSWINDOW_H
#define WINDOWSWINDOW_H

#include "Window/Window.h"

#include <SDL3/SDL.h>

namespace Win
{

	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(std::string pWinTitle, unsigned int w, unsigned int h)
			: Window(pWinTitle, w, h)
		{}

		~WindowsWindow() {}

		virtual void Create() override;

		inline virtual void* GetWindow() override { return pWin; };
		inline virtual WindowType GetWindowType() override { return WindowType::SDL3; }

	private:
		SDL_Window* pWin = nullptr;
	};

	inline WindowsWindow* CreateWindowsWindow(std::string pTitle, unsigned int pWidth, unsigned int pHeight) 
	{ 
		return new WindowsWindow(pTitle, pWidth, pHeight); 
	}

}

#endif