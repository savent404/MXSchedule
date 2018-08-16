#pragma once

#include "common.h"
#include "iAudio.h"
#include "iBlade.h"
#include "iEvent.hpp"
#include "iHand.h"
#include "iKey.h"
#include "iParam.h"
#include "iPower.h"
#include <vector>

/**
 * @brief 实现相同触发的最小触发间隔
 */
class triggerTimeLocker {
    /** @brief 允许触发的最早时间 */
    uint32_t lastTime;
    /** @brief 设定的间隔时间 */
    uint32_t lockTime;

public:
    triggerTimeLocker(uint32_t lockTime = 0)
    {
        lastTime = 0;
    }

    /**
     * @brief 获取时间锁
     * @return true-获取成功
     */
    bool acquire()
    {
        uint32_t stamp = mGetCPUTime();
        if (stamp >= lastTime) {
            lastTime = stamp + lockTime;
            return true;
        }
        return false;
    }

    /**
     * @brief 设置最小时间间隔
     */
    void setLock(uint32_t time)
    {
        lockTime = time;
    }
};

/**
 * @brief 为iSchedule提供驱动列表
 */
typedef struct iDriverList {
    iAudio* audio;
    iParam* param;
    iBlade* blade;
    iKey* key;
    iPower* power;
    iHand* hand;
} iDriverList;

/**
 * @brief主进程
 */
class iShechdule {
public:
    /**
     * @brief user story
     */
    typedef enum stage_t {
        StageInit = 0,
        StageReady = 1,
        StageRunning = 2,
        StageCharging = 3,
        StageCharged = 4,
        StagePowerOff = 5,
    } stage_t;

    /**
     * @brief 运行态trigger中断分级
     * @details combo最低优先级(1)可被swing打断，
     *          所以triggerType中最低优先级的swing
     *          应当为2
     * @note  通过比较triggerType的value判断优先级高低
     */
    typedef enum triggerType_t {
        /** Combo's trigger level range is [1...4] */
        triggerType_1 = 2,
        triggerType_2 = 3,
        triggerType_3 = 4,
        triggerType_other = 5,
    } triggerType_t;

private:
    /** @brief 驱动列表 */
    iDriverList list;
    /** @brief 记录trigger的音轨位置 */
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

    /** @brief 记录当前用户story */
    stage_t stage;
    /** @brief 记录是否为Lock的Hold up状态 */
    bool lockUpHoldOn;
    /** @brief 每个trigger都拥有最小间隔(未定义的其间隔值为0) */
    triggerTimeLocker lockTrigger[12];
    /** @brief 当前在运行的trigger(无trigger运行时为Unknow) */
    triggerID_t idPlayingTrigger;
    /** @brief combo 记录序列 */
    std::vector<uint8_t> comboQueue;
    /** @brief 上一个trigger的触发时间 */
    uint32_t comboLastTime;
    /** @brief 应当触发的comboIndex */
    int comboIndex;

    /** @brief 在reciveSpecificEvent函数中处理重要错误事件 */
    void errorHandle(uint32_t message);

    /**
     * @brief 封装reciveEvent,只接受指定的moduleID和event事件
     * @note 可通过errorHandle处理紧急事件
     */
    bool reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout);

    /**
     * @brief 处理电源事件
     */
    void handlePowerManageEvent(uint16_t event);

    /**
     * @brief bankSwitch时更新各参数
     */
    void parameterUpdate();

    /**
     * @brief 得到trigger的优先级, iSchedule根据优先级可打断正在运行的trigger
     * @param id new Trigger
     * @return trigger's type(interrupt level)
     */
    triggerType_t classifyTriggerType(triggerID_t id);

    /**
     * @brief handleCombo
     * @param id if trigger a combo
     */
    bool handleCombo(triggerID_t id);

public:
    iShechdule(iDriverList l);
    virtual ~iShechdule();

    /**
     * @brief 主函数
     */
    void run();

    /**
     * @brief 切换用户story
     */
    void changeStage(stage_t newStage);

    /**
     * @brief 触发运行态触发
     * @return 是否触发成功
     */
    bool playTrigger(triggerID_t id);
};
