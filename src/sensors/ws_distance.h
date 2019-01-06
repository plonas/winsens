/*
 * ws_distance.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens_types.h"
#include "hwal/ws_adc_adapter.h"

#include "nrf_drv_timer.h"

#ifndef WS_DISTANCE_H_
#define WS_DISTANCE_H_


typedef void (*WS_DistanceCallback_f)(WS_AdcAdapterChannelId_e id, int16_t value);

WINSENS_Status_e WS_DistanceInit(void);

void WS_DistanceDeinit(void);

WINSENS_Status_e WS_DistanceStart(
    WS_AdcAdapterChannelId_e channelId,
    WS_DistanceCallback_f callback);

void WS_DistanceStop(
    WS_AdcAdapterChannelId_e channelId);

#endif /* WS_DISTANCE_H_ */
