#pragma once

#include "MySegment.h"

class MyStrip {
public:
    MySegment* seg1 = nullptr;
    MySegment* seg2 = nullptr;
    MySegment* seg3 = nullptr;
    MyStrip(MySegment* _seg1, MySegment* _seg2, MySegment* _seg3) {
        this->seg1 = _seg1;
        this->seg2 = _seg2;
        this->seg3 = _seg3;
    }
};