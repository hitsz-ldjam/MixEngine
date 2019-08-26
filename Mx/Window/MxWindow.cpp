#include "../../MixEngine.h"
#include "MxWindow.h"
#include "../Exceptions/MxExceptions.hpp"
#include <stb_image/stb_image.h>
#include <map>
#include <iostream>

namespace Mix {
	Window* Window::Get() {
		return MixEngine::Instance().getModule<Window>();
	}

	Window::Window(const std::string& _title, const Math::Vector2i& _size, Flags<WindowFlag> _windowFlag) {
		if (SDL_Init(SDL_INIT_VIDEO))
			throw ThirdPartyLibInitError("SDL2");

		mWindow = SDL_CreateWindow(_title.c_str(),
								   SDL_WINDOWPOS_CENTERED,
								   SDL_WINDOWPOS_CENTERED,
								   _size.x,
								   _size.y,
								   ToSDLWindowFlags(_windowFlag));
		if (!mWindow)
			throw WindowCreationError();
	}

	Window::~Window() {
		if (mWindow)
			SDL_DestroyWindow(mWindow);
		mWindow = nullptr;
	}

	void Window::setIcon(const std::filesystem::path& _path) {
		const int format = STBI_rgb_alpha;
		int channels;
		glm::ivec2 size;
		auto data = static_cast<unsigned char*>(stbi_load(_path.string().c_str(),
												&size.x,
												&size.y,
												&channels,
												format));
		if (!data)
			throw WindowIconLoadingError();

		SDL_Surface* icon = SDL_CreateRGBSurfaceWithFormatFrom(data,
															   size.x,
															   size.y,
															   32,
															   4 * size.x,
															   SDL_PIXELFORMAT_RGBA32);
		if (!icon) {
			stbi_image_free(data);
			throw WindowIconLoadingError();
		}

		SDL_SetWindowIcon(mWindow, icon);
		SDL_FreeSurface(icon);
		stbi_image_free(data);
	}

	Math::Vector2i Window::drawableSize() const {
		Math::Vector2i size;
		if (mWindow)
			SDL_Vulkan_GetDrawableSize(mWindow, &size.x, &size.y);
		return size;
	}

	Math::Vector2i Window::extent() const {
		Math::Vector2i size;
		if (mWindow)
			SDL_GetWindowSize(mWindow, &size.x, &size.y);
		return size;
	}

	std::string Window::getTitle() const {
		if (mWindow)
			return SDL_GetWindowTitle(mWindow);
		return "";
	}

	void Window::setTitle(const std::string& _title) const {
		if (mWindow)
			SDL_SetWindowTitle(mWindow, _title.c_str());
	}

	bool Window::GetRelativeMouseMode() {
		return SDL_GetRelativeMouseMode() == SDL_TRUE;
	}

	void Window::SetRelativeMouseMode(const bool _enable) {
		SDL_SetRelativeMouseMode(_enable ? SDL_TRUE : SDL_FALSE);
	}

	std::vector<const char*> Window::getRequiredInstanceExts() const {
		unsigned int count;
		SDL_Vulkan_GetInstanceExtensions(mWindow, &count, nullptr);
		std::vector<const char*> result(count);
		SDL_Vulkan_GetInstanceExtensions(mWindow, &count, result.data());
		return result;
	}

	Uint32 Window::ToSDLWindowFlags(Flags<WindowFlag> _flags) {
		// Check invalid combination
		bool ok = true;
		if (_flags.isSet(WindowFlag::HIDDEN) && _flags.isSet(WindowFlag::SHOWN)) {
			std::cout << "WindowFlag: Combination of HIDDEN and SHOWN is invalid. HIDDEN will be unset." << std::endl;
			_flags.unset(WindowFlag::HIDDEN);
		}

		if (_flags.isSet(WindowFlag::MINIMIZED) && _flags.isSet(WindowFlag::MAXIMIZED)) {
			std::cout << "WindowFlag: Combination of MINIMIZED and MAXIMIZED is invalid. MINIMIZED will be unset." << std::endl;
			_flags.unset(WindowFlag::HIDDEN);
		}

		static std::map<WindowFlag, Uint32> mapToSDlWindowFlag = {
			{WindowFlag::FULLSCREEN,	SDL_WINDOW_FULLSCREEN},
			{WindowFlag::SHOWN,			SDL_WINDOW_SHOWN},
			{WindowFlag::HIDDEN,		SDL_WINDOW_HIDDEN},
			{WindowFlag::BORDERLESS,	SDL_WINDOW_BORDERLESS},
			{WindowFlag::RESIZABLE,		SDL_WINDOW_RESIZABLE},
			{WindowFlag::MINIMIZED,		SDL_WINDOW_MINIMIZED},
			{WindowFlag::MAXIMIZED,		SDL_WINDOW_MAXIMIZED},
			{WindowFlag::VULKAN,		SDL_WINDOW_VULKAN}
		};

		Uint32 result = 0;
		for (auto pair : mapToSDlWindowFlag) {
			if (_flags.isSet(pair.first))
				result |= pair.second;
		}

		return result;
	}
}
