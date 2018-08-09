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
        event_level1 = 0x01, // warning level
        event_level2 = 0x02, // critical level
        event_plugin = 0x04, // charge plug in
        event_plugout= 0x08, // charge plug out
    } event_t;
public:
    iPower(float level1 = 3.5f, float level2 = 3.2f);
    virtual ~iPower();

    /**
     * @brief measure voltage and send message.
     * @param[in] valueNow voltage value, etc. 4.2f
     */
    void measurement(float valueNow);

};
