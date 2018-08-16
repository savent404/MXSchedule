#include "iSchedule.h"

using namespace std;

iShechdule::iShechdule(iDriverList l)
    : list(l)
    , stage(StageInit)
    , lockUpHoldOn(false)
    , trackIdTrigger(-1)
    , idPlayingTrigger(Unknow)
    , comboLastTime(0)

{
    list.key->setEventMask(iKey::KEY_1_PRESS | iKey::KEY_1_RELEASE |
        iKey::KEY_2_PRESS | iKey::KEY_2_RELEASE);
    list.blade->setEventMask(iBlade::event_end);
    parameterUpdate();
}

iShechdule::~iShechdule()
{
}

void iShechdule::run()
{
    const uint32_t defaultInterval = 10; // 10 ms

    uint16_t handEvent = 0;
#if USE_QT == 1
    changeStage(StageReady);
#else
    changeStage(StageReady);
#endif

    while (1)
    {
        // stage Charging->Charged or Charged->Charging
        if (StageCharging == stage &&
            iPower::stage_full == list.power->getStatus())
        {
            changeStage(StageCharged);
        }
        else if (StageCharged == stage &&
                 iPower::stage_full != list.power->getStatus())
        {
            changeStage(StageCharging);
        }

        if (defaultEventReciver(cached.message, defaultInterval) == false)
        {
            continue;
        }
        cached.moduleID = iEvent::getModuleID(cached.message);
        cached.event = iEvent::getEvent(cached.message);

        if (cached.moduleID == EVENT_MODULE_ID_POWER)
            handlePowerManageEvent(cached.event);
        // response Hand event
        if (StageRunning == stage && cached.moduleID == EVENT_MODULE_ID_HAND)
        {
            if (cached.event & iHand::handSwing)
                playTrigger(Swing);
            if (cached.event & iHand::handClash)
                playTrigger(Clash);
            if (cached.event & iHand::handSlash)
                playTrigger(Slash);
            if (cached.event & iHand::handSpin)
                playTrigger(Spin);
            if (cached.event & iHand::handStab)
                playTrigger(Stab);
        }
        // response Blade event
        if (cached.moduleID == EVENT_MODULE_ID_BLADE)
        {
            if (cached.event & (iBlade::event_end))
            {
                idPlayingTrigger = Unknow;
            }
        }
        // response Key board event
        if (cached.moduleID == EVENT_MODULE_ID_KEY &&
                cached.event & (iKey::KEY_1_PRESS | iKey::KEY_2_PRESS))
        {
            if (StageReady == stage)
            {
                if (cached.event & iKey::KEY_1_PRESS)
                {
                    int tOut, tPowerOff;
                    uint32_t stamp = mGetCPUTime();
                    list.param->getParameter("T_out", tOut);
                    list.param->getParameter("T_poff", tPowerOff);
                    stage_t biggerStage = tOut > tPowerOff ? StageRunning : StagePowerOff;
                    stage_t smallerStage = tOut > tPowerOff ? StagePowerOff : StageRunning;
                    int biggerTime = tOut > tPowerOff ? tOut : tPowerOff;
                    uint32_t smallerTime = tOut > tPowerOff ? tPowerOff : tOut;
                    if (reciveSpecificEvent(cached.message,
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
                else if (cached.event & iKey::KEY_2_PRESS)
                {
                    int tBankSwitch;
                    list.param->getParameter("T_BankSwitch", tBankSwitch);
                    if (!reciveSpecificEvent(cached.message,
                                            EVENT_MODULE_ID_KEY,
                                            iKey::KEY_2_RELEASE,
                                            tBankSwitch))
                    {
                        list.param->switchBank();
                        parameterUpdate();
                        mDebug(DEBUG_LEVEL_INFO,
                               "Bank Switch:%d/%d:%s",
                               list.param->getBankPos() + 1,
                               list.param->getBankNum(),
                               list.param->getBankName().c_str());
                        cached.str = list.param->getBankName() + "Bankswitch.wav";
                        list.audio->_play(cached.str.c_str());
                        reciveSpecificEvent(cached.message,
                                            EVENT_MODULE_ID_AUDIO,
                                            iAudio::end,
                                            uint32_t(-1));
                    }
                }
            }
            else if (StageRunning == stage)
            {
                if (cached.event & iKey::KEY_1_PRESS)
                {
                    int tIn, tColorSwitch;
                    list.param->getParameter("T_in", tIn);
                    list.param->getParameter("T_colorswitch", tColorSwitch);
                    if (reciveSpecificEvent(cached.message,
                                            EVENT_MODULE_ID_KEY,
                                            iKey::KEY_1_RELEASE | iKey::KEY_2_PRESS,
                                            tIn))
                    {
                        cached.event = iKey::getEvent(cached.message);
                        if (cached.event & iKey::KEY_1_RELEASE)
                        {
                        }
                        else if (cached.event & iKey::KEY_2_PRESS)
                        {
                            if (!reciveSpecificEvent(cached.message,
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
                else if (cached.event & iKey::KEY_2_PRESS)
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
                        reciveSpecificEvent(cached.message,
                                            EVENT_MODULE_ID_HAND | EVENT_MODULE_ID_KEY,
                                            handMask | iKey::KEY_2_RELEASE,
                                            tForce > tLockup ? tForce : tLockup))
                    {
                        cached.moduleID = iEvent::getModuleID(cached.message);
                        cached.event = iEvent::getEvent(cached.message);
                        if (cached.moduleID == EVENT_MODULE_ID_HAND)
                            playTrigger(Force);
                        else
                            playTrigger(Blaster);
                    }
                    else
                    {
                        playTrigger(Lockup);
                        if (reciveSpecificEvent(cached.message,
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
                cached.str = list.param->getPrefixPath() + "System/charging.wav";
                list.audio->_play(cached.str.c_str());
            }
            else if (StageCharging == stage)
            {
                cached.str = list.param->getPrefixPath() + "System/charging.wav";
                list.audio->_play(cached.str.c_str());
            }
        }
    }
}
bool iShechdule::reciveSpecificEvent(uint32_t& message, uint16_t moduleID, uint16_t event, uint32_t timeout)
{
    uint32_t stamp = mGetCPUTime();
    bool flag = false;
    while (!flag)
    {
        uint32_t spendTime = mGetCPUTime() - stamp;
        if (spendTime > timeout)
            return false;
        bool stamp = defaultEventReciver(message, timeout - spendTime);
        if (stamp == true)
        {
            cached.moduleID = iEvent::getModuleID(message);
            cached.event = iEvent::getEvent(message);
            errorHandle(message);
            if (cached.moduleID & moduleID && (cached.event & event))
                flag = true;
        }
        else
            return false;
    }
    return true;
}

void iShechdule::handlePowerManageEvent(uint16_t event)
{
    if (event & iPower::event_level2)
    {
        mDebug(DEBUG_LEVEL_INFO, "Recharge");
        bool warningEnable = StageReady == stage ||
                StageRunning == stage ||
                StageInit == stage;
        if (warningEnable)
        {
            for (int i = 0; i < 3; i++)
                list.audio->abort(i);
            cached.str = list.param->getPrefixPath() + "System/Recharge.wav";
            list.audio->_play(cached.str.c_str());
            reciveSpecificEvent(cached.message,
                                EVENT_MODULE_ID_AUDIO,
                                iAudio::end,
                                uint32_t(-1));
            stage = StageReady;
            changeStage(StagePowerOff);
            return;
        }
    }
    if (event & iPower::event_level1)
    {
        mDebug(DEBUG_LEVEL_INFO, "LowPower");
        bool warningEnable = StageReady == stage ||
                StageRunning == stage ||
                StageInit == stage;
        if (warningEnable)
        {
            cached.str = list.param->getPrefixPath() + "System/lowpower.wav";
            list.audio->_play(cached.str.c_str());
        }
    }
    if (event & iPower::event_plugin)
    {
        changeStage(StageCharging);
    }
    if (event & iPower::event_plugout)
    {
        changeStage(StagePowerOff);
    }
}

void iShechdule::parameterUpdate()
{
    int aInt;
    if (list.param->getParameter("T_SwingFreeze", aInt))
        lockTrigger[Swing].setLock(aInt);
    else
        lockTrigger[Swing].setLock(0);
    if (list.param->getParameter("T_SpinFreeze", aInt))
        lockTrigger[Spin].setLock(aInt);
    else
        lockTrigger[Spin].setLock(0);
    if (list.param->getParameter("T_StabFreeze", aInt))
        lockTrigger[Stab].setLock(aInt);
    else
        lockTrigger[Stab].setLock(0);
    if (list.param->getParameter("T_ClashFreeze", aInt))
        lockTrigger[Clash].setLock(aInt);
    else
        lockTrigger[Clash].setLock(0);
    if (list.param->getParameter("T_BlasterFreeze", aInt))
        lockTrigger[Blaster].setLock(aInt);
    else
        lockTrigger[Blaster].setLock(0);
    lockTrigger[Combo].setLock(0);
    lockTrigger[Out].setLock(0);
    lockTrigger[In].setLock(0);
    lockTrigger[ColorSwitch].setLock(0);
    lockTrigger[Lockup].setLock(0);
    lockTrigger[Force].setLock(0);
    lockTrigger[Blaster].setLock(0);
}

iShechdule::triggerType_t iShechdule::classifyTriggerType(triggerID_t id)
{
    if (id == Swing || id == Spin || id == Slash)
        return triggerType_1;
    else if (id == Stab || id == Clash)
        return triggerType_2;
    else if (id == Blaster || id == Force)
        return triggerType_3;
    return triggerType_other;
}

bool iShechdule::handleCombo(triggerID_t id)
{

    // storage tirggerType
    uint8_t tTrigger = classifyTriggerType(id) - 2;
    int combo_T;
    list.param->getParameter("T_combo", combo_T);
    if (comboLastTime <= mGetCPUTime())
    {
        comboQueue.clear();
    }
    comboQueue.push_back(tTrigger);
    comboLastTime = mGetCPUTime() + combo_T;
    // check if match any combo sequence
    const vector<iParam::combo_t>* l = list.param->comboList();
    int comboMaxLen = 0;
    for (size_t i = 0; i < l->size(); i++)
    {
        bool isSame = true;
        if (comboMaxLen < l->at(i).len)
            comboMaxLen = l->at(i).len;
        if (l->at(i).len > comboQueue.size())
            continue;
        for (size_t j = 0; j < l->at(i).len; j++)
        {
            if (l->at(i).sequence[j] != comboQueue[j + comboQueue.size() - l->at(i).len])
            {
                comboIndex = int(i);
                isSame = false;
                break;
            }
        }
        if (isSame)
        {
            comboQueue.clear();
            return true;
        }
    }
    if (comboMaxLen < comboQueue.size())
    {
        comboQueue.erase(comboQueue.begin(),
                         comboQueue.begin() + comboQueue.size() - comboMaxLen);
        mDebug(DEBUG_LEVEL_VERBOSS, "Left comboQueue Size:%ud", comboQueue.size());
    }
    return false;
}

void iShechdule::errorHandle(uint32_t message)
{
    cached.moduleID = iEvent::getModuleID(message);
    cached.event = iEvent::getEvent(message);
    if (cached.moduleID == EVENT_MODULE_ID_POWER)
        handlePowerManageEvent(cached.event);
}
void iShechdule::changeStage(stage_t newStage)
{
    stage_t now = stage;
    stage_t next = newStage;

    /** bit map
     * \ | 0 1 2 3 4 5 | new\now
     * - | - - - - - - -
     * 0 | - O O X O O |
     * 1 | X - X X O O |
     * 2 | O X - X O O |
     * 3 | O O O - X O |
     * 4 | O O O X - O |
     * 5 | O X O X X - |
     */

    if (StageInit == now && StageReady == next)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage from init -> ready");
        stage = StageReady;
        cached.str = list.param->getPrefixPath() + "System/Boot.wav";
        list.audio->_play(cached.str.c_str());
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
        mDebug(DEBUG_LEVEL_INFO, "Stage from ready -> power off");
        stage = StagePowerOff;
        cached.str = list.param->getPrefixPath() + "System/Poweroff.wav";
        list.audio->_play(cached.str.c_str());
    }
    else if ((StageCharged == now || StageCharging == now) &&
             StagePowerOff == next)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage from charge -> power off");
        stage = StagePowerOff;
    }
    else if (StageReady == now && StageRunning == next)
    {
        // TODO: Trigger Out
        mDebug(DEBUG_LEVEL_INFO, "Stage from ready -> run");
        playTrigger(Out);
        stage = StageRunning;
    }
    else if (StageCharging == next && StagePowerOff != now)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage goto charging");
        stage = StageCharging;
    }
    else if (StageCharged == next && StageCharging == now)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage from charging->charged");
        stage = StageCharged;
    }
    else if (StageCharged == now && StageCharging == next)
    {
        mDebug(DEBUG_LEVEL_INFO, "Stage from charged->charging");
        stage = StageCharging;
    }
    else
    {
        mDebug(DEBUG_LEVEL_ERROR, "Unspport stage change:%d->%d",
               now, next);
    }
}

bool iShechdule::playTrigger(triggerID_t id)
{
    if (lockTrigger[id].acquire() == false)
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "trigger(%d) not triggered cause of time lock",
               id);
        return false;
    }
    // chekc if can interrupt
    if (idPlayingTrigger != Unknow)
    {
        triggerType_t newTriggerType = id == Combo
                ? triggerType_t(list.param->comboList()->at(comboIndex).priority)
                : classifyTriggerType(id);
        triggerType_t oldTriggerType = classifyTriggerType(idPlayingTrigger);
        if (oldTriggerType >= newTriggerType)
        {
            mDebug(DEBUG_LEVEL_VERBOSS,
                   "new Trigger(%d) cant interrupt old trigger(%d)",
                   id, idPlayingTrigger);
            return false;
        }
        list.blade->abort();
        reciveSpecificEvent(cached.message,
                            EVENT_MODULE_ID_BLADE,
                            iBlade::event_end,
                            10);
        list.audio->abort(trackIdTrigger);
        reciveSpecificEvent(cached.message,
                            EVENT_MODULE_ID_AUDIO,
                            iAudio::end,
                            10);
    }

    if (handleCombo(id)) {
        if (playTrigger(Combo) == true)
            return true;
    }

    idPlayingTrigger = id;

    switch (id)
    {
    case ColorSwitch:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:ColorSwitch");
        list.param->incColorPos();
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Lockup:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Lockup start");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Blaster:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Blaster");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Force:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Forec");
        list.blade->play(id);
        break;
    }
    case Out:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Out");
        list.audio->mainTrack(true);
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case In:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:In");
        // if not call list.bale->parameterUpdate(), Color remains
        list.param->resetColorPos();
        trackIdTrigger = list.audio->play(id);
        list.audio->mainTrack(false);
        list.blade->play(id);
        break;
    }
    case Swing:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Swing");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Stab:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Stab");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Spin:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Spin");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Slash:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Slash");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Clash:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Clash");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id);
        break;
    }
    case Combo:
    {
        mDebug(DEBUG_LEVEL_VERBOSS, "T:Combo");
        trackIdTrigger = list.audio->play(id);
        list.blade->play(id, comboIndex);
        break;
    }
    default:
    {
        mDebug(DEBUG_LEVEL_ERROR, "Unknow trigger:%d", id);
        break;
    }
    }
}
