/*
 * app.c
 *
 *  Created on: 02.09.20201
 *      Author: Damian.Plonek
 */

#include "app.h"
#include "winsens.h"
#include "system.h"
#include "task_queue.h"
#include "acc.h"
#include "acc_ctrl.h"
#include "ble/window_state_observer.h"
#include "pwr_mgr.h"
#define ILOG_MODULE_NAME winsens_app
#include "log.h"
#include "log_internal_nrf52.h"

#include "boards.h"
#include "app_timer.h"
#include "nrf_delay.h"


LOG_REGISTER();
APP_TIMER_DEF(blink_timer);


static void timer_callback(void *p_data, uint16_t data_size)
{
    bsp_board_led_invert( 0 );
}

static void timer_isr(void* context)
{
    task_queue_add(NULL, 0, timer_callback);
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


void app_init(void)
{
    winsens_status_t status = WINSENS_ERROR;
    ret_code_t err_code;

    NRF_WDT->CONFIG &= 0x00000001;

    LOG_INIT(NULL);

    status = system_init();
    LOG_ERROR_RETURN(status, ;);

    status = task_queue_init();
    LOG_ERROR_RETURN(status, ;);

    status = winsens_init();
    LOG_ERROR_RETURN(status, ;);

    status = acc_init();
    LOG_ERROR_RETURN(status, ;);

    status = acc_ctrl_init();
    LOG_ERROR_RETURN(status, ;);

    status = window_state_observer_ble_init();
    LOG_ERROR_RETURN(status, ;);

    bsp_board_init(BSP_INIT_LEDS);

    err_code = app_timer_create(&blink_timer, APP_TIMER_MODE_REPEATED, timer_isr);
    LOG_WARNING_CHECK(err_code);

    err_code = app_timer_start(blink_timer, APP_TIMER_TICKS(500), NULL);
    LOG_WARNING_CHECK(err_code);

    err_code = winsens_subscribe(pwr_mgr_callback);
    LOG_WARNING_CHECK(err_code);

    status = acc_subscribe(acc_event_handler);
    LOG_ERROR_RETURN(status, ;);
}

void app_run(void)
{
    while (true)
    {
        task_queue_execute();
        LOG_FLUSH();
    };
}
