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

#define ADC_CFG_CHANNEL_CONF(ain, ms, ix)   { NRFX_SAADC_DEFAULT_CHANNEL_SE(ain, ix), { {0} }, ms, NULL, (1 << ix) }

#define ADC_CFG_CHANNEL_INIT { \
    ADC_CFG_CHANNEL_CONF(NRF_SAADC_INPUT_AIN7, 1000, 0), \
    ADC_CFG_CHANNEL_CONF(NRF_SAADC_INPUT_AIN6, 1000, 1), \
}

#endif /* ADC_CFG_H_ */
