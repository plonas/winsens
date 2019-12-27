/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"
//#include "ws_server_stub.h"
#include "ws_server_bt.h"
#include "hwal/ws_system.h"

#include "app_error.h"
#include "app_scheduler.h"
#include "hwal/ws_task_queue.h"
#include "ws_configuration.h"
#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t err_code;
    WS_Server_t server;
    const WS_Configuration_t *config = NULL;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_INFO("main in\n");

    status = WS_SystemInit();
    if (WINSENS_OK != status) return -1;
    NRF_LOG_DEBUG("WS_SystemInit: %u, \n", status);

    status = WS_TaskQueueInit();
    if (WINSENS_OK != status) return -1;

    status = WS_ConfigurationInit(); //todo handle return value
    NRF_LOG_DEBUG("WS_ConfigurationInit: %u, \n", status);

    config = WS_ConfigurationGet();
    NRF_LOG_INFO("enabled: %u, enabled: %u\n", config->windowEnabled[0], config->windowEnabled[1]);
    NRF_LOG_FLUSH();

//    WS_ServerStubInit(&server); //todo handle return value
    WS_ServerBtInit(&server, config); //todo handle return value
    NRF_LOG_FLUSH();

    status = WINSENS_Init(&server, config);
    NRF_LOG_FLUSH();
    if (WINSENS_OK != status) return -1;

    while (true)
    {
        WS_TaskQueueExecute();
        NRF_LOG_FLUSH();
    };

    WINSENS_Deinit();
    server.deinit(&server);

    NRF_LOG_INFO("main out\n");
    return 0;
}
