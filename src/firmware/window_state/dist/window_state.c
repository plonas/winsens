/*
 * WindowState.c
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#include "window_state.h"
#include "window_state_cfg.h"
#include "distance.h"
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

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static const adc_channel_id_t WS_ADC_CHANNEL_WINDOW_MAP[ADC_CHANNELS_NUMBER] = {
    WINDOW_STATE_CFG_WINDOW_1,
    WINDOW_STATE_CFG_WINDOW_2,
};
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_WINDOWS_NUMBER];
static window_state_callback_t  g_callbacks[WINDOW_STATE_CFG_WINDOWS_NUMBER] = {NULL};
static uint16_t                 g_open_closed_threshold = 0;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t window_state_init(void)
{
    winsens_status_t status = distance_init();

    if (WINSENS_OK != status)
    {
        return status;
    }

    g_open_closed_threshold = OPEN_THRESHOLD_DEFAULT;

    memset(g_window_state, 0, sizeof(bool) * WINDOW_STATE_CFG_WINDOWS_NUMBER);
    memset(g_callbacks, 0, sizeof(window_state_callback_t) * WINDOW_STATE_CFG_WINDOWS_NUMBER);

    return start_distance_sensors();
}

winsens_status_t window_state_subscribe(
    window_id_t windowsId,
    window_state_callback_t callback)
{
    if (windowsId >= WINDOW_STATE_CFG_WINDOWS_NUMBER)
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
    (void) callback;

    if (windowsId >= WINDOW_STATE_CFG_WINDOWS_NUMBER)
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

    if (wStatus != g_window_state[WS_ADC_CHANNEL_WINDOW_MAP[id]])
    {
        g_window_state[WS_ADC_CHANNEL_WINDOW_MAP[id]] = wStatus;
        if (g_callbacks[WS_ADC_CHANNEL_WINDOW_MAP[id]])
        {
            g_callbacks[WS_ADC_CHANNEL_WINDOW_MAP[id]](WS_ADC_CHANNEL_WINDOW_MAP[id], wStatus);
        }
    }
}

static winsens_status_t start_distance_sensors(void)
{
    winsens_status_t status = WINSENS_ERROR;

    status = distance_enable(ADC_CHANNEL_1, distance_callback);
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
