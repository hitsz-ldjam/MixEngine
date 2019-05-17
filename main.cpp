#include "MixEngine.h"

int main(int argc, char** argv) {
    return Mix::MixEngine::Instance(argc, argv).exec();
}
