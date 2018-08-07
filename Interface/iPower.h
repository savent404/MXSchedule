#pragma once

#include <stdint.h>
#include "iEvent.hpp"

class iPower : public iEvent
{
protected:
    float now;
    float L1;
    float L2;
public:
    typedef enum event_t
    {
        event_level1 = 0x01,
        event_level2 = 0x02,
    } event_t;
public:
    iPower(float level1 = 3.5f, float level2 = 3.2f);
    virtual ~iPower();

    void measurement(float valueNow);

};
