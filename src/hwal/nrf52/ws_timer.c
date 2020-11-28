/*
 * ws_timer.c
 *
 *  Created on: 18.02.2020
 *      Author: Damian Plonek
 */

#include "ws_timer.h"
#include "ws_task_queue.h"
#define WS_LOG_MODULE_NAME TIMR
#include "ws_log.h"

#include "app_timer.h"


#define WS_TIMER_TIMERS_NUM     2
#define WS_TIMER_INTERVAL       100 // ms

typedef struct
{
    uint32_t interval;
    uint32_t expire;
    WS_TimerCallback_f callback;

} WS_Timer_t;
#define WS_TIMER_INIT           ((WS_Timer_t) { 0, 0, NULL })

static void WS_TimerCallback(
    void *p_data,
    uint16_t data_size);
static void WS_TimerIrqHandler(
    void* context);


APP_TIMER_DEF(ws_timer);

static uint32_t activeTimers = 0;
static WS_Timer_t ws_timers[WS_TIMER_TIMERS_NUM];

WINSENS_Status_e WS_TimerInit(void)
{
    uint32_t i;
    for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
    {
        ws_timers[i] = WS_TIMER_INIT;
    }

    ret_code_t err_code;

    err_code = app_timer_init();
    WS_APP_ERROR(err_code);

    err_code = app_timer_create(&ws_timer, APP_TIMER_MODE_REPEATED, WS_TimerIrqHandler);
    WS_APP_ERROR(err_code);

    return WINSENS_OK;
}

void WS_TimerDeinit(void)
{
}

WINSENS_Status_e WS_TimerSetTimer(
    uint32_t interval,
    bool repeat,
    WS_TimerCallback_f callback,
    WS_TimerId_t *id)
{
    if (NULL == id)
    {
        return WINSENS_INVALID_PARAMS;
    }

    uint32_t i;
    for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
    {
        if (NULL == ws_timers[i].callback)
        {
            if (repeat)
            {
                ws_timers[i].interval = APP_TIMER_TICKS(interval);
            }
            else
            {
                ws_timers[i].interval = 0;
            }

            ws_timers[i].expire = app_timer_cnt_get() + APP_TIMER_TICKS(interval);
            ws_timers[i].callback = callback;
            *id = i;

            if (0 == activeTimers)
            {
                app_timer_start(ws_timer, APP_TIMER_TICKS(WS_TIMER_INTERVAL), NULL);
            }

            activeTimers++;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

void WS_TimerCancel(
    WS_TimerId_t id)
{
    if (id < WS_TIMER_TIMERS_NUM)
    {
        ws_timers[id] = WS_TIMER_INIT;
        activeTimers--;

        if (0 == activeTimers)
        {
            app_timer_stop(ws_timer);
        }
    }
}

static void WS_TimerCallback(
    void *p_data,
    uint16_t data_size)
{
    uint32_t i;
    for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
    {
        if (ws_timers[i].callback
            && app_timer_cnt_get() >= ws_timers[i].expire)
        {
            WS_TimerCallback_f callback = ws_timers[i].callback;

            if (ws_timers[i].interval)
            {
                ws_timers[i].expire += ws_timers[i].interval;
            }
            else
            {
                WS_TimerCancel(i);
            }

            callback(i);
        }
    }
}

static void WS_TimerIrqHandler(
    void* context)
{
    WS_TaskQueueAdd(NULL, 0, WS_TimerCallback);
}
