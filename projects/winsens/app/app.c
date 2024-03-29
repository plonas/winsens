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
#include "ble_peripheral_ctrl.h"
#include "acc_ctrl.h"
#include "battery_observer.h"
#include "ble/window_state_observer.h"
#include "distance_observer.h"
#include "pwr_mgr.h"
#include "timer.h"
#include "cli.h"
#define ILOG_MODULE_NAME winsens_app
#include "log.h"
#include "log_internal_nrf52.h"

#include "boards.h"
#include "app_timer.h"
#include "nrf_delay.h"


LOG_REGISTER();
APP_TIMER_DEF(blink_timer);


// static void timer_callback(void *p_data, uint16_t data_size)
// {
//     bsp_board_led_invert(0);
// }

static void timer_isr(void* context)
{
    // task_queue_add(NULL, 0, timer_callback);
   bsp_board_led_invert( 0 );
}

static void event_handler(winsens_event_t event)
{
    if (PWR_MGR_EVT_PREPARE_TO_SLEEP == event.id ||
        PWR_MGR_EVT_PREPARE_TO_SHUTDOWN == event.id)
    {
        // app_timer_stop(blink_timer);
        // bsp_board_led_off(0);
    }
}


void app_init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    LOG_INIT(NULL);

    timer_init();

    status = cli_init();
    LOG_ERROR_RETURN(status, ;);

    status = system_init();
    LOG_ERROR_RETURN(status, ;);

    status = task_queue_init();
    LOG_ERROR_RETURN(status, ;);

    status = winsens_init();
    LOG_ERROR_RETURN(status, ;);

    status = ble_peripheral_ctrl_init();
    LOG_ERROR_RETURN(status, ;);

    status = acc_ctrl_init();
    LOG_ERROR_RETURN(status, ;);

    status = window_state_observer_ble_init();
    LOG_ERROR_RETURN(status, ;);

    status = battery_observer_init();
    LOG_ERROR_RETURN(status, ;);

    status = distance_observer_init();
    LOG_ERROR_RETURN(status, ;);

    bsp_board_init(BSP_INIT_LEDS);

    ret_code_t err_code;
    err_code = app_timer_create(&blink_timer, APP_TIMER_MODE_REPEATED, timer_isr);
    LOG_WARNING_CHECK(err_code);

    err_code = app_timer_start(blink_timer, APP_TIMER_TICKS(500), NULL);
    LOG_WARNING_CHECK(err_code);

    err_code = winsens_subscribe(pwr_mgr_callback);
    LOG_WARNING_CHECK(err_code);

    status = pwr_mgr_subscribe(event_handler);
    LOG_ERROR_RETURN(status, ;);
}

void app_run(void)
{
    while (true)
    {
        task_queue_execute();
        // LOG_FLUSH();
    };
}
