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

#include "nrfx_saadc.h"
#include "app_timer.h"

#include <string.h>


typedef struct
{
    app_timer_t             timer;
    uint32_t                interval_ms;
    uint32_t                channel_mask;
    nrf_saadc_input_t       ain;
    adc_callback_t          callback;
} adc_channel_t;


static void adc_isr(nrfx_saadc_evt_t const *event);
static void timer_isr(void *context);
static void adc_event_handler(void *p_event_data, uint16_t event_size);
static bool trigger_adc(uint32_t channels_mask);
static uint8_t count_bits(uint32_t n);


static adc_channel_t        g_channels[ADC_CHANNELS_NUMBER] = ADC_CFG_CHANNEL_INIT;
static nrf_saadc_value_t    g_adc_buffer[ADC_CHANNELS_NUMBER];
static uint32_t             g_ch_mask_queue = 0;
static uint32_t             g_ch_triggered = 0;
static bool                 g_initialized = false;


LOG_REGISTER();


winsens_status_t adc_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        nrfx_saadc_channel_t channels_conf[ADC_CHANNELS_NUMBER];
        nrfx_err_t err_code = NRF_ERROR_INTERNAL;

        // init a timer
        err_code = app_timer_init();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        // init ADC
        err_code = nrfx_saadc_init(NRFX_SAADC_CONFIG_IRQ_PRIORITY);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        err_code = nrfx_saadc_offset_calibrate(NULL);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        // configure adc channels and timers
        for (int ch = 0; ch < ADC_CHANNELS_NUMBER; ++ch)
        {
            app_timer_id_t timer_id = &g_channels[ch].timer;
            err_code = app_timer_create(&timer_id, APP_TIMER_MODE_REPEATED, timer_isr);
            LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

            g_channels[ch].channel_mask = (1 << ch);

            channels_conf[ch] = (nrfx_saadc_channel_t)NRFX_SAADC_DEFAULT_CHANNEL_SE(g_channels[ch].ain, ch);
        }

        // channels need to be configured at once
        err_code = nrfx_saadc_channels_config(channels_conf, ADC_CHANNELS_NUMBER);
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);
    }

    return WINSENS_OK;
}

winsens_status_t adc_start(adc_channel_id_t channel_id, adc_callback_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(ADC_CHANNELS_NUMBER > channel_id, WINSENS_NOT_FOUND);

    g_channels[channel_id].callback = callback;

    nrfx_err_t err_code = app_timer_start((app_timer_id_t)&g_channels[channel_id].timer, g_channels[channel_id].interval_ms, &g_channels[channel_id]);
    LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

    return WINSENS_OK;
}

void adc_stop(adc_channel_id_t channel_id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    g_channels[channel_id].callback = NULL;

    nrfx_err_t err_code = app_timer_stop((app_timer_id_t)&g_channels[channel_id].timer);
    LOG_NRF_ERROR_RETURN(err_code, ;);
}

static void adc_isr(nrfx_saadc_evt_t const *event)
{
    if (NRFX_SAADC_EVT_DONE == event->type)
    {
        uint8_t ch = 0;
        for (uint8_t i = 0; i < ADC_CHANNELS_NUMBER; ++i)
        {
            if ((1 << i) & g_ch_triggered)
            {
                winsens_status_t status = WINSENS_ERROR;
                adc_event_t adc_evt = { i, (int16_t) event->data.done.p_buffer[ch] };

                status = task_queue_add(&adc_evt, sizeof(adc_evt), adc_event_handler);
                if (WINSENS_OK != status)
                {
                    LOG_ERROR("task_queue_add failed");
                }

                ch++;
            }
        }

        g_ch_triggered = 0;
    }

    if (g_ch_mask_queue)
    {
        if (trigger_adc(g_ch_mask_queue))
        {
            g_ch_triggered = g_ch_mask_queue;
            g_ch_mask_queue = 0;
        }
    }
}

static void timer_isr(void *context)
{
    LOG_ERROR_BOOL_RETURN(NULL != context, ;);

    adc_channel_t *ch = context;

    if (ch->callback)
    {
        g_ch_mask_queue |= ch->channel_mask;

        if (trigger_adc(g_ch_mask_queue))
        {
            g_ch_triggered = g_ch_mask_queue;
            g_ch_mask_queue = 0;
        }
    }
}

static void adc_event_handler(void *p_event_data, uint16_t event_size)
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

static bool trigger_adc(uint32_t channels_mask)
{
    nrfx_err_t err_code = nrfx_saadc_simple_mode_set(channels_mask, NRF_SAADC_RESOLUTION_8BIT, NRF_SAADC_OVERSAMPLE_DISABLED, adc_isr);
    LOG_NRF_ERROR_RETURN(err_code, false);

    uint8_t ch_num = count_bits(channels_mask);
    err_code = nrfx_saadc_buffer_set(&g_adc_buffer[0], ch_num);
    LOG_NRF_ERROR_RETURN(err_code, false);
    
    err_code = nrfx_saadc_mode_trigger();
    LOG_NRF_ERROR_RETURN(err_code, false);

    return true;
}

static uint8_t count_bits(uint32_t n)
{
    int count = 0;
    for(uint8_t i = 0; i < sizeof(n) * 8; ++i)
    {
        count += (n >> i) & 1;
    }

    return count;
}