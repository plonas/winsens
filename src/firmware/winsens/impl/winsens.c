/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "acc.h"
#include "hmi.h"
#include "winsens.h"
#include "winsens_cfg.h"
#include "window_state.h"
#include "window_state_cfg.h"
#include "timer.h"
#define ILOG_MODULE_NAME WNSN
#include "log.h"

#define WINSENS_WINDOWS_NUM     ( sizeof(g_windows)/sizeof(window_id_t) )
#define WINSENS_SCAN_DURATION   (10000) // ms


LOG_REGISTER();


static void window_state_callback(winsens_event_t evt);
static void acc_evt_handler(winsens_event_t evt);
static void tmr_evt_handler(winsens_event_t evt);


static window_id_t  g_windows[]         = WINSENS_CFG_WINDOWS_INIT;
static bool         g_windows_enabled   = false;
static timer_ws_t   g_timer;


winsens_status_t winsens_init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    status = acc_init();
    LOG_ERROR_RETURN(status, status);

    status = window_state_init();
    LOG_ERROR_RETURN(status, status);

    status = hmi_init();
    LOG_ERROR_RETURN(status, status);

    status = timer_init();
    LOG_ERROR_RETURN(status, status);

    status = timer_create(&g_timer, tmr_evt_handler, NULL);
    LOG_ERROR_RETURN(status, status);

    status = acc_subscribe(acc_evt_handler);
    LOG_ERROR_RETURN(status, status);

    status = window_state_subscribe(window_state_callback);
    LOG_ERROR_RETURN(status, status);

    return WINSENS_OK;
}

static void window_state_callback(winsens_event_t evt)
{
    LOG_DEBUG("window %u evt %u", evt.data, evt.eventId);

    if (WINDOW_STATE_EVT_OPEN == evt.id)
    {
        // todo emit alarm signal
    }
}

static void acc_evt_handler(winsens_event_t evt)
{
    if (evt.id == ACC_EVT_HIPASS_INT)
    {
        if (!g_windows_enabled)
        {
            for (window_id_t win_id = 0; win_id < WINSENS_WINDOWS_NUM; ++win_id)
            {
                window_status_enable(g_windows[win_id]);
            }

            g_windows_enabled = true;
            timer_start(&g_timer, WINSENS_SCAN_DURATION, false);
        }
        else
        {
            timer_restart(&g_timer, WINSENS_SCAN_DURATION);
        }
    }
}

static void tmr_evt_handler(winsens_event_t evt)
{
    if (TIMER_EVT_SIGNAL ==  evt.id)
    {
        for (window_id_t win_id = 0; win_id < WINSENS_WINDOWS_NUM; ++win_id)
        {
            window_status_disable(g_windows[win_id]);
        }
    }
}
