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

typedef struct HSV
{
    float h;
    float s;
    float v;
    HSV(uint8_t _h, uint8_t _s, uint8_t _v)
    {
        h = _h;
        s = _s;
        v = _v;
    }
    HSV(RGB& rgb)
    {
        *this = rgb;
    }

    RGB convert2RGB()
    {
        // In-case h is negative
        while (h < 0)
            h += 360.0f;
        float C = v * s;
        float P = fmodf(h / 60.0f, 6.0f);
        float X = C * (1 - fabs(fmodf(P, 2) - 1));
        float m = v - C;
        C += m;
        X += m;

        if (h < 60) {
            return RGB(C * 255, X * 255, m * 255);
        } else if (h < 120) {
            return RGB(X * 255, C * 255, m * 255);
        } else if (h < 180) {
            return RGB(m * 255, C * 255, X * 255);
        } else if (h < 240) {
            return RGB(m * 255, X * 255, C * 255);
        } else if (h < 300) {
            return RGB(X * 255, m * 255, C * 255);
        } else if (h < 360) {
            return RGB(C * 255, m * 255, X * 255);
        } else {
            return RGB(0, 0, 0);
        }
    }

    HSV& operator=(const HSV& other)
    {
        if (&other == this)
        {
            return *this;
        }
        h = other.h;
        s = other.s;
        v = other.v;
    }

    HSV& operator =(const RGB& rgb)
    {
        float r = rgb.R / 255.0f;
        float g = rgb.G / 255.0f;
        float b = rgb.B / 255.0f;

        int pmax = 2;
        float max = b, min = b;
        if (max < r) {
            pmax = 0;
            max = r;
        }
        if (min > r) {
            min = r;
        }
        if (max < g) {
            max = g;
            pmax = 1;
        }
        if (min > g) {
            min = g;
        }
        float delta = max - min;

        if (delta <= float(1e-4) && delta >= float(-1e-4)) {
            h = 0;
        } else if (pmax == 0) {
            h = (int((g - b) / delta) % 6) * 60.0f;
        } else if (pmax == 1) {
            h = (((b - r) / delta) + 2) * 60.0f;
        } else if (pmax == 2) {
            h = (((r - g) / delta) + 4) * 60.0f;
        }

        if (max <= float(1e-4) && max >= float(-1e-4)) {
            s = 0;
        } else {
            s = delta / max;
        }

        v = max;
        return *this;
    }

    operator class RGB()
    {
        return this->convert2RGB();
    }
} HSV;
