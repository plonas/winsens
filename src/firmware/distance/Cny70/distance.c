/*
 * distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "distance.h"

#include "app_error.h"
#include "utils/utils.h"
#include "adc.h"


static void adc_event_handler(
    adc_channel_id_t id,
    int16_t value);

static distance_callback_t g_callbacks[ADC_CHANNELS_NUMBER];

winsens_status_t distance_init(void)
{
    winsens_status_t status = adc_init();
    return status;
}

winsens_status_t distance_enable(
    adc_channel_id_t channelId,
    distance_callback_t callback)
{
    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);

    g_callbacks[channelId] = callback;

    winsens_status_t status = adc_enable_channel(channelId, adc_event_handler);
    if (WINSENS_OK != status)
    {
        g_callbacks[channelId] = NULL;
    }

    return status;
}

void distance_disable(
    adc_channel_id_t channelId)
{
    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);

    adc_disable_channel(channelId);
    g_callbacks[channelId] = NULL;
}

winsens_status_t distance_start(void)
{
    return adc_start();
}

void distance_stop(void)
{
    adc_stop();
}

static void adc_event_handler(
    adc_channel_id_t id,
    int16_t value)
{
    if (NULL != g_callbacks[id])
    {
        g_callbacks[id](id, value);
    }
}
