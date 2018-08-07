#pragma once

#include <stdint.h>
#include <string>
#include "systemSupport.h"

#ifndef USE_QT
#define USE_QT 1
#endif

/** @brief  trigger id */
typedef enum {
    Unknow = -1,
    Swing = 0,
    Slash = 1,
    Spin = 2,
    Stab = 3,
    Clash = 4,
    Blaster = 5,
    Force = 6,
    Lockup = 7,
    Combo = 8,
    In = 9,
    Out = 10,
    ColorSwitch = 11,
} triggerID_t;

extern const std::string typeIntParam[44];
extern const std::string typeFloatParam[6];
extern const std::string typeRGBParam[3];
extern const std::string triggerName[11];

/**
 * @brief triggerName2ID convert string to triggerID_t
 * @param trigger name
 * @return triggerID
 */
triggerID_t triggerName2ID(const char* trigger);
