/*
 * ws_switch.c
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */


#include "ws_digital_input.h"
#include "digital_input_config.h"
#define WS_LOG_MODULE_NAME DIN
#include "ws_task_queue.h"
#include "ws_log.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"


#define WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT     { WS_DIGITAL_INPUT_PIN_INVALID, NULL, false }
#define WS_DIGITAL_INPUT_PINS_NUMBER            (sizeof(ws_digitalInputConfig) / sizeof(WS_DigitalInputPinCfg_t))


typedef struct
{
    WS_DigitalInputPins_e pin;
    WS_DigitalInputCallback_f callback;
    bool status;

} WS_DigitalInputPinCallback_t;


static void WS_DigitalInputIrqHandler(
    nrfx_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action);
static void WS_DigitalInputEventHandler(
    void *p_event_data,
    uint16_t event_size);

static nrf_gpio_pin_pull_t WS_ConvertPullUpDown(
    WS_DigitalInputPullUpDown_e pull);

static uint32_t ws_initCount = 0;
static WS_DigitalInputPinCallback_t ws_pinCallbacks[WS_DIGITAL_INPUT_PINS_NUMBER];


WINSENS_Status_e WS_DigitalInputInit(void)
{
    ++ws_initCount;
    if (1 == ws_initCount)
    {
        uint8_t i = 0;

        for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;
        }

        if (NRFX_SUCCESS != nrfx_gpiote_init())
        {
            return WINSENS_ERROR;
        }

        for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
        {
            nrf_gpio_cfg_input(ws_digitalInputConfig[i].pin, WS_ConvertPullUpDown(ws_digitalInputConfig[i].pullUpDown));
        }
    }
    return WINSENS_OK;
}

void WS_DigitalInputDeinit(void)
{
    if (0 < ws_initCount)
    {
        --ws_initCount;
        if (0 == ws_initCount)
        {
            nrfx_gpiote_uninit();
        }
    }
}

WINSENS_Status_e WS_DigitalInputRegisterCallback(
    WS_DigitalInputPins_e pin,
    WS_DigitalInputCallback_f callback)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == ws_digitalInputConfig[i].pin)
        {
            nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
            config.pull = WS_ConvertPullUpDown(ws_digitalInputConfig[i].pullUpDown);
            ret_code_t ret = nrfx_gpiote_in_init((nrfx_gpiote_pin_t)pin, &config, WS_DigitalInputIrqHandler);
            if (NRFX_SUCCESS != ret)
            {
                WS_LOG_ERROR("nrf_drv_gpiote_in_init failed\r\n");
                return WINSENS_ERROR;
            }
            nrfx_gpiote_in_event_enable(pin, true);

            ws_pinCallbacks[i].pin = pin;
            ws_pinCallbacks[i].callback = callback;
            ws_pinCallbacks[i].status =  nrfx_gpiote_in_is_set(pin);

            return WINSENS_OK;
        }
    }

    return WINSENS_NOT_FOUND;
}

void WS_DigitalInputUnregisterCallback(
    WS_DigitalInputPins_e pin)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;

            nrfx_gpiote_in_event_disable(pin);
            nrfx_gpiote_in_uninit(pin);

            return;
        }
    }
}

static void WS_DigitalInputIrqHandler(
    nrfx_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action)
{
    uint8_t i = 0;
    WINSENS_Status_e status = WINSENS_ERROR;

    for (i = 0; i < WS_DIGITAL_INPUT_PINS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            status = WS_TaskQueueAdd(&i, sizeof(i), WS_DigitalInputEventHandler);
            if (WINSENS_OK != status)
            {
                WS_LOG_ERROR("WS_TaskQueueAdd failed\r\n");
            }
            break;
        }
    }
}

static void WS_DigitalInputEventHandler(
    void *p_event_data,
    uint16_t event_size)
{
    const uint8_t *pi =  p_event_data;
    const uint8_t i = *pi;
    UNUSED_PARAMETER(event_size);

    ws_pinCallbacks[i].status = !ws_pinCallbacks[i].status;
    ws_pinCallbacks[i].callback(ws_pinCallbacks[i].pin, ws_pinCallbacks[i].status);
}

static nrf_gpio_pin_pull_t WS_ConvertPullUpDown(
    WS_DigitalInputPullUpDown_e pull)
{
    switch (pull)
    {
    case WS_DIGITAL_INPUT_PULL_UP:
        return NRF_GPIO_PIN_PULLUP;
        break;
    case WS_DIGITAL_INPUT_PULL_DOWN:
        return NRF_GPIO_PIN_PULLDOWN;
        break;
    default:
        break;
    }

    return NRF_GPIO_PIN_NOPULL;
}
