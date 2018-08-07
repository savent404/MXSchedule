#include "common.h"
#include "textHelper.h"

/**
 * @brief   typeIntParam 支持整数型的参数名
 * @details 包含:
 * - SETTING.txt
 *  - Mode       LED模式 0=Lens 1=NP
 *  - SystemVol  音量(0~5)
 *  - T_pon      开机按钮时长
 *  - T_poff     关机按钮时长
 *  - T_out      开剑按钮时长
 *  - T_in       收剑按钮时长
 *  - T_BankSwitch   切换bank按钮时长
 *  - T_autoin   自动收剑时间
 *  - T_autooff  自动关机时间
 *  - T_mute     关机状态下，按下B按键后按下A按键，超过T_mute时间进入静音模式
 *  - T_colorswitch  开剑状态下，按下B按键后按下A按键，超过T_colorswitch切换光效配置
 * - EFFECT.txt
 *  - Vol       相对音量(0~5)
 *  - M_AFC     Type 1 触发方式 (0=音频文件混音 1=hum 变频)
 *  - T_Mix     开剑时，hum参入混音的延时
 *  - T_Cycle   呼吸、闪烁、流动效果的周期
 *  - T_Spark   单次触发闪烁的周期
 *  - Sparkcount 触发闪烁次数
 *  - M_InOut   In/Out执行时间 (0=计时亮起 1=跟随音效)
 *  - T_LOut    开剑 LED 动画时间
 *  - T_LIn     收剑 LED 动画时间
 *  - T_Max     最大亮度
 *  - T_Min     最小亮度
 * - EFFECT.txt 一类(type 1) 触发参数
 *  - T_Swingfreeze
 *  - T_SpinFreeze
 * - EFFECT.txt 二类(type 2) 触发参数
 *  - T_Stabfreeze
 *  - T_Clashfreeze
 * - EFFECT.txt 三类(type 3) 触发参数
 *  - T_Blasterfreeze
 *  - T_BlasterDelay
 *  - T_Force
 *  - T_Lockup
 *  - T_LockupHold
 */
const std::string typeIntParam[44] = {
    // SETTING.txt
    "Mode",
    "SystemVol",
    "T_pon",
    "T_poff",
    "T_out",
    "T_in",
    "T_BankSwitch",
    "T_autoin",
    "T_autooff",
    "T_mute",
    "T_colorswitch",
    //EFFECT.txt
    "Vol",
    "M_AFC",
    "T_Mix",
    "T_Cycle",
    "T_Spark",
    "Sparkcount",
    "M_InOut",
    "T_LOut",
    "T_LIn",
    "T_Max",
    "T_Min",

    "T_Swingfreeze",
    "T_SpinFreeze",

    "T_Stabfreeze",
    "T_Clashfreeze",

    "T_Blasterfreeze",
    "T_BlasterDelay",
    "T_Force",
    "T_Lockup",
    "T_LockupHold",

    // NP
    "NP_Cset",
    "NP_PartSparkCount",
    "NP_Amode",
    "NP_Tflowlength",
    "NP_TComet",

    "NP_SwingMode",
    "NP_SpinMode",
    "NP_StabMode",
    "NP_ClashMode",
    "NP_BlasterMode",
    "NP_ForceMode",
    "NP_LockupMode",
    "NP_ComboMode",

};

const std::string typeFloatParam[6] = {
    "L_Max",
    "L_Min",
    "NP_Cdrift",
    "NP_PartSparkLength",
    "NP_TflowLength",
    "NP_Density"
};

const std::string typeRGBParam[3] = { "MC", "FC", "LC" };

const std::string triggerName[12] = {
    "Swing",
    "Slash",
    "Spin",
    "Stab",
    "Clash",
    "Blaster",
    "Force",
    "Lockup",
    "Combo",
    "In",
    "Out",
    "ColorSwitch",
};

triggerID_t triggerName2ID(const char* trigger)
{
    int triggerNum = sizeof(triggerName) / sizeof(string);

    return (triggerID_t)getArrayIndex(&triggerName[0], trigger, triggerNum);
}
