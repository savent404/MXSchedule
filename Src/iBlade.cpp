#include "iBlade.h"


void iBlade::drawBackGroundStatic()
{
    drawLine(MC, 0, getPixelNum());
}

void iBlade::drawBackGroundShade()
{
    float dif;
    parameter->getParameter("NP_Cdrift", dif);

    HSV hsv(MC);

    hsv.h += dif;
    RGB shade = hsv;
    drawShade(MC, shade, 0, getPixelNum());
}

void iBlade::drawBackGroundStaticRainbow()
{
    RGB red(255, 0, 0);
    RGB green(0, 255, 0);
    RGB blue(0, 0, 255);

    drawShade(red, green, 0, getPixelNum() / 2);
    drawShade(green, blue, getPixelNum() / 2, getPixelNum());
}

void iBlade::drawBackGroundDynamicRainbow(step_t &step)
{
    int interval = getPixelNum() / 3;
    int start_pos = getPixelNum() * step.nowStep / step.totalStep;
    RGB color[3];
    color[0] = RGB(255, 0, 0);
    color[1] = RGB(0, 255, 0);
    color[2] = RGB(0, 0, 255);

    for (int i = 0; i < 3; i++)
    {
        int pos = (start_pos + i*interval) % getPixelNum();
        drawShade(color[i], color[(i+1) % 3], pos, pos + interval);
    }
    int p = 2 - start_pos / interval;
    int pp = start_pos % interval - interval;
    drawShade(color[p], color[(p+1)%3], pp, pp + interval);
}

void iBlade::drawBackGroundFlame()
{
    float rate;
    if (flame == NULL)
    {
        mDebug(DEBUG_LEVEL_ERROR, "Not alloc mem for flame");
        return;
    }
    parameter->getParameter("NP_Density", rate);
    flame->update(ptr(), int(rate * 1.28f));
}

void iBlade::drawFilterStatic()
{
    int i = 0;
    int max, min, light, num = getPixelNum();
    RGB* _p = ptr();
    parameter->getParameter("L_Max", max);
    parameter->getParameter("L_Min", min);

    for (; i < num; i++, _p++)
    {
        light = _p->realLight();

        if (light == 0)
            light = 1;

        if (light > max)
        {
            _p->W = _p->W * max / light;
        }
        if (light < min)
        {
            int ans = _p->W * min / light;
            _p->W = ans > 255 ? 255 : ans;
        }
    }
}

void iBlade::drawFilterBreath(step_t& step)
{
    // rate [-1.0f...1.0f]
    float rate = float(step.nowStep) / step.totalStep * 2.0f - 1.0f;
    int max, min;
    parameter->getParameter("L_Max", max);
    parameter->getParameter("L_Min", min);
    uint8_t light = uint8_t(sinf(rate*M_PI) * (max-min)/2) + (max+min)/2;
    light = light * 255 / 100;
    int i = 0;
    RGB* _p = ptr();
    for (; i < getPixelNum(); i++, _p++)
    {
        _p->W = light;
    }
}

void iBlade::drawFilterFlow(step_t& step)
{
    int max, min, iwidth;
    float fwidth;
    RGB* _p = ptr();
    parameter->getParameter("L_Max", max);
    parameter->getParameter("L_Min", min);
    parameter->getParameter("NP_Tflowlength", fwidth);
    max = max * 255 / 100;
    min = min * 255 / 100;
    iwidth = int(fwidth / 100.0f * getPixelNum());
    if (iwidth == 0)
        iwidth = 1;
    for (int i = 0; i < getPixelNum(); i++, _p++)
    {
        float _x = float(i) / iwidth - step.nowStep * 2.0f / step.totalStep;
        float x = abs(fmod(abs(_x), 2.0f) - 1.0f) ;
        int y = int((min - max) * x + max);
        _p->W = y;
    }
}

