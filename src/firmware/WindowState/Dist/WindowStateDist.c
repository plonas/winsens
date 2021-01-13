/*
 * WindowState.c
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#include "IWindowState.h"
#include "IWindowStateCfg.h"
#include "WindowStateDist.h"
#include "sensors/ws_distance.h"
#include <string.h>

#ifdef WINSENS_IF_WINDOW_STATE_DIST
#define WindowStateDistInit         IWindowStateInit
#define WindowStateDistSubscribe    IWindowStateSubscribe
#define WindowStateDistUnsubscribe  IWindowStateUnsubscribe
#endif

#define OPEN_THRESHOLD_DEFAULT      400


/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value);

static WINSENS_Status_e WS_StartDistanceSensors(void);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static const WS_AdcAdapterChannelId_e WS_ADC_CHANNEL_WINDOW_MAP[WS_ADC_ADAPTER_CHANNELS_NUMBER] = {
    IWINDOW_STATE_CFG_WINDOW_1,
    IWINDOW_STATE_CFG_WINDOW_2,
};
static IWindowState_e ws_windowState[IWINDOW_STATE_CFG_WINDOWS_NUMBER];
static IWindowStateCallback_f ws_callbacks[IWINDOW_STATE_CFG_WINDOWS_NUMBER] = {NULL};
static uint16_t ws_openClosedthreshold = 0;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
WINSENS_Status_e WindowStateDistInit(void)
{
    WINSENS_Status_e status = WS_DistanceInit();

    if (WINSENS_OK != status)
    {
        return status;
    }

    ws_openClosedthreshold = OPEN_THRESHOLD_DEFAULT;

    memset(ws_windowState, 0, sizeof(bool) * IWINDOW_STATE_CFG_WINDOWS_NUMBER);
    memset(ws_callbacks, 0, sizeof(IWindowStateCallback_f) * IWINDOW_STATE_CFG_WINDOWS_NUMBER);

    return WS_StartDistanceSensors();
}

WINSENS_Status_e WindowStateDistSubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback)
{
    if (windowsId >= IWINDOW_STATE_CFG_WINDOWS_NUMBER)
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

void WindowStateDistUnsubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback)
{
    (void) callback;

    if (windowsId >= IWINDOW_STATE_CFG_WINDOWS_NUMBER)
    {
        return;
    }

    ws_callbacks[windowsId] = NULL;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void WS_DistanceCallback(
    WS_AdcAdapterChannelId_e id,
    int16_t value)
{
    IWindowState_e wStatus = IWINDOW_STATE_UNKNOWN;

    if (ws_openClosedthreshold < value)
    {
        wStatus = IWINDOW_STATE_OPEN;
    }
    else
    {
        wStatus = IWINDOW_STATE_CLOSED;
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
    WINSENS_Status_e status = WINSENS_ERROR;

    status = WS_DistanceEnable(WS_ADC_ADAPTER_CHANNEL_1, WS_DistanceCallback);
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

    return WS_DistanceStart();
}
