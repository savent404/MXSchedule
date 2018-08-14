#pragma once

#include "common.h"
#include "iEvent.hpp"
#include "iParam.h"
#include <stdint.h>
#include "color.hpp"

// length(0.0f...1.0f)
#ifndef BLADE_COMET_LENGTH
#define BLADE_COMET_LENGTH (0.2f)
#endif

class iBladeDriver
{
  private:
    int numPixe;

  protected:
    RGB *vector;

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

    int getPixelNum() const
    {
        return numPixe;
    }

    RGB &operator[](int pos)
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
    void drawLine(RGB &color, int start, int end)
    {
        RGB *_ptr = ptr();
        int num = end - start;
        for (int i = 0; i < num; i++)
        {
            if (i < 0 || i >= getPixelNum())
                continue;
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
                   int posStart,
                   int posEnd)
    {
        if (colorStart.similar(colorEnd))
            drawLine(colorStart, posStart, posEnd);

        int sub[4];
        RGB *_ptr = posStart >= 0 ? ptr() + posStart : ptr();
        int num = posEnd - posStart;

        sub[0] = colorEnd.R - colorStart.R;
        sub[1] = colorEnd.G - colorStart.G;
        sub[2] = colorEnd.B - colorStart.B;
        sub[3] = colorEnd.W - colorStart.W;

        for (int i = 0; i < num; i++)
        {
            if (i + posStart < 0 || i + posStart >= getPixelNum())
                continue;
            _ptr->R = colorStart.R + sub[0] * i / num;
            _ptr->G = colorStart.G + sub[1] * i / num;
            _ptr->B = colorStart.B + sub[2] * i / num;
            _ptr->W = colorStart.W;
            _ptr++;
        }
    }
    virtual void update() = 0;
};

typedef struct vertex_t
{
    vertex_t(RGB c, int p) : color(c), pos(p){}
    RGB color;
    int pos;
} vertex_t;

/**
 * @brief step_t descript animation process
 * @param repeatCnt if == -1 means infinity loop
 *                  if == 0 means no repeat
 * @param totalStep operat times
 * @param nowStep
 */
typedef struct step_t {

    step_t(int now, int total, int cnt=0)
    {
        nowStep = now;
        totalStep = total;
        repeatCnt = cnt;
    }

    /**
     * @brief walk
     * @return if finish
     */
    bool walk()
    {
        bool finishALoop = ++nowStep >= totalStep;
        if (finishALoop)
        {
            nowStep = 0;
            if (repeatCnt == 0)
                return true;
            else if (repeatCnt > 0)
                repeatCnt--;
        }
        return false;
    }

    int nowStep;
    int totalStep;
    int repeatCnt;
} step_t;

class iBlade : public iEvent, public iBladeDriver
{
  protected:
    const iParam *parameter;
  protected: // API
    /**
     * @name BackGround
     * @{ */
    void drawBackGroundStatic();
    void drawBackGroundShade();
    void drawBackGroundStaticRainbow();
    void drawBackGroundDynamicRainbow(step_t& step);
    void drawBackGroundFlame();
    /** @} */

    /**
     * @name Dynamic Filter
     * @{ */
    void drawFilterStatic();
    void drawFilterBreath(step_t &step);
    void drawFilterFlow(step_t &step);
    void drawFilterSpark();
    void drawFilterRain();
    void drawFilterVolFollow();
    /** @} */

    /**
     * @name Trigger
     * @{ */
    void drawTriggerNone();
    void drawTriggerSpark(step_t &step);
    void drawTriggerPartialSpark(step_t &step);
    void drawTriggerFollowVol();
    void drawTriggerComet(step_t &step);
    /** @} */

protected: // vars
    bool isActive;
    bool needClear;
    int modeTrigger; //1~5, -1 means invalide
    step_t stepBackGround;
    step_t stepFilter;
    step_t stepTrigger;
  public:
    /**
     * @brief event
     */
    typedef enum event_t
    {
        event_start = 0x01,
        event_end = 0x02
    } event_t;

    iBlade(const iParam *p)
        : iEvent(EVENT_MODULE_ID_BLADE)
        , parameter(p)
        , iBladeDriver(BLADE_PIXEL)
        , stepBackGround(0, 2000/BLADE_INTERVAL, -1)
        , stepTrigger(0, 0, 0)
        , stepFilter(0, 0, -1)
    {
        setEventMask(0);
        isActive = false;
        needClear = true;
    }

    virtual ~iBlade()
    {
        delete vector;
    }

public:
    void hanlde();

    virtual bool play(triggerID_t id);
    virtual bool abort(triggerID_t id = Unknow);
};
