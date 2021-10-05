/*
 * distance_cfg.h
 *
 *  Created on: 05.09.2021
 *      Author: Damian Plonek
 */

#ifndef DISTANCE_CFG_H_
#define DISTANCE_CFG_H_


#include "digital_io_cfg.h"
#include "adc_cfg.h"


#define DISTANCE_CFG_ADC_IDS_INIT           { ADC_CHANNEL_WIN_DISTANCE }
#define DISTANCE_CFG_PROBE_INTERVAL_MS      (1000)

#define DISTANCE_CFG_SENSOR_POWER_PIN       DIGITAL_IO_OUTPUT_DIST_SENS_POWER
#define DISTANCE_CFG_POWER_ON               (bool)true
#define DISTANCE_CFG_POWER_OFF              (bool)false


#endif /* DISTANCE_CFG_H_ */
