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
    ADC_CHANNEL_WIN_DISTANCE,
    ADC_CHANNEL_BATTERY,

    ADC_CHANNELS_NUMBER

} adc_cfg_channel_id_enum_t;

#define ADC_CFG_REFERENCE_VOL       (3600) //mV
#define ADC_CFG_RESOLUTION          (256) //8 bits

#define ADC_CFG_CHANNEL_CONF(ain, ms)   { { {0} }, ms, 0, ain, NULL }

#define ADC_CFG_CHANNEL_INIT { \
    ADC_CFG_CHANNEL_CONF(NRF_SAADC_INPUT_AIN7, 0), \
    ADC_CFG_CHANNEL_CONF(NRF_SAADC_INPUT_AIN5, 0), \
}

#endif /* ADC_CFG_H_ */
