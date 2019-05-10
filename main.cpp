#include "MixEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

int main(int argc, char** argv) {
    Mix::MixEngine engine(argc, argv);
    return engine.exec();
}
