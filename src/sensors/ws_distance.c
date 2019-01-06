/*
 * ws_distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "ws_distance.h"

#include "app_error.h"
#include "utils.h"
#include "ws_adc_adapter.h"


static void WS_AdcEventHandler(
    WS_AdcAdapterChannelId_e id,
    int16_t value);

static WS_DistanceCallback_f callbacks[WS_ADC_ADAPTER_CHANNELS_NUMBER];

WINSENS_Status_e WS_DistanceInit(void)
{
    WINSENS_Status_e status = WS_AdcAdapterInit();
    return status;
}

void WS_DistanceDeinit(void)
{
    WS_AdcAdapterDeinit();
}

WINSENS_Status_e WS_DistanceStart(
    WS_AdcAdapterChannelId_e channelId,
    WS_DistanceCallback_f callback)
{
    WS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId)

    callbacks[channelId] = callback;

    WINSENS_Status_e status = WS_AdcAdapterEnableChannel(channelId, WS_AdcEventHandler);
    if (WINSENS_OK != status)
    {
        callbacks[channelId] = NULL;
    }

    return status;
}

void WS_DistanceStop(
    WS_AdcAdapterChannelId_e channelId)
{
    WS_ASSERT(WS_ADC_ADAPTER_CHANNELS_NUMBER > channelId)

    WS_AdcAdapterDisableChannel(channelId);
    callbacks[channelId] = NULL;
}

static void WS_AdcEventHandler(
    WS_AdcAdapterChannelId_e id,
    int16_t value)
{
    if (NULL != callbacks[id])
    {
        callbacks[id](id, value);
    }
}
