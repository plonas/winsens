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
#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t err_code;
    WS_Broker_t broker;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("XXX Start\n");

    WS_BrokerBtInit(&broker); //todo handle return value
//    WS_BrokerStubInit(&broker); //todo handle return value
    status = WINSENS_Init(&broker);
    if (WINSENS_OK != status) return -1;

    status = WINSENS_Loop();

    NRF_LOG_ERROR("WINSENS_Loop failed with %u\n", status);

    WINSENS_Deinit();
    WS_BrokerBtDeinit(&broker);
//    WS_BrokerStubDeinit(&broker);
    return 0;
}
