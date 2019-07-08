#include "MixEngine.h"
#include <SDL2/SDL_main.h>

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv) {
    Mix::MixEngine::Initialize("Mix", argc, argv);
    return Mix::MixEngine::Instance().exec();
}
