/*
 * distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "distance.h"
#include "distance_cfg.h"
#include "digital_io.h"
#include "app_error.h"
#include "utils/utils.h"
#include "adc.h"
#include "subscribers.h"
#include "timer.h"
#define ILOG_MODULE_NAME distance
#include "log.h"


#define DISTANCE_ADC_IDS_NUM        ( sizeof(g_adc_ids)/sizeof(g_adc_ids[0]) )


static void adc_evt_handler(adc_channel_id_t id, int16_t value);
static void tmr_evt_handler(winsens_event_t event);

static bool g_initialized = false;
static adc_channel_id_t g_adc_ids[] = DISTANCE_CFG_ADC_IDS_INIT;
static winsens_event_handler_t g_callbacks[DISTANCE_ADC_IDS_NUM];
static subscribers_t g_subscribers;
static int16_t g_values[DISTANCE_ADC_IDS_NUM] = {0};
static timer_ws_t g_timer;


LOG_REGISTER();


winsens_status_t distance_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;

        status = digital_io_init();
        LOG_ERROR_RETURN(status, status);

        status = subscribers_init(&g_subscribers, g_callbacks, DISTANCE_ADC_IDS_NUM);
        LOG_ERROR_RETURN(status, status);

        status = adc_init();
        LOG_ERROR_RETURN(status, status);

        status = timer_init();
        LOG_ERROR_RETURN(status, status);

        status = timer_create(&g_timer, tmr_evt_handler, NULL);
        LOG_ERROR_RETURN(status, status);
    }

    return status;
}

winsens_status_t distance_subscribe(winsens_event_handler_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return subscribers_add(&g_subscribers, callback);
}

winsens_status_t distance_get(distance_sensor_id_t id, int16_t* value)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(DISTANCE_ADC_IDS_NUM > id, WINSENS_INVALID_PARAMS);

    *value = g_values[g_adc_ids[id]];

    return WINSENS_OK;
}

winsens_status_t distance_enable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(DISTANCE_ADC_IDS_NUM > id, WINSENS_INVALID_PARAMS);

    winsens_status_t status = adc_start(g_adc_ids[id], adc_evt_handler);
    LOG_ERROR_RETURN(status, status);

    return timer_start(&g_timer, DISTANCE_CFG_PROBE_INTERVAL_MS, true);
}

void distance_disable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, ;);
    LOG_WARNING_BOOL_RETURN(DISTANCE_ADC_IDS_NUM > id, ;);

    timer_stop(&g_timer);
    adc_stop(g_adc_ids[id]);
}

static void adc_evt_handler(adc_channel_id_t id, int16_t value)
{
    if (!g_initialized)
    {
        return;
    }

    g_values[g_adc_ids[id]] = value;

    winsens_event_t evt = { .id = DISTANCE_EVT_NEW_DATA, .data = id };
    subscribers_update(&g_subscribers, evt);
}

static void tmr_evt_handler(winsens_event_t event)
{
    if (TIMER_EVT_SIGNAL == event.id)
    {
        digital_io_set(DISTANCE_CFG_SENSOR_POWER_PIN, DISTANCE_CFG_POWER_ON);
        adc_probe(g_adc_ids, DISTANCE_ADC_IDS_NUM);
        digital_io_set(DISTANCE_CFG_SENSOR_POWER_PIN, DISTANCE_CFG_POWER_OFF);
    }
}