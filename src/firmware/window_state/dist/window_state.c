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
static winsens_status_t start_distance_sensors(void);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_NUMBER];
static window_state_callback_t  g_callbacks[WINDOW_STATE_CFG_NUMBER] = {NULL};
static uint16_t                 g_open_closed_threshold = 0;

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

        memset(g_window_state, 0, sizeof(bool) * WINDOW_STATE_CFG_NUMBER);
        memset(g_callbacks, 0, sizeof(window_state_callback_t) * WINDOW_STATE_CFG_NUMBER);

        status = start_distance_sensors();
    }

    return status;
}

winsens_status_t window_state_subscribe(
    window_id_t windowsId,
    window_state_callback_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    if (windowsId >= WINDOW_STATE_CFG_NUMBER)
    {
        return WINSENS_ERROR;
    }

    if (g_callbacks[windowsId])
    {
        return WINSENS_NO_RESOURCES;
    }

    g_callbacks[windowsId] = callback;

    return WINSENS_OK;
}

void window_state_unsubscribe(
    window_id_t windowsId,
    window_state_callback_t callback)
{
    if (!g_initialized)
    {
        return;
    }

    (void) callback;

    if (windowsId >= WINDOW_STATE_CFG_NUMBER)
    {
        return;
    }

    g_callbacks[windowsId] = NULL;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void distance_callback(adc_channel_id_t id, int16_t value)
{
    LOG_WARNING_BOOL_RETURN(WINDOW_STATE_CFG_NUMBER > id, ;);

    window_state_type_t win_status = WINDOW_STATE_UNKNOWN;

    if (g_open_closed_threshold < value)
    {
        win_status = WINDOW_STATE_OPEN;
    }
    else
    {
        win_status = WINDOW_STATE_CLOSED;
    }

    LOG_DEBUG("dist %u", value);

    if (win_status != g_window_state[id])
    {
        g_window_state[id] = win_status;
        if (g_callbacks[id])
        {
            g_callbacks[id](id, win_status);
        }
    }
}

static winsens_status_t start_distance_sensors(void)
{
    winsens_status_t status = WINSENS_ERROR;

    status = distance_enable(ADC_CHANNEL_DISTANCE, distance_callback);

    return status;
}
