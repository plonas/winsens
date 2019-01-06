/*
 * ws_adc_adapter.h
 *
 *  Created on: 18 paź 2018
 *      Author: damian
 */

#ifndef WS_ADC_ADAPTER_H_
#define WS_ADC_ADAPTER_H_

#include "winsens_types.h"


struct WS_AdcAdapterChannel;

typedef enum
{
    WS_ADC_ADAPTER_CHANNEL_1,
    WS_ADC_ADAPTER_CHANNEL_2,

    WS_ADC_ADAPTER_CHANNELS_NUMBER

} WS_AdcAdapterChannelId_e;

typedef void (*WS_AdcAdapterCallback_f)(WS_AdcAdapterChannelId_e id, int16_t value);

WINSENS_Status_e WS_AdcAdapterInit(void);

void WS_AdcAdapterDeinit(void);

WINSENS_Status_e WS_AdcAdapterEnableChannel(
    WS_AdcAdapterChannelId_e channelId,
    WS_AdcAdapterCallback_f callback);

void WS_AdcAdapterDisableChannel(
    WS_AdcAdapterChannelId_e channelId);

#endif /* WS_ADC_ADAPTER_H_ */
