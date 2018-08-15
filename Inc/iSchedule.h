#pragma once

#include "common.h"
#include "iAudio.h"
#include "iBlade.h"
#include "iEvent.hpp"
#include "iKey.h"
#include "iParam.h"
#include "iPower.h"
#include "iHand.h"

class triggerTimeLocker
{
    uint32_t lastTime;
    uint32_t lockTime;
public:
    triggerTimeLocker(uint32_t lockTime = 0) {
        lastTime = 0;
    }

    bool acquire()
    {
        uint32_t stamp = mGetCPUTime();
        if (stamp >= lastTime)
        {
            lastTime = stamp + lockTime;
            return true;
        }
        return false;
    }

    void setLock(uint32_t time)
    {
        lockTime = time;
    }
};

typedef struct iDriverList {
    iAudio* audio;
    iParam* param;
    iBlade* blade;
    iKey* key;
    iPower* power;
    iHand* hand;
} iDriverList;

class iShechdule {
public:
    typedef enum stage_t {
        StageInit = 0,
        StageReady = 1,
        StageRunning = 2,
        StageCharging = 3,
        StageCharged = 4,
        StagePowerOff = 5,
    } stage_t;

private:
    iDriverList list;
    iAudio::trackId_t trackIdTrigger;
protected:

    struct cacheVar {
        std::string str;
        uint32_t message;
        uint16_t moduleID;
        uint16_t event;
    } cached;

    stage_t stage;
    bool lockUpHoldOn;
    triggerTimeLocker lockTrigger[12];
    void errorHandle(uint32_t message);
    bool reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout);
    void handlePowerManageEvent(uint16_t event);
    void parameterUpdate();
public:
    iShechdule(iDriverList l);
    virtual ~iShechdule();

    void run();

    void changeStage(stage_t newStage);
    void playTrigger(triggerID_t id);
};
