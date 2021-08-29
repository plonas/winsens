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


static void adc_event_handler(
    adc_channel_id_t id,
    int16_t value);

static bool g_initialized = false;
static distance_callback_t g_callbacks[ADC_CHANNELS_NUMBER];

winsens_status_t distance_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;

        status = adc_init();
    }

    return status;
}

winsens_status_t distance_enable(
    adc_channel_id_t channelId,
    distance_callback_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);

    g_callbacks[channelId] = callback;

    return adc_start(channelId, adc_event_handler);
}

void distance_disable(
    adc_channel_id_t channelId)
{
    if (!g_initialized)
    {
        return;
    }

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);

    adc_stop(channelId);

    g_callbacks[channelId] = NULL;
}

static void adc_event_handler(
    adc_channel_id_t id,
    int16_t value)
{
    if (!g_initialized)
    {
        return;
    }

    if (NULL != g_callbacks[id])
    {
        g_callbacks[id](id, value);
    }
}
