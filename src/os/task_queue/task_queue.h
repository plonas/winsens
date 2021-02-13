/*
 * task_queue.h
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#ifndef TASK_QUEUE_H_
#define TASK_QUEUE_H_

#include "winsens_types.h"


typedef void (*task_function_t)(void *p_data, uint16_t data_size);

winsens_status_t task_queue_init(void);

winsens_status_t task_queue_add(
    void *p_data,
    uint16_t data_size,
    task_function_t function);

void task_queue_execute(void);

#endif /* TASK_QUEUE_H_ */
