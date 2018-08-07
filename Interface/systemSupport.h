#pragma once

#include <stdint.h>

#ifdef __cpluscplus
extern "C" {
#endif
/**
 * @brief 获取系统时间(ms)
 * @note  系统时间是启动后从0开始即时的时间
 * @return time(ms)
 */
uint32_t mGetCPUTime(void);

/**
 * @brief OS Delay(ms)
 */
void mDelay(uint32_t time);

#ifdef __cpluscplus
}
#endif