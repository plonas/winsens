/*
 * window_state_cfg.h
 *
 *  Created on: 10.01.2021
 *      Author: Damian.Plonek
 */

#ifndef WINDOW_STATE_CFG_H_
#define WINDOW_STATE_CFG_H_


#include "adc_cfg.h"


#define WINDOW_STATE_CFG_SUBSCRIBERS_NUM    (2)


typedef enum
{
    WINDOW_STATE_CFG_WINDOW = ADC_CHANNEL_WIN_DISTANCE,

    WINDOW_STATE_CFG_WINDOW_NUMBER

} window_id_enum_t;


#endif /* WINDOW_STATE_CFG_H_ */
