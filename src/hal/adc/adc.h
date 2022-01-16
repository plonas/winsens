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

winsens_status_t adc_start(adc_channel_id_t channel_id, adc_callback_t callback);

void adc_stop(adc_channel_id_t channel_id);

void adc_probe(const adc_channel_id_t *channel_ids, uint16_t len);

int16_t adc_get_voltage(int16_t value);

#endif /* ADC_H_ */
