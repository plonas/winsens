/*
 * ws_window_state.c
 *
 *  Created on: 16.10.2018
 *      Author: Damian.Plonek
 */

#include "ws_window_state.h"
#include "sensors/ws_distance.h"
#include <string.h>

static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value);


static WINSENS_Status_e WS_StartDistanceSensors(void);
static void WS_StopDistanceSensors(void);

static const WS_AdcAdapterChannelId_e WS_WINDOW_ADC_CHANNEL_MAP[WS_WINDOWS_NUMBER] = {
    WS_ADC_ADAPTER_CHANNEL_1
};
static const WS_AdcAdapterChannelId_e WS_ADC_CHANNEL_WINDOW_MAP[WS_ADC_ADAPTER_CHANNELS_NUMBER] = {
    WS_WINDOW_1
};
static WS_WindowState_e ws_windowState[WS_WINDOWS_NUMBER];
static WS_WindowStateCallback_f ws_callback = NULL;

WINSENS_Status_e WS_WindowStateInit(
    WS_WindowStateCallback_f callback)
{
    WINSENS_Status_e status = WS_DistanceInit();

    if (WINSENS_OK != status)
    {
        return status;
    }

    ws_callback = callback;
    memset(ws_windowState, 0, sizeof(bool) * WS_WINDOWS_NUMBER);

    return WS_StartDistanceSensors();
}

void WS_WindowStateDeinit(void)
{
    WS_StopDistanceSensors();
    WS_DistanceDeinit();
}

WS_WindowState_e WS_WindowStateGet(
    WS_Window_e windowId)
{
    return ws_windowState[WS_WINDOW_ADC_CHANNEL_MAP[windowId]];
}

static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value)
{
    WS_WindowState_e wStatus = WS_WINDOW_STATE_UNKNOWN;

    if (400 < value)
    {
        wStatus = WS_WINDOW_STATE_OPEN;
    }
    else
    {
        wStatus = WS_WINDOW_STATE_CLOSED;
    }

    if (wStatus != ws_windowState[WS_ADC_CHANNEL_WINDOW_MAP[id]])
    {
        ws_windowState[WS_ADC_CHANNEL_WINDOW_MAP[id]] = wStatus;
        ws_callback(WS_ADC_CHANNEL_WINDOW_MAP[id], wStatus);
    }
}

static WINSENS_Status_e WS_StartDistanceSensors(void)
{
    return WS_DistanceStart(WS_ADC_ADAPTER_CHANNEL_1, WS_DistanceCallback);
}

static void WS_StopDistanceSensors(void)
{
    WS_DistanceStop(WS_ADC_ADAPTER_CHANNEL_1);
}
