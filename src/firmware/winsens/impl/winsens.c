/*
 * winsens.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "acc.h"
#include "adc.h"
#include "distance.h"
#include "hmi.h"
#include "subscribers.h"
#include "timer.h"
#include "winsens.h"
#include "winsens_cfg.h"
#define ILOG_MODULE_NAME WNSN
#include "log.h"

#define WINSENS_SENSORS_NUM     ( sizeof(g_dist_sensors)/sizeof(adc_channel_id_t) )
#define WINSENS_SCAN_DURATION   (10100) // ms


LOG_REGISTER();


static void acc_evt_handler(winsens_event_t evt);
static void tmr_evt_handler(winsens_event_t evt);


static distance_sensor_id_t     g_dist_sensors[]    = WINSENS_CFG_SENSORS_INIT;
static bool                     g_sensors_enabled   = false;
static timer_ws_t               g_timer;
static subscribers_t            g_subscribers;
static winsens_event_handler_t  g_evt_handlers[WINSENS_CFG_SUBSCRIBERS_NUM];


winsens_status_t winsens_init(void)
{
    winsens_status_t status = WINSENS_ERROR;

    status = acc_init();
    LOG_ERROR_RETURN(status, status);

    status = distance_init();
    LOG_ERROR_RETURN(status, status);

    status = hmi_init();
    LOG_ERROR_RETURN(status, status);

    status = timer_init();
    LOG_ERROR_RETURN(status, status);

    status = timer_create(&g_timer, tmr_evt_handler, NULL);
    LOG_ERROR_RETURN(status, status);

    status = acc_subscribe(acc_evt_handler);
    LOG_ERROR_RETURN(status, status);

    status = subscribers_init(&g_subscribers, g_evt_handlers, WINSENS_CFG_SUBSCRIBERS_NUM);
    LOG_ERROR_RETURN(status, status);

    g_sensors_enabled = true;
    timer_start(&g_timer, WINSENS_SCAN_DURATION, false); // disable scan after some time

    return WINSENS_OK;
}

winsens_status_t winsens_subscribe(winsens_event_handler_t evt_handler)
{
    return subscribers_add(&g_subscribers, evt_handler);
}

static void acc_evt_handler(winsens_event_t evt)
{
    if (evt.id == ACC_EVT_HIPASS_INT)
    {
        if (!g_sensors_enabled)
        {
            for (distance_sensor_id_t sensor_id = 0; sensor_id < WINSENS_SENSORS_NUM; ++sensor_id)
            {
                distance_enable(g_dist_sensors[sensor_id]);
            }

            const winsens_event_t e = { .id = WINSENS_EVT_WORKING, .data = 0 };
            subscribers_update(&g_subscribers, e);
            g_sensors_enabled = true;
            timer_start(&g_timer, WINSENS_SCAN_DURATION, false);
        }
        else
        {
            timer_restart(&g_timer, WINSENS_SCAN_DURATION);
        }
    }
}

static void tmr_evt_handler(winsens_event_t evt)
{
    if (TIMER_EVT_SIGNAL ==  evt.id)
    {
        for (distance_sensor_id_t sensor_id = 0; sensor_id < WINSENS_SENSORS_NUM; ++sensor_id)
        {
            distance_disable(g_dist_sensors[sensor_id]);
        }

        const winsens_event_t e = { .id = WINSENS_EVT_IDLE, .data = 0 };
        subscribers_update(&g_subscribers, e);
        g_sensors_enabled = false;
    }
}
