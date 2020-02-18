/*
 * ws_timer.c
 *
 *  Created on: 18.02.2020
 *      Author: Damian Plonek
 */

#include "ws_timer.h"

#include "nrf_drv_timer.h"


#define WS_TIMER_TIMERS_NUM      2

typedef struct
{
    uint32_t interval;
    WS_TimerCallback_f callback;

} WS_Timer_t;

static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context);

static const nrf_drv_timer_t ws_systemTimer = NRF_DRV_TIMER_INSTANCE(1);
static WS_Timer_t ws_timers[WS_TIMER_TIMERS_NUM] = { 0, NULL };

WINSENS_Status_e WS_TimerInit(void)
{
    // init a timer
    nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
    timer_cfg.frequency = NRF_TIMER_FREQ_31250Hz;
    uint32_t err_code = nrf_drv_timer_init(&ws_systemTimer, &timer_cfg, WS_TimerCallback);
    APP_ERROR_CHECK(err_code);
    nrf_drv_timer_extended_compare(&ws_systemTimer, NRF_TIMER_CC_CHANNEL0, 3125UL, NRF_TIMER_SHORT_COMPARE0_CLEAR_MASK, false);
    nrf_drv_timer_enable(&ws_systemTimer);

    uint32_t i;
    for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
    {
        ws_timers[i] = (WS_Timer_t) { 0, NULL };
    }
}

void WS_TimerDeinit(void)
{
    nrf_drv_timer_disable(&ws_systemTimer);
    nrf_drv_timer_uninit(&ws_systemTimer);
}

WINSENS_Status_e WS_TimerSetTimer(
    uint32_t interval,
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
            ws_timers[i].interval = interval;
            ws_timers[i].callback = callback;
            *id = i;
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
        ws_timers[id] = (WS_Timer_t) { 0, NULL };
    }
}

const void* WS_TimerGetNativeTimer(void)
{
    return &ws_systemTimer;
}

static void WS_TimerCallback(
    nrf_timer_event_t eventType,
    void* context)
{
    static uint32_t counter = 0;

    switch (eventType)
    {
        case NRF_TIMER_EVENT_COMPARE0:
            ++counter;
            uint32_t i;
            for (i = 0; i < WS_TIMER_TIMERS_NUM; ++i)
            {
                if (ws_timers[i].callback
                    && 0 == (counter % ws_timers[i].interval))
                {
                    ws_timers[i].callback(i);
                }
            }
            break;

        default:
            //Do nothing.
            break;
    }
}
