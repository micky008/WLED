#pragma once
#include <stdint.h>
#include "MyStrip.h"

class Sonar {
public:
    uint8_t echo;
    uint8_t triger;
    MyStrip* strip = nullptr;

    Sonar(int e, int t, MyStrip* _strip) {
        this->echo = (uint8_t)e;
        this->triger = (uint8_t)t;
        this->strip = _strip;
    }
};