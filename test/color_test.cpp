#include "color.hpp"
#include "gtest/gtest.h"
#include <stdio.h>
#include <stdlib.h>

static void isEqual(const RGB& a, const RGB& b)
{
    ASSERT_EQ(a.R, b.R);
    ASSERT_EQ(a.G, b.G);
    ASSERT_EQ(a.B, b.B);
}
static void isEqual(const RGB& a, const uint8_t& colors[3])
{
    ASSERT_EQ(a.R, colors[0]);
    ASSERT_EQ(a.G, colors[1]);
    ASSERT_EQ(a.B, colors[2]);
}

TEST(ColorTest, CheckCpy)
{
    uint8_t c[3];
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            c[j] = rand() % 256;
        }
        RGB c1(c[0], c[1], c[2]);
        RGB c2(c1);
        RGB c3 = c1;
        isEqual(c1, c);
        isEqual(c1, c2);
        isEqual(c1, c3);
    }
}

TEST(ColorTest, CheckOperator)
{
    uint8_t c[3];
    for (int i = 0; i < 128; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            c[j] = rand() % 128 + 128;
        }
        RGB c1(c[0], c[1], c[2]);

        // operator '-='
        for (int j = 0; j < 3; j++)
        {
            c[j] -= 20;
        }
        c1 -= 20;
        isEqual(c1, c);

        // operator '+='
        for (int j = 0; j < 3; j++)
        {
            c[j] += 20;
        }
        c1 += 20;
        isEqual(c1, c);

        // operator '*='
        for (int j = 0; j < 3; j++)
        {
            c[j] *= 0.3;
        }
        c1 *= 0.3;
        isEqual(c1, c);

        // operator '=='
        RGB c2 = c1;
        ASSERT_EQ(true, c1 == c2);
    }
}

TEST(ColorTest, CheckFunction)
{
    uint8_t c[3];
    for (int i = 0; i < 128; i++)
    {
        // similar
        for (int j = 0; j < 3; j++)
        {
            c[j] = rand() % 256;
        }
        RGB c1(c[0], c[1], c[2]);
        for (int j = 0; j < 3; j++)
        {
            c[j] += rand() % 20 - 10;
        }
        RGB c2(c[0], c[1], c[2]);
        ASSERT_EQ(c1.similar(c2));
    }
}

TEST(ColorTest, CheckColorConvert)
{
    HSV hsv_red(0, 1.0, 1.0);
    RGB rgb_red(255, 0, 0);

    HSV hsv_green(128, 1.0, 1.0);
    RGB rgb_green(0, 255, 0);

    HSV hsv_blue(240, 1.0, 1.0);
    RGB rgb_blue(0, 0, 255);
    
    ASSERT_EQ(true, rgb_red.similar(hsv_red.convert2RGB(), 2));
    ASSERT_EQ(true, rgb_green.similar(hsv_green.convert2RGB(), 2));
    ASSERT_EQ(true, rgb_blue.similar(hsv_blue.convert2RGB(), 2));
}