/*
 * ws_distance.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens_types.h"

#include "nrf_drv_timer.h"

#ifndef WS_DISTANCE_H_
#define WS_DISTANCE_H_

typedef void (*WS_DistanceCallback_f)(int16_t value);

WINSENS_Status_e WS_DistanceInit(
    WS_DistanceCallback_f callback,
    const nrf_drv_timer_t* timer);

void WS_DistanceDeinit();

WINSENS_Status_e WS_DistanceStart();

void WS_DistanceStop();

#endif /* WS_DISTANCE_H_ */
