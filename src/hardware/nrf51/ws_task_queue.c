/*
 * ws_task_queue.c
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#include "hwal/ws_task_queue.h"
#include "app_scheduler.h"

#include "app_timer_appsh.h"
#include "hwal/ws_adc_adapter.h"

// Scheduler settings
#define TASK_QUEUE_CFG_MAX_EVENT_DATA_SIZE       MAX(WS_ADC_ADAPTER_SCHED_EVT_SIZE, APP_TIMER_SCHED_EVT_SIZE) // todo adjust that size
#define TASK_QUEUE_CFG_QUEUE_SIZE                10

winsens_status_t WS_TaskQueueInit(void)
{
    APP_SCHED_INIT(TASK_QUEUE_CFG_MAX_EVENT_DATA_SIZE, TASK_QUEUE_CFG_QUEUE_SIZE);
    return WINSENS_OK;
}

winsens_status_t WS_TaskQueueAdd(
    void *p_data,
    uint16_t data_size,
    task_function_t function)
{
    uint32_t res = app_sched_event_put(p_data, data_size, function);

    return (NRF_SUCCESS == res) ? WINSENS_OK : WINSENS_ERROR;
}

void WS_TaskQueueExecute(void)
{
    app_sched_execute();
}
