/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"
#include "system.h"
#include "task_queue.h"
#include "config.h"
#define ILOG_MODULE_NAME MAIN
#include "log.h"
#include "log_internal_nrf52.h"
#include "acc.h"
#include "acc_ctrl.h"

#include "boards.h"
#include "app_timer.h"
#include "nrf_delay.h"


LOG_REGISTER();
APP_TIMER_DEF(blink_timer);


static void WS_TimerCallback(void *p_data, uint16_t data_size)
{
    bsp_board_led_invert( 0 );
}

static void WS_TimerIrqHandler(void* context)
{
    task_queue_add(NULL, 0, WS_TimerCallback);
//    bsp_board_led_invert( 0 );
}

static void acc_task(void *p_data, uint16_t data_size)
{
    while (0 != acc_get_data_len())
    {
        acc_t acc_data;
        winsens_status_t status = acc_get_data(&acc_data, 1);
        LOG_WARNING_RETURN(status, );

//        LOG_DEBUG("acc: \nx: %6.3f\ny: %6.3f\nz: %6.3f", g(acc_data.x), g(acc_data.y), g(acc_data.z));
    }
}

void acc_event_handler(winsens_event_t event)
{
    if (ACC_EVT_NEW_DATA == event.id)
    {
        task_queue_add(NULL, 0, acc_task);
    }
}


int main(void)
{
    winsens_status_t status = WINSENS_ERROR;
    ret_code_t err_code;

    NRF_WDT->CONFIG &= 0x00000001;

    LOG_INIT(NULL);

    status = system_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = task_queue_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = config_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    LOG_ERROR_RETURN(status, WINSENS_ERROR);
    LOG_FLUSH();

    status = IWinsens_Init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = acc_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    status = acc_ctrl_init();
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    bsp_board_init(BSP_INIT_LEDS);

    err_code = app_timer_create(&blink_timer, APP_TIMER_MODE_REPEATED, WS_TimerIrqHandler);
    LOG_WARNING_CHECK(err_code);

    err_code = app_timer_start(blink_timer, APP_TIMER_TICKS(500), NULL);
    LOG_WARNING_CHECK(err_code);

    status = acc_subscribe(acc_event_handler);
    LOG_ERROR_RETURN(status, WINSENS_ERROR);

    while (true)
    {
        task_queue_execute();
        LOG_FLUSH();
    };

    return 0;
}
