/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "ws_broker_bt.h"
//#include "ws_broker_stub.h"

#include "nrf_delay.h"
#include "app_error.h"
#include "app_scheduler.h"
#include "app_timer_appsh.h"
#include "hwal/ws_adc_adapter.h"
#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

// Scheduler settings
#define SCHED_MAX_EVENT_DATA_SIZE   MAX(WS_ADC_ADAPTER_SCHED_EVT_SIZE, APP_TIMER_SCHED_EVT_SIZE) // todo adjust that size
#define SCHED_QUEUE_SIZE            10


int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t err_code;
    WS_Broker_t broker;

    APP_SCHED_INIT(SCHED_MAX_EVENT_DATA_SIZE, SCHED_QUEUE_SIZE);
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("main in\n");

    WS_BrokerBtInit(&broker); //todo handle return value
//    WS_BrokerStubInit(&broker); //todo handle return value
    status = WINSENS_Init(&broker);
    if (WINSENS_OK != status) return -1;

    while (true)
    {
        app_sched_execute();
        NRF_LOG_FLUSH();
    };

    WINSENS_Deinit();
    WS_BrokerBtDeinit(&broker);
//    WS_BrokerStubDeinit(&broker);

    NRF_LOG_INFO("main out\n");
    return 0;
}
