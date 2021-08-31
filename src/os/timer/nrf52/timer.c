/*
 * timer.c
 *
 *  Created on: 18.02.2020
 *      Author: Damian Plonek
 */

#include "timer.h"
#include "timer_cfg.h"
#define ILOG_MODULE_NAME TMR
#include "log.h"
#include "log_internal_nrf52.h"

#include "app_timer.h"


#define NRF_TMR(t)       ((timer_nrf52_t*)(t->prv))


LOG_REGISTER();


typedef struct timer_nrf52_t
{
    app_timer_t     app_timer;
    bool            used;
} timer_nrf52_t;


static timer_nrf52_t* get_nrf_timer(void);
static void app_tmr_evt_handler(void* p_context);

static bool             g_initialized = false;
static timer_nrf52_t    g_timers[TIMER_CFG_TIMERS_NUM];


winsens_status_t timer_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        memset(g_timers, 0, sizeof(g_timers));

        ret_code_t err_code = app_timer_init();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
    }

    return WINSENS_OK;
}

winsens_status_t timer_create(timer_ws_t* timer, winsens_event_handler_t callback, void* context)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(NULL != callback, WINSENS_INVALID_PARAMS);

    timer_nrf52_t* nrf_tmr = get_nrf_timer();
    LOG_ERROR_BOOL_RETURN(NULL != nrf_tmr, WINSENS_NO_RESOURCES);

    nrf_tmr->used = true;
    timer->prv = nrf_tmr;
    timer->context = context;
    timer->callback = callback;

    return WINSENS_OK;
}

void timer_destroy(timer_ws_t *timer)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, ;);

    NRF_TMR(timer)->used = false;
}

winsens_status_t timer_start(timer_ws_t* timer, uint32_t interval_ms, bool repeat)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    app_timer_id_t app_tmr = &NRF_TMR(timer)->app_timer;
    app_timer_mode_t m = repeat ? APP_TIMER_MODE_REPEATED : APP_TIMER_MODE_SINGLE_SHOT;

    ret_code_t ret = app_timer_create(&app_tmr, m, app_tmr_evt_handler);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    ret = app_timer_start(app_tmr, APP_TIMER_TICKS(interval_ms), timer);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    return WINSENS_OK;
}

winsens_status_t timer_restart(timer_ws_t* timer, uint32_t interval_ms)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    ret_code_t ret = app_timer_stop(&NRF_TMR(timer)->app_timer);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    ret = app_timer_start(&NRF_TMR(timer)->app_timer, APP_TIMER_TICKS(interval_ms), timer);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    return WINSENS_OK;
}

void timer_stop(timer_ws_t *timer)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, ;);

    ret_code_t ret = app_timer_stop(&NRF_TMR(timer)->app_timer);
    LOG_NRF_ERROR_RETURN(ret, ;);

    // no need to destroy the app_timer, can be re-initialized again
}

static timer_nrf52_t* get_nrf_timer(void)
{
    for (uint32_t i = 0; i < TIMER_CFG_TIMERS_NUM; ++i)
    {
        if (!g_timers[i].used)
        {
            return &g_timers[i];
        }
    }

    return NULL;
}

static void app_tmr_evt_handler(void* p_context)
{
    const timer_ws_t* t = p_context;

    winsens_event_t e = { .id = TIMER_EVT_SIGNAL, .data = (uint32_t)t->context };
    t->callback(e);
}
