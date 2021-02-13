/*
 * adc.c
 *
 *  Created on: 18.10.2018
 *      Author: Damian Plonek
 */

#include "adc.h"
#include "adc_cfg.h"
#include "task_queue.h"
#include "timer.h"
#include "utils.h"
#define ILOG_MODULE_NAME ADC
#include "log.h"
#include "log_internal_nrf52.h"

//#include "nrfx.h"
//#include "nrf_drv_saadc.h"
#include "nrfx_saadc.h"
#include "nrfx_ppi.h"
#include "nrfx_timer.h"

#include <string.h>


typedef struct
{
    adc_callback_t callback;
    nrfx_saadc_channel_t nativeChannelConf;

} adc_channel_t;

static void adc_isr(
    nrfx_saadc_evt_t const *event);
static void timer_isr(
    nrf_timer_event_t eventType,
    void* context);
static void adc_event_handler(
    void *p_event_data,
    uint16_t event_size);


static const uint32_t       ADC_CHANNEL_CONFIG[ADC_CHANNELS_NUMBER] = ADC_CFG_CHANNEL_INIT;

static nrf_ppi_channel_t    g_ppi_channel_adc;
static adc_channel_t        g_channels[ADC_CHANNELS_NUMBER];

static nrf_saadc_value_t    g_adc_buffer[ADC_CHANNELS_NUMBER];
static uint8_t              g_active_adc_channels_num = 0;

static nrfx_timer_t         g_timer = NRFX_TIMER_INSTANCE(1);

static bool                 g_initialized = false;

winsens_status_t adc_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        nrfx_err_t err_code = NRF_ERROR_INTERNAL;

        memset(g_channels, 0, sizeof(adc_channel_t) * ADC_CHANNELS_NUMBER);
        g_active_adc_channels_num = 0;

        // init a timer
        nrfx_timer_config_t timer_cfg = NRFX_TIMER_DEFAULT_CONFIG;
        err_code = nrfx_timer_init(&g_timer, &timer_cfg, timer_isr);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
        nrfx_timer_extended_compare(&g_timer, NRF_TIMER_CC_CHANNEL0, 31250UL, NRF_TIMER_SHORT_COMPARE1_CLEAR_MASK, false);

        // init ADC
        err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        // init PPI
        err_code = nrfx_ppi_channel_alloc(&g_ppi_channel_adc);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
        err_code = nrfx_ppi_channel_assign(g_ppi_channel_adc,
                                           nrfx_timer_event_address_get(&g_timer, NRF_TIMER_EVENT_COMPARE0),
                                           nrf_saadc_task_address_get(NRF_SAADC_TASK_START));
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
    }

    return WINSENS_OK;
}

winsens_status_t adc_enable_channel(
    adc_channel_id_t channelId,
    adc_callback_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    nrfx_err_t err_code;

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);
    UTILS_ASSERT(callback);

    // Add channel to the list
    g_channels[channelId].callback = callback;
    g_channels[channelId].nativeChannelConf = (nrfx_saadc_channel_t) NRFX_SAADC_DEFAULT_CHANNEL_SE(ADC_CHANNEL_CONFIG[channelId], channelId);
//    ws_channels[channelId].nativeChannelConf.config.config.input = NRF_ADC_CONFIG_SCALING_INPUT_ONE_THIRD;

    // Increase the number of active channels
    g_active_adc_channels_num++;

    // init the channel
    err_code = nrfx_saadc_channels_config(&g_channels[channelId].nativeChannelConf, 1);
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
    LOG_NRF_ERROR_CHECK(err_code);

    err_code = nrfx_saadc_simple_mode_set(channelId + 1, NRF_SAADC_RESOLUTION_8BIT, NRF_SAADC_OVERSAMPLE_DISABLED, adc_isr); //todo bit mask
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void adc_disable_channel(
    adc_channel_id_t channelId)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    UTILS_ASSERT(ADC_CHANNELS_NUMBER > channelId);

    if (g_active_adc_channels_num)
    {
        // Stop sampling
        nrfx_ppi_channel_disable(g_ppi_channel_adc);
    }

    g_channels[channelId].callback = NULL;
    g_active_adc_channels_num--;

    if (g_active_adc_channels_num)
    {
        nrfx_err_t err_code;
        // Start sampling
        err_code = nrfx_saadc_buffer_set(g_adc_buffer, g_active_adc_channels_num);
        LOG_NRF_ERROR_RETURN(err_code, ;);
        err_code = nrfx_ppi_channel_enable(g_ppi_channel_adc);
        LOG_NRF_ERROR_RETURN(err_code, ;);
    }
}

winsens_status_t adc_start(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    uint32_t err_code;

    // Start sampling
    err_code = nrfx_saadc_buffer_set(g_adc_buffer, g_active_adc_channels_num);
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
    err_code = nrfx_ppi_channel_enable(g_ppi_channel_adc);
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void adc_stop(void)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    if (g_active_adc_channels_num)
    {
        // Stop sampling
        nrfx_err_t err_code = nrfx_ppi_channel_disable(g_ppi_channel_adc);
        LOG_NRF_ERROR_RETURN(err_code, ;);
    }
}

static void adc_isr(
    nrfx_saadc_evt_t const *event)
{
    if (NRFX_SAADC_EVT_DONE == event->type)
    {
        uint32_t i;

        if (g_active_adc_channels_num != event->data.done.size)
        {
            LOG_ERROR("Active channels #%u but got %u reads", g_active_adc_channels_num, event->data.done.size);
            return;
        }

        for (i = 0; i < event->data.done.size; i++)
        {
            winsens_status_t status = WINSENS_ERROR;
            adc_event_t adcEvent = { i, (int16_t) event->data.done.p_buffer[i] };

            status = task_queue_add(&adcEvent, sizeof(adcEvent), adc_event_handler);
            if (WINSENS_OK != status)
            {
                LOG_ERROR("WS_TaskQueueAdd failed");
            }
        }

        nrfx_err_t err_code = nrfx_saadc_buffer_set(g_adc_buffer, g_active_adc_channels_num);
        LOG_NRF_ERROR_CHECK(err_code);
    }
}

static void timer_isr(
    nrf_timer_event_t eventType,
    void* context)
{
}

static void adc_event_handler(
    void *p_event_data,
    uint16_t event_size)
{
    const adc_event_t *adcEvent = p_event_data;
    UNUSED_PARAMETER(event_size);

    UTILS_ASSERT(adcEvent);

    if (g_channels[adcEvent->id].callback)
    {
        g_channels[adcEvent->id].callback(adcEvent->id, adcEvent->value);
    }
    else
    {
        LOG_ERROR("Missing channel's #%u callback", adcEvent->id);
    }
}
