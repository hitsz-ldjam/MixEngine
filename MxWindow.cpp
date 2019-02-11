#include "MXWindow.h"

namespace Mix
{
	MxWindow::MxWindow()
	{
		window = nullptr;
	}

	MxWindow::~MxWindow()
	{
		if (window)
		{
			SDL_DestroyWindow(window);
			window = nullptr;
		}
	}

	void MxWindow::create(const std::string title, const int width, const int height, const unsigned int flag)
	{
// Edit flag in demo -> SDL_WINDOW_VULKAN
		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  width, height, flag | SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
		this->title = title;
		rect = { width, height };

		if (!window)
		{
			throw std::runtime_error("Error: Failed to create window");
		}
	}

	void MxWindow::destory()
	{
		if (window)
		{
			SDL_DestroyWindow(window);
			window = nullptr;
		}
	}

	SDL_Window* MxWindow::getWindow()
	{
		return window;
	}

	const std::string MxWindow::getTitle()
	{
		return title;
	}

	const MXRect MxWindow::getWindowRect()
	{
		return rect;
	}

}