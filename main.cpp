#include "MixEngine.h"

#include "Demo.h"

int main() {
    // todo
    const uint32_t behaviourNum = 1;
    Mix::Application app;
    app.behaviours.push_back(new Demo);
    return app.exec();
}
