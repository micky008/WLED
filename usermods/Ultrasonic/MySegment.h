#pragma once

#include <stdint.h>

class MySegment {
public:
    int minCm;
    int maxCm;
    uint8_t idSegment;
    int maxLed;
    int minLed;
    MySegment(int _minCm, int _maxCm, int _minLed, int _maxLed, int _idSeg) {
        this->minCm = _minCm;
        this->maxCm = _maxCm;
        this->idSegment = (uint8_t)_idSeg;
        this->maxLed = _maxLed;
        this->minLed = _minLed;
    }
};