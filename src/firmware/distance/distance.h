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


typedef enum
{
    DISTANCE_EVT_NEW_DATA = (WINSENS_IDS_MODULE_DISTANCE << 16),
} distance_evt_t;


// todo make the interface not dependent on ADC module

winsens_status_t distance_init(void);

winsens_status_t distance_subscribe(winsens_event_handler_t callback);

winsens_status_t distance_get(adc_channel_id_t ch, int16_t* value);

winsens_status_t distance_enable(adc_channel_id_t ch);
void distance_disable(adc_channel_id_t ch);

#endif /* DISTANCE_H_ */
