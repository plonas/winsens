/*
 * distance.h
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "winsens_types.h"
#include "adc.h"

#ifndef DISTANCE_H_
#define DISTANCE_H_


// todo make the interface not dependent on ADC module
typedef void (*distance_callback_t)(adc_channel_id_t id, int16_t value);

winsens_status_t distance_init(void);

winsens_status_t distance_enable(adc_channel_id_t channelId, distance_callback_t callback);

void distance_disable(adc_channel_id_t channelId);

#endif /* DISTANCE_H_ */
