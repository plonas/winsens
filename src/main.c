/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "ws_server_bt.h"
#include "ws_system.h"
#include "ws_task_queue.h"
#include "ws_configuration.h"
#define WS_LOG_MODULE_NAME "MAIN"
#include "ws_log.h"

int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t err_code;
    WS_Server_t server;
    const WS_Configuration_t *config = NULL;

    err_code = WS_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    WS_LOG_DEBUG("main in\r\n");

    status = WS_SystemInit();
    if (WINSENS_OK != status) return -1;
    WS_LOG_DEBUG("WS_SystemInit: %u, \r\n", status);

    status = WS_TaskQueueInit();
    if (WINSENS_OK != status) return -1;

    status = WS_ConfigurationInit(); //todo handle return value
    WS_LOG_DEBUG("WS_ConfigurationInit: %u, \r\n", status);

    config = WS_ConfigurationGet();
    WS_LOG_INFO("enabled: %u, enabled: %u\r\n", config->windowEnabled[0], config->windowEnabled[1]);
    WS_LOG_FLUSH();

//    WS_ServerStubInit(&server); //todo handle return value
    WS_ServerBtInit(&server, config); //todo handle return value
    WS_LOG_FLUSH();

    status = WINSENS_Init(&server, config);
    WS_LOG_FLUSH();
    if (WINSENS_OK != status) return -1;

    while (true)
    {
        WS_TaskQueueExecute();
        WS_LOG_FLUSH();
    };

    WINSENS_Deinit();
    server.deinit(&server);

    WS_LOG_INFO("main out\r\n");
    return 0;
}
