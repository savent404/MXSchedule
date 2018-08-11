#include "iBlade.h"


void iBlade::drawBackGroundStatic()
{
    RGB MC;
    parameter->getParameter("MC", MC);
    drawLine(MC, 0, getPixelNum());
}

void iBlade::drawBackGroundShade()
{
    RGB MC, LC;
    parameter->getParameter("MC", MC);
    parameter->getParameter("LC", LC);
    drawShade(MC, LC, 0, getPixelNum());
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
    step.walk();
}

void iBlade::drawBackGroundFlame()
{

}

void iBlade::drawFilterStatic()
{

}

void iBlade::drawFilterBreath()
{

}

void iBlade::drawFilterFlow()
{

}

void iBlade::drawFilterSpark()
{

}

void iBlade::drawFilterRain()
{

}

void iBlade::drawFilterVolFollow()
{

}

void iBlade::hanlde()
{
    if (needClear)
    {
        RGB a;
        needClear = false;
        drawLine(a, 0, 255);
    }
    if (isActive == false)
        return;

    int modeBackGround;

    parameter->getParameter("NP_Cset", modeBackGround);

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
    }
}

bool iBlade::play(triggerID_t id)
{
    return false;
}

bool iBlade::abort(triggerID_t id)
{
    return false;
}
