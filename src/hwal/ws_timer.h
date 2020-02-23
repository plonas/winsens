/*
 * ws_timer.h
 *
 *  Created on: 06.01.2019
 *      Author: Damian Plonek
 */

#ifndef WS_TIMER_H_
#define WS_TIMER_H_

#include "winsens_types.h"

typedef uint32_t WS_TimerId_t;
typedef void (*WS_TimerCallback_f)(WS_TimerId_t timerId);

WINSENS_Status_e WS_TimerInit(void);
void WS_TimerDeinit(void);

WINSENS_Status_e WS_TimerSetTimer(
    uint32_t interval, // in 100ms units
    WS_TimerCallback_f callback,
    WS_TimerId_t *id);
void WS_TimerCancel(
    WS_TimerId_t id);

const void* WS_TimerGetNativeTimer(void);

#endif /* WS_TIMER_H_ */