void iBlade::drawFilterSpark()
{
    int max, min;
    parameter->getParameter("L_Max", max);
    parameter->getParameter("L_Min", min);
    max = max * 255 / 100;
    min = min * 255 / 100;
    int ligth = rand() % (max - min) + min;
    RGB* _p = ptr();
    for (int i = 0; i < getPixelNum(); i++, _p++)
    {
        _p->W = ligth;
    }
}

void iBlade::drawFilterRain()
{
    float rate = 0.0f;
    parameter->getParameter("NP_Density", rate);
    int loopCnt = rand() % 100 > rate ? 0 : 1;
    int ans;
    RGB* _p = ptr();
    for (int i = 0; i < loopCnt; i++)
    {
        int pos = rand() % getPixelNum();
        (_p + pos)->W = 0;
    }
    for (int i = 1; i < getPixelNum(); i++)
    {
        if (i) {
            ans = _p[i - 1].W + _p[i].W + _p[i + 1].W + 2;
            ans /= 3;
            _p[i].W = ans > 255 ? 255 : ans;
        } else {
            ans = _p[i].W + _p[i + 1].W + 2;
            ans /= 2;
            _p[i].W = ans > 255 ? 255 : ans;
        }
    }
}

void iBlade::drawFilterVolFollow()
{

}

void iBlade::drawTriggerNone()
{

}

void iBlade::drawTriggerSpark(step_t &step)
{
    if (step.nowStep > step.totalStep / 2)
        return;

    float drit;
    parameter->getParameter("NP_Cdrift", drit);
    HSV hsv(FC);
    hsv.h += drit;
    RGB difColor = hsv;
    drawShade(FC, difColor, 0, getPixelNum());
}

void iBlade::drawTriggerPartialSpark(step_t &step)
{

}

void iBlade::drawTriggerFollowVol()
{

}

void iBlade::drawTriggerComet(step_t &step)
{
    RGB *_p = ptr();
    RGB a;

    int length = int(getPixelNum() * BLADE_COMET_LENGTH);
    int posEnd = step.nowStep * getPixelNum() / step.totalStep;
    int pos = posEnd - length;
    int i = 0;

    if (pos >= 0)
        _p += pos;

    for (; i < length; i++, pos++)
    {
        if (pos < 0)
            continue;
        float rate = float(i) / length;
        _p->operator*=(1 - rate);
        a = FC;
        a *= rate;
        _p->operator +=(a);
        _p++;
    }
}

void iBlade::clearW()
{
    RGB* _p = ptr();
    for (int i = 0; i < getPixelNum(); i++, _p++)
        _p->W = 255;
}

void iBlade::parameterUpdate()
{
    parameter->getParameter("NP_Cset", modeBackGround);
    parameter->getParameter("NP_Amode", modeFilter);
    parameter->getParameter("T_Cycle", durationFilter);
    stepFilter.totalStep = durationFilter / BLADE_INTERVAL;
    oldMC = MC;
    parameter->getParameter("MC", MC);
    parameter->getParameter("FC", FC);
    parameter->getParameter("LC", LC);
}

