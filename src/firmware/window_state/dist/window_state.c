/*
 * window_state.c
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#include "window_state.h"
#include "window_state_cfg.h"
#include "distance.h"
#include "adc_cfg.h"
#include "utils.h"
#include "subscribers.h"
#define ILOG_MODULE_NAME WS
#include "log.h"
#include <string.h>


#define OPEN_THRESHOLD_DEFAULT      50


LOG_REGISTER();


/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void distance_callback(winsens_event_t evt);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_WINDOW_NUMBER];
static winsens_event_handler_t  g_callbacks[WINDOW_STATE_CFG_SUBSCRIBERS_NUM] = { NULL };
static window_state_threshold_t g_open_closed_thresholds[WINDOW_STATE_CFG_WINDOW_NUMBER] = WINDOW_STATE_CFG_THRESHOLD_INIT;
static subscribers_t            g_subscribers;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t window_state_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;

        status = distance_init();

        if (WINSENS_OK != status)
        {
            return status;
        }

        distance_subscribe(distance_callback);

        memset(g_window_state, WINDOW_STATE_UNKNOWN, sizeof(window_state_type_t) * WINDOW_STATE_CFG_WINDOW_NUMBER);

        status = subscribers_init(&g_subscribers, g_callbacks, WINDOW_STATE_CFG_SUBSCRIBERS_NUM);

        for (window_id_t id = 0; id < WINDOW_STATE_CFG_WINDOW_NUMBER; ++id)
        {
            distance_enable(id);
        }
    }

    return status;
}

winsens_status_t window_state_subscribe(winsens_event_handler_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return subscribers_add(&g_subscribers, callback);;
}

window_state_type_t window_state_get(window_id_t window)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > window, WINDOW_STATE_UNKNOWN);

    return g_window_state[window];
}

uint8_t window_state_get_windows_number(void)
{
    return WINDOW_STATE_CFG_WINDOW_NUMBER;
}

winsens_status_t window_state_threshold_get(window_id_t window, window_state_threshold_t* th)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(NULL != th, WINSENS_INVALID_PARAMS);
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > window, WINSENS_INVALID_PARAMS);

    *th = g_open_closed_thresholds[window];
    return WINSENS_OK;
}

winsens_status_t window_state_threshold_set(window_id_t window, window_state_threshold_t th)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > window, WINSENS_INVALID_PARAMS);

    g_open_closed_thresholds[window] = th;

    winsens_event_t new_evt = { .id = WINDOW_STATE_EVT_THRESHOLD_UPDATE, .data = th };
    subscribers_update(&g_subscribers, new_evt);

    return WINSENS_OK;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void distance_callback(winsens_event_t evt)
{
    const window_id_t id = (window_id_t)evt.data;
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_WINDOW_NUMBER > id, ;);
    winsens_event_t new_evt;
    window_state_threshold_t value = 0;

    distance_get((adc_channel_id_t)id, &value);

    if (g_open_closed_thresholds[id] < value)
    {
        g_window_state[id] = WINDOW_STATE_OPEN;
        new_evt.id = WINDOW_STATE_EVT_OPEN;
    }
    else
    {
        g_window_state[id] = WINDOW_STATE_CLOSED;
        new_evt.id = WINDOW_STATE_EVT_CLOSED;
    }
    new_evt.data = id;
    subscribers_update(&g_subscribers, new_evt);

    LOG_DEBUG("dist %d", value);
}
