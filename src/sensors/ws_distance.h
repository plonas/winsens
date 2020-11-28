/*
 * ws_distance.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens_types.h"
#include "hwal/ws_adc_adapter.h"

#ifndef WS_DISTANCE_H_
#define WS_DISTANCE_H_


typedef void (*WS_DistanceCallback_f)(WS_AdcAdapterChannelId_e id, int16_t value);

WINSENS_Status_e WS_DistanceInit(void);

void WS_DistanceDeinit(void);

WINSENS_Status_e WS_DistanceEnable(
    WS_AdcAdapterChannelId_e channelId,
    WS_DistanceCallback_f callback);

void WS_DistanceDisable(
    WS_AdcAdapterChannelId_e channelId);

WINSENS_Status_e WS_DistanceStart(void);

void WS_DistanceStop(void);

#endif /* WS_DISTANCE_H_ */
