#pragma once

#ifndef _MIX_ENGINE_H_
#define _MIX_ENGINE_H_

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

// #define TEST_WITH_VULKAN
#ifdef TEST_WITH_VULKAN
// todo include vulkan and other stuffs
#endif

#include "MxApplication.h"
#include "MxAudio.h"
#include "MxBehaviour.h"
#include "MxEvent.h"
#include "MxInput.h"
#include "MxSurface.h"
#include "MxWindow.h"

#endif
