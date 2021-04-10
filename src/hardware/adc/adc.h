/*
 * adc.h
 *
 *  Created on: 18 paź 2018
 *      Author: damian
 */

#ifndef ADC_H_
#define ADC_H_

#include "winsens_types.h"


typedef uint8_t adc_channel_id_t;
typedef void (*adc_callback_t)(adc_channel_id_t id, int16_t value);

typedef struct
{
    adc_channel_id_t id;
    int16_t value;
} adc_event_t;
#define ADC_SCHED_EVT_SIZE          sizeof(adc_event_t)

winsens_status_t adc_init(void);

winsens_status_t adc_enable_channel(adc_channel_id_t channelId, adc_callback_t callback);

void adc_disable_channel(adc_channel_id_t channelId);

winsens_status_t adc_start(void);

void adc_stop(void);

#endif /* ADC_H_ */