void iBlade::hanlde()
{
    if (needClear)
    {
        RGB a;
        needClear = false;
        clearW();
        drawLine(a, 0, getPixelNum());
        update();
    }
    if (isActive == false)
        return;

    // modeTrigger >= 6 no needs to run BackGround draw
    if (modeTrigger < 6) {
        switch(modeBackGround)
        {
        case 1:
            drawBackGroundStatic();
            break;
        case 2:
            drawBackGroundShade();
            break;
        case 3:
            drawBackGroundStaticRainbow();
            break;
        case 4:
            drawBackGroundDynamicRainbow(stepBackGround);
            break;
        case 5:
            drawBackGroundFlame();
            break;
        default:
            mDebug(DEBUG_LEVEL_INFO, "Unknow back ground mode:%d", modeBackGround);
        }
    }
    // in case x/totalStep has div-0 error
    if (stepTrigger.totalStep <= 0)
        stepTrigger.totalStep = 1;
    switch(modeTrigger)
    {
    case -1:
        break;
    case 1:
        drawTriggerNone();
        break;
    case 2:
        drawTriggerSpark(stepTrigger);
        break;
    case 3:
        drawTriggerPartialSpark(stepTrigger);
        break;
    case 4:
        drawTriggerFollowVol();
        break;
    case 5:
        drawTriggerComet(stepTrigger);
        break;
    case 6:
        drawLine(MC, 0, stepTrigger.nowStep * getPixelNum() / stepTrigger.totalStep);
        break;
    case 7:
        drawLine(RGBBlack
                 , (stepTrigger.totalStep - stepTrigger.nowStep)
                   *getPixelNum() / stepTrigger.totalStep
                 , getPixelNum());
        break;
    case 8: {
        int p = stepTrigger.nowStep * getPixelNum() / stepTrigger.totalStep;
        drawShade(MC, oldMC, p - int(BLADE_COMET_LENGTH * getPixelNum()), p);
        break;
    }
    default:
        mDebug(DEBUG_LEVEL_INFO, "Unknow Trigger Mode:%d", modeTrigger);
    }
    switch(modeFilter)
    {
    case 1:
        drawFilterStatic();
        break;
    case 2:
        drawFilterBreath(stepFilter);
        break;
    case 3:
        drawFilterFlow(stepFilter);
        break;
    case 4:
        drawFilterSpark();
        break;
    case 5:
        drawFilterRain();
        break;
    case 6:
        drawFilterVolFollow();
        break;
    default:
        mDebug(DEBUG_LEVEL_INFO, "Unknow Filter mode:%d", modeFilter);
    }

    stepBackGround.walk();
    stepFilter.walk();

    // if return true, end trigger
    if (modeTrigger > 0 && stepTrigger.walk()) {
        // Trigger In
        if (modeTrigger == 7) {
            isActive = false;
            needClear = true;
        }
        modeTrigger = -1;
        sendEvent(event_end);
    }
}

bool iBlade::play(triggerID_t id, uint32_t duration)
{
    int mode = 0;
    switch (id)
    {
    case Swing:
        parameter->getParameter("NP_SwingMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Slash:
        parameter->getParameter("NP_SlashMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Spin:
        parameter->getParameter("NP_SpinMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Stab:
        parameter->getParameter("NP_StabMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Clash:
        parameter->getParameter("NP_ClashMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Blaster:
        parameter->getParameter("NP_BlasterMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Force:
        parameter->getParameter("NP_ForceMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Lockup:
        parameter->getParameter("NP_LockupMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;

        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case Combo:
        parameter->getParameter("NP_ComboMode", mode);
        parameter->getParameter("Sparkcount", stepTrigger.repeatCnt);
        stepTrigger.repeatCnt -= 1;
        if (mode == 5)
            parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        else if (mode == 4)
            stepTrigger.totalStep = duration;
        else
            parameter->getParameter("T_Spark", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = mode;
        break;
    case In: {
        parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        stepTrigger.repeatCnt = 0;
        modeTrigger = 7;
        break;
    }
    case Out : {
        int backGroundMode;
        parameterUpdate();
        clearW();
        parameter->getParameter("NP_Tcomet", stepTrigger.totalStep);
        parameter->getParameter("NP_Cset", backGroundMode);
        stepTrigger.totalStep /= BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        stepTrigger.repeatCnt = 0;
        modeTrigger = 6;
        isActive = true;
        needClear = true;
        if (backGroundMode == 5)
        {
            if (flame == NULL)
            {
                flame = new Flame;
            }
        }
        break;
    }
    case ColorSwitch:
        parameterUpdate();
        // Fixed white Comet, fixed duration
        stepTrigger.repeatCnt = 0;
        stepTrigger.totalStep = 600 / BLADE_INTERVAL;
        stepTrigger.nowStep = 0;
        modeTrigger = 8;
        break;
    }
    sendEvent(event_start);
    return true;
}

bool iBlade::abort(triggerID_t id)
{
    id = id;
    if (modeTrigger != -1)
    {
        modeTrigger = -1;
        sendEvent(event_end);
    }
    return false;
}
