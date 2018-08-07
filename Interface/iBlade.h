#pragma once

#include "common.h"
#include "iEvent.hpp"
#include "iParam.h"
#include <stdint.h>

class iBlade : public iEvent {
protected:
    const iParam* parameter;

public:
    /**
     * @brief event
     */
    typedef enum event_t {
        start = 0x01,
        end = 0x02
    } event_t;

    iBlade(const iParam* p)
        : iEvent(EVENT_MODULE_ID_BLADE)
        , parameter(p)
    {
        setEventMask(start);
    }

    virtual ~iBlade() {}

    virtual bool play(triggerID_t id) = 0;

    virtual bool abort(triggerID_t id) = 0;
};