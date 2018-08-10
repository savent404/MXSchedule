#pragma once

#include "common.h"
#include "iEvent.hpp"
#include "iParam.h"
#include <stdint.h>
#include "color.hpp"

class iBladeDriver
{
  private:
    size_t numPixe;

  protected:
    RGB *vector;

    inline bool rangeCheck(size_t a)
    {
        return a >= getPixelNum() ? true : false;
    }

  public:
    iBladeDriver(size_t pixelNum)
    {
        numPixe = pixelNum;
        vector = new RGB[pixelNum];
    }
    virtual ~iBladeDriver()
    {
        delete vector;
    }

    size_t getPixelNum() const
    {
        return numPixe;
    }

    RGB &operator[](size_t pos)
    {
        if (pos > numPixe)
        {
            mDebug(DEBUG_LEVEL_ERROR, "Vector Out of Range:%d/%d", pos, numPixe);
            return *vector;
        }

        return vector[pos];
    }

    const RGB *c_ptr() const
    {
        return vector;
    }

    RGB *ptr()
    {
        return vector;
    }

  public: // API
    /**
     * @brief drawLine
     * @param driver
     * @param color
     * @param start [0...pixelNum-1]
     * @param end   [0...pixelNum-1]
     */
    void drawLine(RGB &color, size_t start, size_t end)
    {
        RGB *_ptr = ptr();
        size_t num = end - start;
        if (rangeCheck(start) || rangeCheck(end))
            return;
        for (size_t i = 0; i < num; i++)
        {
            *_ptr++ = color;
        }
    }
    /**
     * @brief drawShade
     * @note if colorStart is similar to colorEnd, call drawLine
     * @param driver
     * @param colorStart
     * @param colorEnd
     * @param posStart
     * @param posEnd
     */
    void drawShade(RGB &colorStart,
                   RGB &colorEnd,
                   size_t posStart,
                   size_t posEnd)
    {
        if (colorStart.similar(colorEnd))
            drawLine(colorStart, posStart, posEnd);

        int sub[4];
        RGB *_ptr = ptr();
        int num = posEnd - posStart;
        if (rangeCheck(posStart) || rangeCheck(posEnd))
            return;

        sub[0] = colorEnd.R - colorStart.R;
        sub[1] = colorEnd.G - colorStart.G;
        sub[2] = colorEnd.B - colorStart.B;
        sub[3] = colorEnd.W - colorStart.W;

        for (int i = 0; i < num; i++)
        {
            *_ptr++ = RGB(colorStart,
                          sub[0] * i / num,
                          sub[1] * i / num,
                          sub[2] * i / num,
                          sub[3] * i / num);
        }
    }
    virtual void update() = 0;
};

class iBlade : public iEvent, public iBladeDriver
{
  protected:
    const iParam *parameter;

  public:
    /**
     * @brief event
     */
    typedef enum event_t
    {
        start = 0x01,
        end = 0x02
    } event_t;

    iBlade(const iParam *p)
        : iEvent(EVENT_MODULE_ID_BLADE), parameter(p), iBladeDriver(BLADE_PIXEL)
    {
        setEventMask(start);
    }

    virtual ~iBlade()
    {
        delete vector;
    }

    virtual bool play(triggerID_t id) = 0;

    virtual bool abort(triggerID_t id) = 0;
};
