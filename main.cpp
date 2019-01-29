#include "MxApplication.h"

#include "./Scripts/Demo.h"

int main() {
    Mix::MxApplication app;

    app.behaviours.push_back(std::make_shared<Demo>());

    return app.execute();
}
