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
#define WS_TQ_MAX_EVENT_DATA_SIZE       MAX(WS_ADC_ADAPTER_SCHED_EVT_SIZE, APP_TIMER_SCHED_EVT_SIZE) // todo adjust that size
#define WS_TQ_QUEUE_SIZE                10

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
}
