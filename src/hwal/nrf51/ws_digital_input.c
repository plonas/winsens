/*
 * ws_switch.c
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */


#include "ws_digital_input.h"
#define WS_LOG_MODULE_NAME " DIN"
#include "ws_task_queue.h"
#include "ws_log.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"


#define WS_DIGITAL_INPUT_CALLBACKS_NUMBER       2
#define WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT     { WS_DIGITAL_INPUT_PIN_INVALID, NULL, WS_DIGITAL_INPUT_PIN_CFG_INIT, false }


typedef struct
{
    WS_DigitalInputPin_t pin;
    WS_DigitalInputCallback_f callback;
    WS_DigitalInputPinCfg_t cfg;
    bool status;

} WS_DigitalInputPinCallback_t;


static WS_DigitalInputPinCallback_t ws_pinCallbacks[WS_DIGITAL_INPUT_CALLBACKS_NUMBER];

static uint32_t ws_initCount = 0;
static void WS_DigitalInputIrqHandler(
    nrf_drv_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action);
static void WS_DigitalInputEventHandler(
    void *p_event_data,
    uint16_t event_size);

static nrf_gpio_pin_pull_t WS_ConvertPullUpDown(
    WS_DigitalInputPullUpDown_e pull);


WINSENS_Status_e WS_DigitalInputInit(void)
{
    ++ws_initCount;
    if (1 == ws_initCount)
    {
        uint8_t i = 0;

        for (i = 0; i < WS_DIGITAL_INPUT_CALLBACKS_NUMBER; ++i)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;
        }

        if (NRF_SUCCESS != nrf_drv_gpiote_init())
        {
            return WINSENS_ERROR;
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
            nrf_drv_gpiote_uninit();
        }
    }
}

WINSENS_Status_e WS_DigitalInputSetPinConfig(
    WS_DigitalInputPin_t pin,
    WS_DigitalInputPinCfg_t pinCfg)
{
    uint8_t i = 0;
    uint8_t freePinIndex = 0xFF;

    for (i = 0; i < WS_DIGITAL_INPUT_CALLBACKS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            ws_pinCallbacks[i].cfg = pinCfg;
            break;
        }
        else if (0xFF == freePinIndex &&
                 WS_DIGITAL_INPUT_PIN_INVALID == ws_pinCallbacks[i].pin)
        {
            freePinIndex = i;
        }
    }

    if (WS_DIGITAL_INPUT_CALLBACKS_NUMBER == i)
    {
        if (0xFF != freePinIndex)
        {
            ws_pinCallbacks[freePinIndex].pin = pin;
            ws_pinCallbacks[freePinIndex].cfg = pinCfg;
        }
        else
        {
            return WINSENS_NO_RESOURCES;
        }
    }

    nrf_gpio_cfg_input(pin, WS_ConvertPullUpDown(pinCfg.pullUpDown));
    return WINSENS_OK;
}

WINSENS_Status_e WS_DigitalInputRegisterCallback(
    WS_DigitalInputPin_t pin,
    WS_DigitalInputCallback_f callback)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_CALLBACKS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            nrf_drv_gpiote_in_config_t config = GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
            config.pull = WS_ConvertPullUpDown(ws_pinCallbacks[i].cfg.pullUpDown);
            ret_code_t ret = nrf_drv_gpiote_in_init(pin, &config, WS_DigitalInputIrqHandler);
            if (NRF_SUCCESS != ret)
            {
                return WINSENS_ERROR;
            }
            nrf_drv_gpiote_in_event_enable(pin, true);

            ws_pinCallbacks[i].callback = callback;
            ws_pinCallbacks[i].status =  nrf_drv_gpiote_in_is_set(pin);

            return WINSENS_OK;
        }
    }

    return WINSENS_NOT_FOUND;
}

void WS_DigitalInputUnregisterCallback(
    WS_DigitalInputPin_t pin)
{
    uint8_t i = 0;

    for (i = 0; i < WS_DIGITAL_INPUT_CALLBACKS_NUMBER; ++i)
    {
        if (pin == ws_pinCallbacks[i].pin)
        {
            ws_pinCallbacks[i] = (WS_DigitalInputPinCallback_t) WS_DIGITAL_INPUT_PIN_CALLBACKS_INIT;

            nrf_drv_gpiote_in_event_disable(pin);
            nrf_drv_gpiote_in_uninit(pin);

            return;
        }
    }
}

static void WS_DigitalInputIrqHandler(
    nrf_drv_gpiote_pin_t pin,
    nrf_gpiote_polarity_t action)
{
    uint8_t i = 0;
    WINSENS_Status_e status = WINSENS_ERROR;

    for (i = 0; i < WS_DIGITAL_INPUT_CALLBACKS_NUMBER; ++i)
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
    ws_pinCallbacks[i].callback(i, ws_pinCallbacks[i].status);
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
