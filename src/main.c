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
#define WS_LOG_MODULE_NAME MAIN
#include "ws_log.h"
#include "ws_log_nrf.h"

#include "boards.h"
#include "app_timer.h"
#include "nrf_delay.h"


APP_TIMER_DEF(ws_timer);

static void WS_TimerCallback(
    void *p_data,
    uint16_t data_size)
{
    bsp_board_led_invert( 0 );
}

static void WS_TimerIrqHandler(
    void* context)
{
    WS_TaskQueueAdd(NULL, 0, WS_TimerCallback);
//    bsp_board_led_invert( 0 );
}


int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    ret_code_t err_code;
    WS_Server_t server;
    const WS_Configuration_t *config = NULL;

    WS_LOG_INIT(NULL);

    status = WS_SystemInit();
    WS_ERROR_CHECK(status, WINSENS_ERROR);

    status = WS_TaskQueueInit();
    WS_ERROR_CHECK(status, WINSENS_ERROR);

    status = WS_ConfigurationInit(); //todo handle return value
    WS_ERROR_CHECK(status, WINSENS_ERROR);

    config = WS_ConfigurationGet();
    WS_LOG_INFO("Enabled: %u, enabled: %u", config->windowEnabled[0], config->windowEnabled[1]);

    WS_LOG_FLUSH();
//    WS_ServerStubInit(&server);
    WS_ServerBtInit(&server, config);
    WS_ERROR_CHECK(status, WINSENS_ERROR);
    WS_LOG_FLUSH();

    status = WINSENS_Init(&server, config);
    WS_ERROR_CHECK(status, WINSENS_ERROR);

    bsp_board_init(BSP_INIT_LEDS);

    err_code = app_timer_create(&ws_timer, APP_TIMER_MODE_REPEATED, WS_TimerIrqHandler);
    WS_LOG_NRF_WARNING_CHECK(err_code);

    err_code = app_timer_start(ws_timer, APP_TIMER_TICKS(500), NULL);
    WS_LOG_NRF_WARNING_CHECK(err_code);

    while (true)
    {
        WS_TaskQueueExecute();
        WS_LOG_FLUSH();
    };

    WINSENS_Deinit();
    server.deinit(&server);

    WS_LOG_INFO("main out");
    return 0;
}
