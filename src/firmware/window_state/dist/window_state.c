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
static void distance_callback(adc_channel_id_t id, int16_t value);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_NUMBER];
static winsens_event_handler_t  g_callbacks[WINDOW_STATE_CFG_SUBSCRIBERS_NUM] = { NULL };
static uint16_t                 g_open_closed_threshold = 0;
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

        g_open_closed_threshold = OPEN_THRESHOLD_DEFAULT;

        memset(g_window_state, WINDOW_STATE_UNKNOWN, sizeof(window_state_type_t) * WINDOW_STATE_CFG_NUMBER);

        status = subscribers_init(&g_subscribers, g_callbacks, WINDOW_STATE_CFG_SUBSCRIBERS_NUM);
    }

    return status;
}

winsens_status_t window_state_subscribe(winsens_event_handler_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    return subscribers_add(&g_subscribers, callback);;
}

winsens_status_t window_status_enable(window_id_t win_id)
{
    return distance_enable(win_id, distance_callback);
}

void window_status_disable(window_id_t win_id)
{
    distance_disable(win_id);
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void distance_callback(adc_channel_id_t id, int16_t value)
{
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_NUMBER > id, ;);

    winsens_event_t evt;

    if (g_open_closed_threshold < value)
    {
        g_window_state[id] = WINDOW_STATE_OPEN;
        evt.id = WINDOW_STATE_EVT_OPEN;
    }
    else
    {
        g_window_state[id] = WINDOW_STATE_CLOSED;
        evt.id = WINDOW_STATE_EVT_CLOSED;
    }
    evt.data = id;
    subscribers_update(&g_subscribers, evt);

    LOG_DEBUG("dist %u", value);
}
