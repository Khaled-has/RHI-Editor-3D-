#ifndef WINDOW_H
#define WINDOW_H

#include <stdio.h>
#include <string>

namespace Win
{

	/*	 # The window types	  */
	enum WindowType
	{
		NO				= 0,
		SDL3			= 1,
		GLFW			= 2,
		NATIVE_ACTIVITY = 3		// for android
	};

	class Window
	{
	public:
		Window(std::string pWinTitle, unsigned int w, unsigned int h)
			: pWinTitle(pWinTitle), pW(w), pH(h)
		{
			pWinInstance = this;
		}

		~Window() {}

		virtual void Create() = 0;

		virtual void* GetWindow() = 0;
		virtual WindowType GetWindowType() = 0;
		
		inline std::string GetWindowTitle() const { return pWinTitle; }
		inline std::pair<unsigned int, unsigned int> GetWindowSize() { return std::pair<unsigned int, unsigned int>(pW, pH); }

		inline static Window* GetInstance() { return pWinInstance; }

	private:
		std::string pWinTitle;
		unsigned int pW, pH;

		inline static Window* pWinInstance;
	};

}

#endif