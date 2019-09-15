#include "MixEngine.h"
#include "MxApplicationBase.h"
#include <SDL2/SDL_main.h>

extern "C" int main(int argc, char** argv) {
    Mix::MixEngine::Initialize("Mix", argc, argv);
    Mix::ApplicationSetting setting = {
        "Demo",
        Mix::Version(0,0,1)
    };
    return Mix::MixEngine::Instance().execute(std::make_shared<Mix::ApplicationBase>(setting));
}
