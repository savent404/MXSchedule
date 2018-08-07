#pragma once

#include "iEvent.hpp"

class iKey : public iEvent {
public:
    /** @brief Event ID define */
    typedef enum event_t {
        KEY_1_PRESS = 0x01,
        KEY_2_PRESS = 0x02,
        KEY_1_RELEASE = 0x04,
        KEY_2_RELEASE = 0x08,
        KEY_1_CLICK = 0x10,
        KEY_2_CLICK = 0x20,
    } event_t;

    /** @brief key id define */
    typedef enum {
        key1,
        key2,
    } Key_t;

    /**
     * @brief iEvent
     * @note  defual enable event: key1&key2's click
     */
    iKey()
        : iEvent(EVENT_MODULE_ID_KEY)
    {
        setEventMask(KEY_1_CLICK | KEY_2_CLICK);
    }

    virtual ~iKey() {}
    /**
     * @brief get key status
     * @param id
     * @return if 0-not press | 1-pressed
     */
    virtual int getStatus(Key_t id) = 0;
    /**
     * @brief get last click duration
     * @param id etc. Key1,Key2
     * @return time:ms
     */
    virtual uint32_t lastClickDuration(Key_t id) = 0;
};
