#pragma once

#include "common.h"
#include "iAudio.h"
#include "iBlade.h"
#include "iEvent.hpp"
#include "iKey.h"
#include "iParam.h"
#include "iPower.h"
#include "iHand.h"

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
    stage_t stage;
    bool lockUpHoldOn;
    void errorHandle(uint32_t message);
    bool reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout);
public:
    iShechdule(iDriverList l);
    virtual ~iShechdule();

    void run();

    void changeStage(stage_t newStage);
    void playTrigger(triggerID_t id);
};
