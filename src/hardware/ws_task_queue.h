/*
 * ws_task_queue.h
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#ifndef WS_TASK_QUEUE_H_
#define WS_TASK_QUEUE_H_

#include "winsens_types.h"


typedef void (*WS_TaskFunction_f)(void *p_data, uint16_t data_size);

WINSENS_Status_e WS_TaskQueueInit(void);

WINSENS_Status_e WS_TaskQueueAdd(
    void *p_data,
    uint16_t data_size,
    WS_TaskFunction_f function);

void WS_TaskQueueExecute(void);

#endif /* WS_TASK_QUEUE_H_ */
