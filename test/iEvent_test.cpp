#include "iEvent.hpp"
#include "gtest/gtest.h"
#include <queue>

static std::queue<uint32_t> message_quque;

void defaultEventSender(uint32_t message)
{
    message_quque.push(message);
}

bool defaultEventReciver(uint32_t&message, uint32_t timeout)
{
    if (message_quque.empty())
        return false;
    message = message_quque.front();
    message_quque.pop();
    return true;
}

TEST(iEventTest, checkMash)
{
    iEvent event(0);
    ASSERT_EQ(event.getEventMask(), 0xFFFF);
    for (int i = 0; i < 16; i++) {
        uint16_t mask = 1 << i;
        event.setEventMask(mask);

        ASSERT_EQ(event.getEventMask(), mask);
    }
}

TEST(iEventTest, checkModuleID)
{
    for (int i = 0; i < 16; i++)
    {
        uint16_t moduleID = 1 << i;
        iEvent event(moduleID);
        event.sendEvent(0);
    }
    for (int i = 0; i < 16; i++)
    {
        uint16_t except_id = 1 << i;
        uint32_t res;
        ASSERT_EQ(true, defaultEventReciver(message));
        ASSERT_EQ(except_id, iEvent::getModuleID(message));
    }
}

TEST(iEventTest, checkSignals)
{
    iEvent event_key(EVENT_MODULE_ID_KEY);
    event_key
    for (int i = 0; i < 16; i++) {
        uint16_t event = (1 << i);

        event_key.sendEvent(event);
    }
    for (int i = 0; i < 16; i++) {
        uint16_t except_event = 1 << i;
        uint32_t message; 
        ASSERT_EQ(true, defaultEventReciver(message));
        ASSERT_EQ(except_event, iEvent::getEvent(message));
    }
}