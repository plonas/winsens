/*
 * ws_window_state.c
 *
 *  Created on: 16.10.2018
 *      Author: Damian.Plonek
 */

#include "ws_window_state.h"
#include "sensors/ws_distance.h"
#include <string.h>

#define WS_OPEN_CLOSED_THRESHOLD_DEFAULT    400


static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value);


static WINSENS_Status_e WS_StartDistanceSensors(void);
static void WS_StopDistanceSensors(void);

static const WS_AdcAdapterChannelId_e WS_ADC_CHANNEL_WINDOW_MAP[WS_ADC_ADAPTER_CHANNELS_NUMBER] = {
    WS_WINDOW_1
};
static WS_WindowState_e ws_windowState[WS_WINDOWS_NUMBER];
static WS_WindowStateCallback_f ws_callbacks[WS_WINDOWS_NUMBER] = {NULL};
static uint16_t ws_openClosedthreshold = 0;

WINSENS_Status_e WS_WindowStateInit(void)
{
    WINSENS_Status_e status = WS_DistanceInit();

    if (WINSENS_OK != status)
    {
        return status;
    }

    ws_openClosedthreshold = WS_OPEN_CLOSED_THRESHOLD_DEFAULT;

    memset(ws_windowState, 0, sizeof(bool) * WS_WINDOWS_NUMBER);
    memset(ws_callbacks, 0, sizeof(WS_WindowStateCallback_f) * WS_WINDOWS_NUMBER);

    return WS_StartDistanceSensors();
}

void WS_WindowStateDeinit(void)
{
    WS_StopDistanceSensors();
    WS_DistanceDeinit();
}

WS_WindowState_e WS_WindowStateConfigure(
    WS_Window_e windowsId,
    uint16_t openClosedThreshold)
{
    ws_openClosedthreshold = openClosedThreshold;
    return WINSENS_OK;
}

WS_WindowState_e WS_WindowStateSubscribe(
    WS_Window_e windowsId,
    WS_WindowStateCallback_f callback)
{
    if (windowsId >= WS_WINDOWS_NUMBER)
    {
        return WINSENS_ERROR;
    }

    if (ws_callbacks[windowsId])
    {
        return WINSENS_NO_RESOURCES;
    }

    ws_callbacks[windowsId] = callback;

    return WINSENS_OK;
}

void WS_WindowStateUnsubscribe(
    WS_Window_e windowsId,
    WS_WindowStateCallback_f callback)
{
    (void) callback;

    if (windowsId >= WS_WINDOWS_NUMBER)
    {
        return;
    }

    ws_callbacks[windowsId] = NULL;
}

static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value)
{
    WS_WindowState_e wStatus = WS_WINDOW_STATE_UNKNOWN;

    if (ws_openClosedthreshold < value)
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
        if (ws_callbacks[WS_ADC_CHANNEL_WINDOW_MAP[id]])
        {
            ws_callbacks[WS_ADC_CHANNEL_WINDOW_MAP[id]](WS_ADC_CHANNEL_WINDOW_MAP[id], wStatus);
        }
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
