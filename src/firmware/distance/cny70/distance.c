/*
 * distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "distance.h"

#include "adc_cfg.h"
#include "app_error.h"
#include "utils/utils.h"
#include "adc.h"
#include "subscribers.h"


static void adc_event_handler(adc_channel_id_t id, int16_t value);

static bool g_initialized = false;
static winsens_event_handler_t g_callbacks[ADC_CHANNELS_NUMBER];
static subscribers_t g_subscribers;
static int16_t g_values[ADC_CHANNELS_NUMBER] = {0};

winsens_status_t distance_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;

        status = subscribers_init(&g_subscribers, g_callbacks, ADC_CHANNELS_NUMBER);

        status = adc_init();
    }

    return status;
}

winsens_status_t distance_subscribe(winsens_event_handler_t callback)
{
    return subscribers_add(&g_subscribers, callback);
}

winsens_status_t distance_get(adc_channel_id_t ch, int16_t* value)
{
    if (ADC_CHANNELS_NUMBER <= ch)
    {
        return WINSENS_INVALID_PARAMS;
    }

    *value = g_values[ch];

    return WINSENS_OK;
}

winsens_status_t distance_enable(adc_channel_id_t ch)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > ch);

    return adc_start(ch, adc_event_handler);
}

void distance_disable(adc_channel_id_t ch)
{
    if (!g_initialized)
    {
        return;
    }

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > ch);

    adc_stop(ch);
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
