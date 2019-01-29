// Only for demonstration

#include <iostream>

#include "MxApplication.h"
#include "MxWindow.h"

using namespace Mix;

class Demo : public MxBehaviour {
public:
    void init() {
        window.create("Keyborad events test", 640, 480);
        // window.setIcon(icon_path_here);
        SDL_Surface* surface = SDL_GetWindowSurface(window.getWindowPtr());
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(window.getWindowPtr());
    }
    void onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID) {
        std::cout << scancode << std::endl;
    }
private:
    MxWindow window;
};

int main() {
    MxApplication app;
    app.behaviours.push_back(std::make_shared<Demo>());
    return app.execute();
}
