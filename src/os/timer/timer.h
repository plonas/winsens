/*
 * timer.h
 *
 *  Created on: 06.01.2019
 *      Author: Damian Plonek
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "winsens_types.h"

typedef uint32_t WS_TimerId_t;
typedef void (*WS_TimerCallback_f)(WS_TimerId_t timerId);

winsens_status_t ITimer_Init(void);
void ITimer_Deinit(void);

winsens_status_t ITimer_SetTimer(
    uint32_t interval,
    bool repeat,
    WS_TimerCallback_f callback,
    WS_TimerId_t *id);
void ITimer_Cancel(
    WS_TimerId_t id);


#endif /* TIMER_H_ */
