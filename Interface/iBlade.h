#pragma once

#include "color.hpp"
#include "common.h"
#include "iEvent.hpp"
#include "iParam.h"
#include <stdint.h>

/** @brief flow length(0.0f...1.0f) */
#ifndef BLADE_COMET_LENGTH
#define BLADE_COMET_LENGTH (0.2f)
#endif

/**
 * @brief iBladeDriver iBlade底层驱动
 * @details 实现颜色存储，基本两种绘画，定义了绘画接口
 */
class iBladeDriver {
private:
    /** @brief Pixel数量 */
    int numPixe;

protected:
    /**
     * @brief 颜色存储结构 
     */
    RGB* vector;

public:
    /**
     * @brief 根据指定的Pixel数量构造驱动
     */
    iBladeDriver(size_t pixelNum)
    {
        numPixe = pixelNum;
        vector = new RGB[pixelNum];
    }

    /**
     * @brief 析构
     */
    virtual ~iBladeDriver()
    {
        delete vector;
    }

    /**
     * @brief 返回Pixel数量
     * @note  此函数在绘制函数中大量使用
     */
    int getPixelNum() const
    {
        return numPixe;
    }

    /**
     * @brief 通过数组形式访问颜色
     */
    RGB& operator[](int pos)
    {
        if (pos > numPixe) {
            mDebug(DEBUG_LEVEL_ERROR, "Vector Out of Range:%d/%d", pos, numPixe);
            return *vector;
        }

        return vector[pos];
    }

    /**
     * @brief 获取整个数组的只读指针
     */
    const RGB* c_ptr() const
    {
        return vector;
    }

    /**
     * @brief 获取整个数组的指针
     */
    RGB* ptr()
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
    void drawLine(RGB& color, int start, int end)
    {
        RGB* _ptr = start >= 0 ? ptr() + start : ptr();
        int num = end - start;
        for (int i = 0; i < num; i++) {
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
    void drawShade(RGB& colorStart,
        RGB& colorEnd,
        int posStart,
        int posEnd)
    {
        if (colorStart.similar(colorEnd))
            drawLine(colorStart, posStart, posEnd);

        int sub[4];
        RGB* _ptr = posStart >= 0 ? ptr() + posStart : ptr();
        int num = posEnd - posStart;

        sub[0] = colorEnd.R - colorStart.R;
        sub[1] = colorEnd.G - colorStart.G;
        sub[2] = colorEnd.B - colorStart.B;
        sub[3] = colorEnd.W - colorStart.W;

        for (int i = 0; i < num; i++) {
            if (i + posStart < 0 || i + posStart >= getPixelNum())
                continue;
            _ptr->R = colorStart.R + sub[0] * i / num;
            _ptr->G = colorStart.G + sub[1] * i / num;
            _ptr->B = colorStart.B + sub[2] * i / num;
            _ptr->W = colorStart.W;
            _ptr++;
        }
    }

    /**
     * @brief 驱动接口
     * @details 通过BLADE_INTERVAL间隔事件调用一次
     */
    virtual void update() = 0;
};

/**
 * @brief 含位置信息的RGB
 */
typedef struct vertex_t {
    vertex_t(RGB c, int p)
        : color(c)
        , pos(p)
    {
    }
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

    step_t(int now, int total, int cnt = 0)
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
        if (finishALoop) {
            nowStep = 0;
            if (repeatCnt == 0)
                return true;
            else if (repeatCnt > 0)
                repeatCnt--;
        }
        return false;
    }

    /** @brief 当前step */
    int nowStep;
    /** @brief step总数 */
    int totalStep;
    /** 重复次数 cnt=-1时为无限循环 */
    int repeatCnt;
} step_t;

/**
 * @brief iBlade
 * @note  consider implement it with thread
 * @details 需要用另一个线程调用iBlade::handle
 * \code{.cpp}
 * class myBlade : public iBlade, public Thread {
 *   myBlade(.....) : iBlade(....), Thread(....) { }
 *   ~myBlade() {
 *       ~iBlade();
 *       ~Thread();
 *   }
 *   virtual run() {
 *       iBlade::handle();
 *       if (isActive)
 *           iBladeDriver::update();
 *       delay(BLADE_INTERVAL);
 *   }
 *  };
 *  int main()
 *  {
 *   myBlade b(...);
 *   b.start();
 *  }
 * \endcode
 */
class iBlade : public iEvent, public iBladeDriver {
protected:
    /**
     * @brief const pointer to call iParam's API
     */
    const iParam* parameter;

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
    void drawFilterBreath(step_t& step);
    void drawFilterFlow(step_t& step);
    void drawFilterSpark();
    void drawFilterRain();
    void drawFilterVolFollow();
    /** @} */

