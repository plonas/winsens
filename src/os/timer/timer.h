/*
 * timer.h
 *
 *  Created on: 06.01.2019
 *      Author: Damian Plonek
 */

#ifndef TIMER_H_
#define TIMER_H_


#include "winsens_types.h"


typedef enum
{
    TIMER_EVT_SIGNAL = (WINSENS_IDS_MODULE_TIMER << 16),
} timer_evt_t;

typedef struct timer_t
{
    uint32_t                    interval;
    void*                       prv;
    void*                       context;
    winsens_event_handler_t     callback;
    bool                        repeat;
} timer_ws_t;


winsens_status_t timer_init(void);

winsens_status_t timer_create(timer_ws_t *timer, winsens_event_handler_t callback, void* context);
void timer_destroy(timer_ws_t *timer);

winsens_status_t timer_start(timer_ws_t *timer, uint32_t interval_ms, bool repeat);
void timer_stop(timer_ws_t *timer);


#endif /* TIMER_H_ */
