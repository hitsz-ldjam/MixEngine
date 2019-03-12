#include "MixEngine.h"

#include "Demo.h"

int main(int argc, char *argv[]) {
    // todo
    const uint32_t behaviourNum = 1;
    Mix::Application app(argc, argv);
    app.behaviours.push_back(new Demo);
    return app.exec();
}
