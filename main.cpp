#include "MixEngine.h"

#include "Demo.h"

int main(int argc, char** argv) {
    // todo
    try {
        const uint32_t behaviourNum = 1;
        Mix::Application app(argc, argv);
        app.behaviours.push_back(new Demo);
        return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
