#include "iSchedule.h"

using namespace std;

iShechdule::iShechdule(iDriverList l)
    : list(l), stage(StageInit), lockUpHoldOn(false), trackIdTrigger(-1)
{
    list.key->setEventMask(iKey::KEY_1_PRESS | iKey::KEY_1_RELEASE |
        iKey::KEY_2_PRESS | iKey::KEY_2_RELEASE);
}

iShechdule::~iShechdule()
{
}

void iShechdule::run()
{
    const uint32_t defaultInterval = 10; // 10 ms

    uint32_t message;
    uint16_t moduleID;
    uint16_t event;

    uint16_t handEvent = 0;
    
    static string audioPath;

#if USE_QT == 1
    changeStage(StageReady);
#else
    changeStage(StageReady);
#endif

    while (1)
    {
        if (defaultEventReciver(message, defaultInterval) == false)
        {
            continue;
        }
        moduleID = iEvent::getModuleID(message);
        event = iEvent::getEvent(message);

        // response Key board event
        if (moduleID == EVENT_MODULE_ID_KEY)
        {
            if (StageReady == stage)
            {
                if (event & iKey::KEY_1_PRESS)
                {
                    int tOut, tPowerOff;
                    uint32_t stamp = mGetCPUTime();
                    list.param->getParameter("T_out", tOut);
                    list.param->getParameter("T_poff", tPowerOff);
                    stage_t biggerStage = tOut > tPowerOff ? StageRunning : StagePowerOff;
                    stage_t smallerStage = tOut > tPowerOff ? StagePowerOff : StageRunning;
                    int biggerTime = tOut > tPowerOff ? tOut : tPowerOff;
                    int smallerTime = tOut > tPowerOff ? tPowerOff : tOut;
                    if (reciveSpecificEvent(message,
                                            EVENT_MODULE_ID_KEY,
                                            iKey::KEY_1_RELEASE,
                                            biggerTime))
                    {
                        uint32_t spendTime = mGetCPUTime() - stamp;
                        if (spendTime < smallerTime)
                        {

                        }
                        changeStage(smallerStage);
                    }
                    else
                    {
                        changeStage(biggerStage);
                    }
                }
                else if (event & iKey::KEY_2_PRESS)
                {
                    int tBankSwitch;
                    list.param->getParameter("T_BankSwitch", tBankSwitch);
                    if (!reciveSpecificEvent(message,
                                            EVENT_MODULE_ID_KEY,
                                            iKey::KEY_2_RELEASE,
                                            tBankSwitch))
                    {
                        list.param->switchBank();
                        mDebug(DEBUG_LEVEL_INFO,
                               "Bank Switch:%d/%d:%s",
                               list.param->getBankPos() + 1,
                               list.param->getBankNum(),
                               list.param->getBankName().c_str());
                        audioPath = list.param->getBankName() + "Bankswitch.wav";
                        list.audio->_play(audioPath.c_str());
                        reciveSpecificEvent(message,
                                            EVENT_MODULE_ID_AUDIO,
                                            iAudio::end,
                                            uint32_t(-1));
                    }
                }
            }
            else if (StageRunning == stage)
            {
                if (event & iKey::KEY_1_PRESS)
                {
                    int tIn, tColorSwitch;
                    list.param->getParameter("T_in", tIn);
                    list.param->getParameter("T_colorswitch", tColorSwitch);

                    if (reciveSpecificEvent(message,
                                            EVENT_MODULE_ID_KEY,
                                            iKey::KEY_1_RELEASE | iKey::KEY_2_PRESS,
                                            tIn))
                    {
                        uint16_t event = iKey::getEvent(message);
                        if (event & iKey::KEY_1_RELEASE)
                        {
                        }
                        else if (event & iKey::KEY_2_PRESS)
                        {
                            if (!reciveSpecificEvent(message,
                                                     EVENT_MODULE_ID_KEY,
                                                     iKey::KEY_2_RELEASE,
                                                     tColorSwitch))
                            {
                                playTrigger(ColorSwitch);
                            }
                        }
                    }
                    else
                    {
                        changeStage(StageReady);
                    }
                }
                else if (event & iKey::KEY_2_PRESS)
                {
                    if (lockUpHoldOn == true)
                    {
                    STOPLOCKUP:
                        lockUpHoldOn = false;
                        mDebug(DEBUG_LEVEL_VERBOSS, "T:lock up stop");
                        list.audio->abort(trackIdTrigger);
                        continue;
                    }
                    int tForce, tLockup, tLockHold;

                    list.param->getParameter("T_force", tForce);
                    list.param->getParameter("T_Lockup", tLockup);
                    list.param->getParameter("T_LockupHold", tLockHold);

                    uint16_t handMask = iHand::handClash | iHand::handSlash | iHand::handSpin;
                    if (handMask & handEvent ||
                        reciveSpecificEvent(message,
                                            EVENT_MODULE_ID_HAND | EVENT_MODULE_ID_KEY,
                                            handMask | iKey::KEY_2_RELEASE,
                                            tForce > tLockup ? tForce : tLockup))
                    {
                        moduleID = iEvent::getModuleID(message);
                        event = iEvent::getEvent(message);
                        if (moduleID == EVENT_MODULE_ID_HAND)
                            playTrigger(Force);
                        else
                            playTrigger(Blaster);
                    }
                    else
                    {
                        playTrigger(Lockup);
                        if (reciveSpecificEvent(message,
                                                EVENT_MODULE_ID_KEY,
                                                iKey::KEY_2_RELEASE,
                                                tLockHold))
                        {
                            goto STOPLOCKUP;
                        }
                        else
                        {
                            mDebug(DEBUG_LEVEL_VERBOSS, "Lock up Holding");
                            lockUpHoldOn = true;
                        }
                    }
                        
                }

            }
            else if (StageCharged == stage)
            {
                audioPath = list.param->getPrefixPath() + "System/Charging.wav";
                list.audio->_play(audioPath.c_str());
                reciveSpecificEvent(message,
                                    EVENT_MODULE_ID_AUDIO,
                                    iAudio::end,
                                    uint32_t(-1));
            }
            else if (StageCharging == stage)
            {
                audioPath = list.param->getPrefixPath() + "System/Charging.wav";
                list.audio->_play(audioPath.c_str());
                reciveSpecificEvent(message,
                                    EVENT_MODULE_ID_AUDIO,
                                    iAudio::end,
                                    uint32_t(-1));
            }
        }
    }
}
bool iShechdule::reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout)
{
    uint32_t stamp = mGetCPUTime();
    uint16_t rM = 0, rE = 0; 
    bool flag = false;
    while (!flag)
    {
        int spendTime = mGetCPUTime() - stamp;
        if (spendTime > timeout)
            return false;
        bool stamp = defaultEventReciver(message, timeout - spendTime);
        if (stamp == true)
        {
            rM = iEvent::getModuleID(message);
            rE = iEvent::getEvent(message);
            errorHandle(message);
            if (rM & moduleID && (rE & event))
                flag = true;
        }
        else
            return false;
    }
    return true;
}

