#pragma once

#include "common.h"
#include "iParam.h"
#include <stdint.h>

class iAudio : public iEvent {
protected:
    const iParam* parameter;

public:
    typedef enum event_t {
        start = 0x01,
        end = 0x02,
    } event_t;

    iAudio(const iParam* p)
        : iEvent(EVENT_MODULE_ID_AUDIO)
        , parameter(p)
    {
        setEventMask(start);
    }

    virtual ~iAudio() {}

    virtual bool play(triggerID_t id) = 0;

    virtual bool abort(triggerID_t id) = 0;
};
