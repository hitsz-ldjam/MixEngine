#include "MixEngine.h"

#include "Demo.h"

int main(int argc, char** argv) {
    try {
        Mix::Application app(argc, argv);
        app.behaviours.push_back(new Demo);
        return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}
