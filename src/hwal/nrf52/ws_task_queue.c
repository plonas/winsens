/*
 * ws_task_queue.c
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#include "hwal/ws_task_queue.h"
#include "task_queue_config.h"

#include "app_scheduler.h"

#include "nrf_soc.h"



WINSENS_Status_e WS_TaskQueueInit(void)
{
    APP_SCHED_INIT(WS_TQ_MAX_EVENT_DATA_SIZE, WS_TQ_QUEUE_SIZE);
    return WINSENS_OK;
}

WINSENS_Status_e WS_TaskQueueAdd(
    void *p_data,
    uint16_t data_size,
    WS_TaskFunction_f function)
{
    uint32_t res = app_sched_event_put(p_data, data_size, function);

    return (NRF_SUCCESS == res) ? WINSENS_OK : WINSENS_ERROR;
}

void WS_TaskQueueExecute(void)
{
    app_sched_execute();
    sd_app_evt_wait();
}
