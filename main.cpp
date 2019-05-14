#include "MixEngine.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

int main(int argc, char** argv) {
    return Mix::MixEngine::Instance(argc, argv).exec();
}
