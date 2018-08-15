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

    typedef enum triggerType_t {
       triggerType_1 = 0,
       triggerType_2 = 1,
       triggerType_3 = 2,
       triggerType_other = 3,
    } triggerType_t;

private:
    iDriverList list;
    iAudio::trackId_t trackIdTrigger;
protected:
    /**
     * @brief 暂存的一般变量
     * @note  暂存str减少构造、析构次数
     * @note  暂存Event message可减少模块内输入参数开销
     */
    struct cacheVar {
        std::string str;
        uint32_t message;
        uint16_t moduleID;
        uint16_t event;
    } cached;

    stage_t stage;
    bool lockUpHoldOn;
    triggerTimeLocker lockTrigger[12];
    triggerID_t idPlayingTrigger;

    void errorHandle(uint32_t message);
    bool reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout);
    void handlePowerManageEvent(uint16_t event);
    void parameterUpdate();

    /**
     * @brief 得到trigger的优先级, iSchedule根据优先级可打断正在运行的trigger
     * @param id new Trigger
     * @return trigger's type(interrupt level)
     */
    triggerType_t classifyTriggerType(triggerID_t id);
public:
    iShechdule(iDriverList l);
    virtual ~iShechdule();

    void run();

    void changeStage(stage_t newStage);
    void playTrigger(triggerID_t id);
};
