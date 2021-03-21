/*
 * TimerNrf52.c
 *
 *  Created on: 18.02.2020
 *      Author: Damian Plonek
 */

#include "timer.h"
#include "task_queue.h"
#define ILOG_MODULE_NAME TIMR
#include "log.h"
#include "log_internal_nrf52.h"

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


LOG_REGISTER();
APP_TIMER_DEF(ws_timer);


static uint32_t activeTimers = 0;
static WS_Timer_t ws_timers[WS_TIMER_TIMERS_NUM];

winsens_status_t ITimer_Init(void)
{
    uint32_t i;
    for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
    {
        ws_timers[i] = WS_TIMER_INIT;
    }

    ret_code_t err_code;

    err_code = app_timer_init();
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

    err_code = app_timer_create(&ws_timer, APP_TIMER_MODE_REPEATED, WS_TimerIrqHandler);
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void ITimer_Deinit(void)
{
}

winsens_status_t ITimer_SetTimer(
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

void ITimer_Cancel(
    WS_TimerId_t id)
{
    if (id < WS_TIMER_TIMERS_NUM)
    {
        ws_timers[id] = WS_TIMER_INIT;
        activeTimers--;

        if (0 == activeTimers)
        {
            ret_code_t ret = app_timer_stop(ws_timer);
            LOG_NRF_WARNING_CHECK(ret);
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
                ITimer_Cancel(i);
            }

            callback(i);
        }
    }
}

static void WS_TimerIrqHandler(
    void* context)
{
    task_queue_add(NULL, 0, WS_TimerCallback);
}
