#pragma once

#include <stdint.h>

#ifndef EVENT_MODULE_ID_KEY
#define EVENT_MODULE_ID_KEY (1 << 0)
#endif

#ifndef EVENT_MODULE_ID_BLADE
#define EVENT_MODULE_ID_BLADE (1 << 1)
#endif

#ifndef EVENT_MODULE_ID_LED
#define EVENT_MODULE_ID_LED (1 << 2)
#endif

#ifndef EVENT_MODULE_ID_AUDIO
#define EVENT_MODULE_ID_AUDIO (1 << 3)
#endif

#ifndef EVENT_MODULE_ID_PARAM
#define EVENT_MODULE_ID_PARAM (1 << 4)
#endif

#ifndef EVENT_MODULE_ID_POWER
#define EVENT_MODULE_ID_POWER (1 << 5)
#endif

#ifndef EVENT_MODULE_ID_HAND
#define EVENT_MODULE_ID_HAND (1 << 6)
#endif

/**
 * @brief 外部声明的消息发送函数
 * @param message 包含了高16位的moduleID以及低16位的事件
 * @note 需要实现多进程间的事件传递
 */
extern void defaultEventSender(uint32_t message);

/**
 * @brief 外部声明的消息接收函数
 * @param[out] message 接收到的message
 * @param[in] timeout default:100ms
 * @return if recived a message, return false if timeout
 */
extern bool defaultEventReciver(uint32_t& message, uint32_t timeout = 100);

/**
 * @brief 模块间通信封装
 * @note  通过module ID 识别模块
 * @note  通过event_mask 过滤无用event
 */
class iEvent {
public:
    /**
     * @brief iEvent set module's ID
     * @param moduleID
     */
    iEvent(uint16_t moduleID)
        : event_mask(0xFFFF)
        , module_id(moduleID)
    {
    }

    /**
     * @brief setEventMask set event's mask
     * @note  each event is a bit mapping in 2Bytes
     */
    void setEventMask(uint16_t event)
    {
        event_mask = event;
    }

    /**
     * @brief getEventMask get event's mask
     * @return event_mask
     */
    uint16_t getEventMask()
    {
        return event_mask;
    }

    /**
     * @brief sendEvent send event
     * @param event which ready to send
     * @param sendFunc func, and sendFunc's parameter is 
     *        abination of module id and event.
     *        param = moduleID << 16 | event;
     * @note is event_mask & event == 0, sendFunc is invalid
     * @note sendFunc can do anything, etc.:call some function
     *       release a sem
     */
    void sendEvent(uint16_t event, void (*sendFunc)(uint32_t) = defaultEventSender)
    {
        uint16_t e = event & event_mask;
        if (e)
            sendFunc((module_id << 16) | e);
    }

    /**
     * @brief get moduleID from eventMessage
     * @param[in] eventMessage 32-bits message recieved
     * @return moduleID
     */
    static uint16_t getModuleID(const uint32_t eventMessage)
    {
        return uint16_t(eventMessage >> 16);
    }

    /**
     * @brief get event from eventMessave
     * @param[in] eventMessage 32-bits message recieved
     * @return recieved event(16-bits)
     */
    static uint16_t getEvent(const uint32_t eventMessage)
    {
        return uint16_t(eventMessage & 0xFFFF);
    }

private:
    /**
     * @brief event mask only this event can be recieved.
     */
    uint16_t event_mask;
    /**
     * @brief module_id module's ID
     */
    uint16_t module_id;
};
