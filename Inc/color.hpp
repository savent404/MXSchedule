#pragma once

#include <stdint.h>

class RGB {
public:
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W;

    RGB(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t w = 0)
        : R(r)
        , G(g)
        , B(b)
        , W(w)
    {
    }

    RGB(const RGB& r)
    {
        *this = r;
    }

    RGB& operator=(const RGB& other)
    {
        if (&other == this)
            return *this;
        R = other.R;
        G = other.G;
        B = other.B;
        W = other.W;
        return *this;
    }
};
