#pragma once

#ifndef _MX_WINDOW_H_
#define _MX_WINDOW_H_

#include <stdexcept>
#include <string>

#include <SDL2/SDL.h>

namespace Mix
{
	struct MXRect { int width, height; };

	class MxWindow
	{
	public:
		MxWindow();
		~MxWindow();
		void create(const std::string title, const int width, const int height, const unsigned int flag = 0);
		void destory();
		SDL_Window* getWindow();
		const std::string getTitle();
		const MXRect getWindowRect();

	private:
		SDL_Window* window;
		std::string title;
		MXRect rect;
	};
}

#endif
