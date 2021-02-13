/*
 * adc_cfg.h
 *
 *  Created on: 13.02.2021
 *      Author: Damian Plonek
 */

#ifndef ADC_CFG_H_
#define ADC_CFG_H_

typedef enum
{
    ADC_CHANNEL_DISTANCE,
    ADC_CHANNEL_BATTERY,

    ADC_CHANNELS_NUMBER

} adc_cfg_channel_id_enum_t;

#define ADC_CFG_CHANNEL_INIT { \
    NRF_SAADC_INPUT_AIN3, \
    NRF_SAADC_INPUT_AIN6, \
    }

#endif /* ADC_CFG_H_ */
