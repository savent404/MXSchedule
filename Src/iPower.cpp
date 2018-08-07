#include "iPower.h"

iPower::iPower(float level1, float level2) 
        : iEvent(EVENT_MODULE_ID_POWER)
        , L1(level1)
        , L2(level2)
        , now(4.2f)
{
    setEventMask(event_level1 | event_level2);
}

iPower::~iPower()
{

}

void iPower::measurement(float valueNow)
{
    if (valueNow < L1 && now >= L1)
    {
        sendEvent(event_level1);
    }
    if (valueNow < L2 && now >= L2)
    {
        sendEvent(event_level2);
    }
    now = valueNow;
}
