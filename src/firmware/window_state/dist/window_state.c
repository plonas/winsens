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
#include <string.h>


#define OPEN_THRESHOLD_DEFAULT      400


/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void distance_callback(
    adc_channel_id_t id,
    int16_t value);

static winsens_status_t start_distance_sensors(void);
static window_id_t get_window_id(const adc_channel_id_t windows_id);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static const adc_channel_id_t   WINDOW_ADC_MAP[WINDOW_STATE_CFG_NUMBER] = WINDOW_STATE_CFG_WINDOW_MAP_INIT;

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
static void distance_callback(
    adc_channel_id_t id,
    int16_t value)
{
    window_state_type_t wStatus = WINDOW_STATE_UNKNOWN;

    if (g_open_closed_threshold < value)
    {
        wStatus = WINDOW_STATE_OPEN;
    }
    else
    {
        wStatus = WINDOW_STATE_CLOSED;
    }

    const window_id_t win_id = get_window_id(id);
    if (wStatus != g_window_state[win_id])
    {
        g_window_state[win_id] = wStatus;
        if (g_callbacks[win_id])
        {
            g_callbacks[win_id](win_id, wStatus);
        }
    }
}

static winsens_status_t start_distance_sensors(void)
{
    winsens_status_t status = WINSENS_ERROR;

    status = distance_enable(ADC_CHANNEL_DISTANCE, distance_callback);
    if (WINSENS_OK != status)
    {
        return status;
    }

//    status = WS_DistanceEnable(WS_ADC_ADAPTER_CHANNEL_2, WS_DistanceCallback);
//    if (WINSENS_OK != status)
//    {
//        WS_DistanceDisable(WS_ADC_ADAPTER_CHANNEL_1);
//        return status;
//    }

    return distance_start();
}

static window_id_t get_window_id(const adc_channel_id_t adc_channel_id)
{
    window_id_t window_id;
    for (window_id = 0; window_id < WINDOW_STATE_CFG_NUMBER; window_id++)
    {
        if (adc_channel_id == WINDOW_ADC_MAP[window_id])
        {
            break;
        }
    }

    UTILS_ASSERT(WINDOW_STATE_CFG_NUMBER == window_id);

    return window_id;
}