    /**
     * @name Trigger
     * @{ */
    void drawTriggerNone();
    void drawTriggerSpark(step_t& step);
    void drawTriggerPartialSpark(step_t& step);
    void drawTriggerFollowVol();
    void drawTriggerComet(step_t& step);
    /** @} */

protected: // vars
    /**
     * @brief flag if active
     * @details if isActive==true iBlade call iBladeDriver::update to display effect */
    bool isActive;

    /**
     * @brief flag if clear all pixel to black
     */
    bool needClear;

    /**
     * @name modes
     * @{
     */
    int modeBackGround;
    /**
     * @brief modeTrigger
     * @details mode=-1 means invalide
     *          mode=6 means triggerOut
     *          mode=7 means triggerIn
     *          mode=8 means colorSwitch
     */
    int modeTrigger; //1~5, -1 means invalide, 6-triggerOut 7-triggerIn 8-ColorSwitch
    int modeFilter;
    /** @} */

    /** @brief 定义滤镜效果的持续事件(周期) */
    int durationFilter;

    /**
     * @name steps
     * @{ */
    step_t stepBackGround;
    step_t stepFilter;
    step_t stepTrigger;
    /** @} */

    /**
     * @name Colors
     * @{ */
    RGB MC;
    RGB FC;
    RGB LC;
    /** @brief pre-MC, used in colorSwitch */
    RGB oldMC;
    /** @brief cached black, used to clear pixels */
    RGB RGBBlack;
    /** @} */

public:
    /**
     * @brief event
     */
    typedef enum event_t {
        /** @brief trigger start event */
        event_start = 0x01,
        /** @brief trigger end event */
        event_end = 0x02
    } event_t;

    /**
     * @brief 构造
     */
    iBlade(const iParam* p)
        : iEvent(EVENT_MODULE_ID_BLADE)
        , parameter(p)
        , iBladeDriver(BLADE_PIXEL)
        , stepBackGround(0, 2000 / BLADE_INTERVAL, -1)
        , stepTrigger(0, 0, 0)
        , stepFilter(0, 0, -1)
        , modeBackGround(1)
        , modeFilter(1)
        , modeTrigger(-1)
        , durationFilter(1000)
        , RGBBlack(0, 0, 0, 255)
    {
        setEventMask(0);
        isActive = false;
        needClear = true;
    }

    /**
     * @brief 析构
     * @note  需调用父类的析构
     */
    virtual ~iBlade()
    {
        delete vector;
    }

public:
    /**
     * @brief handle
     * @note should call it at another thread
     * @note call this function, interval=BLADE_INTERVAL(ms)
     */
    void hanlde();

    /**
     * @brief update cached parameter
     * @note  when etc. trigger'Out', trigger'ColorSwitch'
     */
    void parameterUpdate();

    /**
     * @brief start a trigger
     * @note maybe sub-class has some sepecific operator
     *        , so it's virtual method.
     * @return if start trigger is good.
     */
    virtual bool play(triggerID_t id, uint32_t audioDuration = 0);

    /**
     * @brief end a trigger
     * @note maybe sub-class has some sepecific operator
     *       , so it's virtual method.
     * @return if end trigger is good.
     */
    virtual bool abort(triggerID_t id = Unknow);
};
