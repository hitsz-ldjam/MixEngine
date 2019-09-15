#pragma once

#ifndef MX_WINDOW_H_
#define MX_WINDOW_H_

#include "../Math/MxVector2.h"
#include "../Engine/MxModuleBase.h"
#include "../Utils/MxFlags.h"
#include <SDL2/SDL_vulkan.h>
#include <SDL2/SDL.h>
#include <filesystem>
#include <string>

namespace Mix {
	enum class WindowFlag {
		FULLSCREEN = 0x0001,	/**< Fullscreen window */
		SHOWN = 0x0002,			/**< Window is visible */
		HIDDEN = 0x0004,		/**< Window is not visible */
		BORDERLESS = 0x0008,	/**< Window without border*/
		RESIZABLE = 0x0010,		/**< Window can be resized */
		MINIMIZED = 0x0020,		/**< Window is minimized */
		MAXIMIZED = 0x0040,		/**< Window is maximized */
		VULKAN = 0x0080			/**< Window for Vulkan */
	};

	MX_ALLOW_FLAGS_FOR_ENUM(WindowFlag);

	class Window final :public ModuleBase {
	public:
		static Window* Get();

		explicit Window(SDL_Window* _window = nullptr) : mWindow(_window) {}

		Window(const std::string& _title, const Vector2i& _size, Flags<WindowFlag> _windowFlag);

		~Window();

		void load() override {};

		void init() override {};

		void setIcon(const std::filesystem::path& _path);

		Vector2i getDrawableSize() const;

		Vector2i getExtent() const;

		SDL_Window* rawPtr() const { return mWindow; }

		SDL_Surface* rawSurface() const { return SDL_GetWindowSurface(mWindow); }

		std::string getTitle() const;

		void setTitle(const std::string& _title) const;

		std::vector<const char*> getRequiredInstanceExts() const;

	private:
		static Uint32 ToSDLWindowFlags(Flags<WindowFlag> _flags);

		SDL_Window* mWindow;
	};
}

#endif
