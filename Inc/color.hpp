#pragma once

#include <stdint.h>
#include <math.h>

class RGB
{
  public:
    uint8_t R;
    uint8_t G;
    uint8_t B;
    uint8_t W; // user cant see it!

    RGB(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0, uint8_t w = 255)
        : R(r), G(g), B(b), W(w)
    {
    }

    RGB(const RGB &r)
    {
        this->R = r.R;
        this->G = r.G;
        this->B = r.B;
        this->W = r.W;
    }

    RGB(const RGB &r,
        int offset_r,
        int offset_g,
        int offset_b,
        int offset_w)
    {
        R = r.R + offset_r;
        G = r.G + offset_g;
        B = r.B + offset_b;
        W = r.W + offset_w;
    }
    RGB &operator=(const RGB &other)
    {
        if (&other == this)
            return *this;
        R = other.R;
        G = other.G;
        B = other.B;
        W = other.W;
        return *this;
    }
    RGB &operator-=(const RGB &other)
    {
        R -= other.R;
        G -= other.G;
        B -= other.B;
        return *this;
    }
    RGB &operator+=(const RGB &other)
    {
        R += other.R;
        G += other.G;
        B += other.B;
        return *this;
    }
    RGB &operator *=(const float q)
    {
        R *= q;
        G *= q;
        B *= q;
        return *this;
    }
    bool operator==(const RGB &other)
    {
        if (&other == this)
            return true;
        if (R != other.R)
            return false;
        if (G != other.G)
            return false;
        if (B != other.B)
            return false;
        if (W != other.W)
            return false;
        return true;
    }
    bool similar(const RGB &other, uint8_t offset = 10)
    {
        int sub;
        if (&other == this)
            return true;
        sub = wR() - other.wR();
        if (abs(sub) >= offset)
            return false;
        sub = wG() - other.wG();
        if (abs(sub) >= offset)
            return false;
        sub = wB() - other.wB();
        if (abs(sub) >= offset)
            return false;
        return true;
    }
    uint8_t light() const
    {
        int sum = R + G + B;
        return sum / 3;
    }
    uint8_t realLight() const
    {
        return (uint8_t((R * 299) + (G * 587) + (B * 114)) / 1000);
    }
    uint8_t wR() const
    {
        if (W != 255)
        {
            return (R * W) >> 8;
        }
        else
        {
            return R;
        }
    }
    uint8_t wG() const
    {
        if (W != 255)
        {
            return (G * W) >> 8;
        }
        else
        {
            return G;
        }
    }
    uint8_t wB() const
    {
        if (W != 255)
        {
            return (B * W) >> 8;
        }
        else
        {
            return B;
        }
    }
};
