/*
 *  task_queue_cfg.h
 *
 *  Created on: 07.11.2020
 *      Author: Damian Plonek
 */

#ifndef TASK_QUEUE_CFG_H_
#define TASK_QUEUE_CFG_H_

#include "adc.h"

#include "app_timer.h"


#define TASK_QUEUE_CFG_MAX_EVENT_DATA_SIZE              MAX(ADC_SCHED_EVT_SIZE, APP_TIMER_SCHED_EVENT_DATA_SIZE) // todo adjust that size
#define TASK_QUEUE_CFG_QUEUE_SIZE                       100


#endif /* TASK_QUEUE_CFG_H_ */
