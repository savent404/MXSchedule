#pragma once

#include "iEvent.hpp"

class iHand : public iEvent {
public:
    /**
     * @brief iHande event
     * @details
     * event's name show as same as triggers
     */
    typedef enum event_t {
        handSwing = 0x1,
        handSlash = 0x2,
        handSpin = 0x4,
        handStab = 0x8,
        handClash = 0x10,
    } event_t;

    /**
     * @brief iHand
     * @note other mothod is not necessary, implement in sub-class
     */
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
