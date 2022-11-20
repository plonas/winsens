/*
 * task_queue.c
 *
 *  Created on: 28.03.2019
 *      Author: Damian Plonek
 */

#include "task_queue.h"
#include "task_queue_cfg.h"
#include "cli.h"
#define ILOG_MODULE_NAME TASK
#include "log.h"
#include "app_scheduler.h"

#include "nrf_soc.h"


LOG_REGISTER();

static bool g_initialized = false;


winsens_status_t task_queue_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        APP_SCHED_INIT(TASK_QUEUE_CFG_MAX_EVENT_DATA_SIZE, TASK_QUEUE_CFG_QUEUE_SIZE);
        cli_init();
    }

    return WINSENS_OK;
}

winsens_status_t task_queue_add(
    void *p_data,
    uint16_t data_size,
    task_function_t function)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    uint32_t res = app_sched_event_put(p_data, data_size, function);

    return (NRF_SUCCESS == res) ? WINSENS_OK : WINSENS_ERROR;
}

void task_queue_execute(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    app_sched_execute();
    cli_process();
    LOG_FLUSH();
    sd_app_evt_wait();
}
