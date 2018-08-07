#pragma once

#include "iEvent.hpp"

class iHand : public iEvent {
public:
    typedef enum event_t {
        handSwing = 0x1,
        handSlash = 0x2,
        handSpin = 0x4,
        handStab = 0x8,
        handClash = 0x10,
    } event_t;

    iHand():iEvent(EVENT_MODULE_ID_HAND)
    {
        setEventMask(
            handSwing |
            handSlash |
            handSpin |
            handStab |
            handClash
        );
    }
};