#pragma once

#ifndef _Demo_H_
#define _Demo_H_

#include <iostream>

#include "../MxBehaviour.h"
#include "../MxWindow.h"

class Demo : public Mix::MxBehaviour {
public:
    Demo();
    ~Demo();
    void init();
    void update();
    void onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID);
private:
    Mix::MxWindow window;
};

#endif
