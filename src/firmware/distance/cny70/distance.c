/*
 * distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "distance.h"
#include "distance_cfg.h"
#include "digital_io.h"
#include "adc_cfg.h"
#include "app_error.h"
#include "utils/utils.h"
#include "adc.h"
#include "subscribers.h"
#define ILOG_MODULE_NAME distance
#include "log.h"


static void adc_event_handler(adc_channel_id_t id, int16_t value);

static bool g_initialized = false;
static winsens_event_handler_t g_callbacks[ADC_CHANNELS_NUMBER];
static subscribers_t g_subscribers;
static int16_t g_values[ADC_CHANNELS_NUMBER] = {0};


LOG_REGISTER();


winsens_status_t distance_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;

        status = digital_io_init();
        LOG_ERROR_RETURN(status, status);

        status = subscribers_init(&g_subscribers, g_callbacks, ADC_CHANNELS_NUMBER);
        LOG_ERROR_RETURN(status, status);

        status = adc_init();
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
    LOG_WARNING_BOOL_RETURN(ADC_CHANNELS_NUMBER > id, WINSENS_INVALID_PARAMS);

    *value = g_values[id];

    return WINSENS_OK;
}

winsens_status_t distance_enable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_WARNING_BOOL_RETURN(ADC_CHANNELS_NUMBER > id, WINSENS_INVALID_PARAMS);

    digital_io_set(DISTANCE_CFG_SENSOR_POWER_PIN, DISTANCE_CFG_POWER_ON);
    return adc_start(id, adc_event_handler);
}

void distance_disable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, ;);
    LOG_WARNING_BOOL_RETURN(ADC_CHANNELS_NUMBER > id, ;);

    adc_stop(id);
    digital_io_set(DISTANCE_CFG_SENSOR_POWER_PIN, DISTANCE_CFG_POWER_OFF);
}

static void adc_event_handler(adc_channel_id_t id, int16_t value)
{
    if (!g_initialized)
    {
        return;
    }

    g_values[id] = value;

    winsens_event_t evt = { .id = DISTANCE_EVT_NEW_DATA, .data = id };
    subscribers_update(&g_subscribers, evt);
}
