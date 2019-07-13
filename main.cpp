#include "MixEngine.h"
#include <SDL2/SDL_main.h>

extern "C" int main(int argc, char** argv) {
    Mix::MixEngine::Initialize("Mix", argc, argv);
    return Mix::MixEngine::Instance().exec();
}
