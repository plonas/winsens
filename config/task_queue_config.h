/*
 * task_queue_config.h
 *
 *  Created on: 07.11.2020
 *      Author: Damian Plonek
 */

#ifndef TASK_QUEUE_CONFIG_H_
#define TASK_QUEUE_CONFIG_H_

#include "ws_adc_adapter.h"

#include "app_timer.h"


#define WS_TQ_MAX_EVENT_DATA_SIZE       MAX(WS_ADC_ADAPTER_SCHED_EVT_SIZE, APP_TIMER_SCHED_EVENT_DATA_SIZE) // todo adjust that size
#define WS_TQ_QUEUE_SIZE                100


#endif /* TASK_QUEUE_CONFIG_H_ */
