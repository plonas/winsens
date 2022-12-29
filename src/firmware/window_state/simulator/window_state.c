/*
 * window_state.c
 *
 *  Created on: 27.03.2021
 *      Author: Damian.Plonek
 */

#include "window_state.h"
#include "window_state_cfg.h"
#include "subscribers.h"
#include "timer.h"
#define ILOG_MODULE_NAME WS
#include "log.h"


#define UPDATE_INTERVAL     (1500)

/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
LOG_REGISTER();

static void update_state(void);

static void timer_callback(winsens_event_t event);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_WINDOW_NUMBER] = {WINDOW_STATE_UNKNOWN};
static winsens_event_handler_t  g_callbacks[WINDOW_STATE_CFG_WINDOW_NUMBER] = {NULL};
static subscribers_t            g_subscribers;
static timer_ws_t               g_timer_id;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t window_state_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        for (int i = 0; i < WINDOW_STATE_CFG_WINDOW_NUMBER; ++i)
        {
            g_window_state[i] = (i % 2 == 0) ? WINDOW_STATE_OPEN : WINDOW_STATE_CLOSED;
        }

        timer_init();
        timer_create(&g_timer_id, timer_callback, NULL);
        timer_start(&g_timer_id, UPDATE_INTERVAL, true);

        subscribers_init(&g_subscribers, g_callbacks, WINDOW_STATE_CFG_WINDOW_NUMBER);
    }

    return WINSENS_OK;
}

winsens_status_t window_state_subscribe(winsens_event_handler_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    return subscribers_add(&g_subscribers, callback);
}

window_state_type_t window_state_get(window_id_t window)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > window, WINDOW_STATE_UNKNOWN);

    return g_window_state[window];
}

winsens_status_t window_state_threshold_get(window_id_t window, window_state_threshold_t* th)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t window_state_threshold_set(window_id_t window, window_state_threshold_t th)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > window, WINSENS_INVALID_PARAMS);

    winsens_event_t new_evt = { .id = WINDOW_STATE_EVT_THRESHOLD_UPDATE, .data = th };
    subscribers_update(&g_subscribers, new_evt);

    return WINSENS_OK;
}

uint8_t window_state_get_windows_number(void)
{
    return WINDOW_STATE_CFG_WINDOW_NUMBER;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void update_state(void)
{
    for (int i = 0; i < WINDOW_STATE_CFG_WINDOW_NUMBER; ++i)
    {
        g_window_state[i] = (WINDOW_STATE_OPEN == g_window_state[i]) ? WINDOW_STATE_CLOSED : WINDOW_STATE_OPEN;
        winsens_event_id_t id = (WINDOW_STATE_OPEN == g_window_state[i]) ? WINDOW_STATE_EVT_OPEN : WINDOW_STATE_EVT_CLOSED;
        winsens_event_t new_evt = { .id = id, .data = 0 };
        subscribers_update(&g_subscribers, new_evt);
    }
}

static void timer_callback(winsens_event_t event)
{
    if (TIMER_EVT_SIGNAL == event.id)
    {
        update_state();
    }
}