void iShechdule::errorHandle(uint32_t message)
{
    // no thing
}
void iShechdule::changeStage(stage_t newStage)
{
    stage_t now = stage;
    stage_t next = newStage;
    static string audioPath;
    uint32_t message;
    uint16_t moduleID;
    uint16_t event;

    /** bit map
     * \ | 0 1 2 3 4 5 | new\now
     * - | - - - - - - -
     * 0 | - O O O O O |
     * 1 | X - X O O O |
     * 2 | O X - O O O |
     * 3 | O O O - O O |
     * 4 | O O O O - O |
     * 5 | O X O O O - |
     */

    if (StageInit == now && StageReady == next)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage from init -> ready");
        stage = StageReady;
        audioPath = list.param->getPrefixPath() + "System/Boot.wav";
        list.audio->_play(audioPath.c_str());
        reciveSpecificEvent(message,
                            EVENT_MODULE_ID_AUDIO,
                            iAudio::end,
                            uint32_t(-1));
    }
    else if (StageRunning == now && StageReady == next)
    {
        // TODO: Play trigger 'in'
        mDebug(DEBUG_LEVEL_INFO, "Stage from run -> ready");
        if (lockUpHoldOn == true)
        {
            lockUpHoldOn = false;
            mDebug(DEBUG_LEVEL_VERBOSS, "T:lock up stop");
            // TODO: stop lock up
        }
        playTrigger(In);
        stage = StageReady;
    }
    else if (StageReady == now && StagePowerOff == next)
    {
        // TODO: System power off
        mDebug(DEBUG_LEVEL_INFO, "Stage from ready -> power off");
        stage = StagePowerOff;
    }
    else if (StageReady == now && StageRunning == next)
    {
        // TODO: Trigger Out
        mDebug(DEBUG_LEVEL_INFO, "Stage from ready -> run");
        playTrigger(Out);
        stage = StageRunning;
    }
}

void iShechdule::playTrigger(triggerID_t id)
{
    switch (id)
    {
    case ColorSwitch:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:ColorSwitch");
        trackIdTrigger = list.audio->play(id);
        break;
    }
    case Lockup:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Lockup start");
        trackIdTrigger = list.audio->play(id);
        break;
    }
    case Blaster:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Blaster");
        trackIdTrigger = list.audio->play(id);
        break;
    }
    case Force:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Forec");
        break;
    }
    case Out:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Out");
        list.audio->mainTrack(true);
        trackIdTrigger = list.audio->play(id);
        break;
    }
    case In:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:In");
        trackIdTrigger = list.audio->play(id);
        list.audio->mainTrack(false);
        break;
    }
    default:
    {
        break;
    }
    }
}
