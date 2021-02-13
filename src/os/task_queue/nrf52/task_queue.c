/*
 * task_queue.c
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#include "task_queue.h"
#include "task_queue_cfg.h"

#include "app_scheduler.h"

#include "nrf_soc.h"



winsens_status_t task_queue_init(void)
{
    APP_SCHED_INIT(TASK_QUEUE_CFG_MAX_EVENT_DATA_SIZE, TASK_QUEUE_CFG_QUEUE_SIZE);
    return WINSENS_OK;
}

winsens_status_t task_queue_add(
    void *p_data,
    uint16_t data_size,
    task_function_t function)
{
    uint32_t res = app_sched_event_put(p_data, data_size, function);

    return (NRF_SUCCESS == res) ? WINSENS_OK : WINSENS_ERROR;
}

void task_queue_execute(void)
{
    app_sched_execute();
    sd_app_evt_wait();
}
