/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "server.h"
#include "system.h"
#include "task_queue.h"
#include "config.h"
#define ILOG_MODULE_NAME MAIN
#include "log.h"
#include "log_internal_nrf52.h"

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
    task_queue_add(NULL, 0, WS_TimerCallback);
//    bsp_board_led_invert( 0 );
}


int main(void)
{
    winsens_status_t status = WINSENS_ERROR;
    ret_code_t err_code;

    LOG_INIT(NULL);

    status = system_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = task_queue_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = config_init(); //todo handle return value
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    server_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);
    LOG_FLUSH();

    status = IWinsens_Init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    bsp_board_init(BSP_INIT_LEDS);

    err_code = app_timer_create(&ws_timer, APP_TIMER_MODE_REPEATED, WS_TimerIrqHandler);
    LOG_WARNING_CHECK(err_code);

    err_code = app_timer_start(ws_timer, APP_TIMER_TICKS(500), NULL);
    LOG_WARNING_CHECK(err_code);

    while (true)
    {
        task_queue_execute();
        LOG_FLUSH();
    };

    return 0;
}
