/*
 * DigitalIoNrf52.c
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */


#include "digital_io.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME DIN
#include "log.h"
#include "log_internal_nrf52.h"
#include "task_queue.h"
#include "nrf_gpio.h"
#include "nrfx_gpiote.h"


#define DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT     { DIGITAL_INPUT_PIN_INVALID, NULL, false }
#define DIGITAL_IO_INPUT_PINS_NUMBER            (sizeof(DIGITAL_IO_INPUT_CONFIG) / sizeof(digital_io_input_pin_cfg_t))
#define DIGITAL_IO_OUTPUT_PINS_NUMBER           (sizeof(DIGITAL_IO_OUTPUT_CONFIG) / sizeof(digital_io_output_pin_cfg_t))


typedef struct
{
    digital_io_pin_t            pin_no;
    digitalio_input_callback_t  callback;
    bool                        status;

} digital_io_input_pin_callback_t;


static void digital_io_input_isr(nrfx_gpiote_pin_t pin_no, nrf_gpiote_polarity_t action);
static void digital_io_input_event_handler(void *p_event_data, uint16_t event_size);

static nrf_gpio_pin_pull_t convert_pull_up_down(digital_io_pull_up_down_t pull);

static const digital_io_input_pin_cfg_t     DIGITAL_IO_INPUT_CONFIG[] = DIGITAL_IO_CFG_INPUT_INIT;
static const digital_io_output_pin_cfg_t    DIGITAL_IO_OUTPUT_CONFIG[] = DIGITAL_IO_CFG_OUTPUT_INIT;
static bool                                 g_initialized = false;
static digital_io_input_pin_callback_t      g_pin_callbacks[DIGITAL_IO_INPUT_PINS_NUMBER];


LOG_REGISTER();


winsens_status_t digital_io_init(void)
{
    if (false == g_initialized)
    {
        g_initialized = true;

        uint8_t i = 0;
        nrfx_err_t err_code;

        for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
        {
            g_pin_callbacks[i] = (digital_io_input_pin_callback_t) DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT;
        }

        err_code = nrfx_gpiote_init();
        LOG_NRF_ERROR_RETURN(err_code, WINSENS_ERROR);

        for (int i = 0; i < DIGITAL_IO_OUTPUT_PINS_NUMBER; ++i)
        {
            nrf_gpio_cfg_output(DIGITAL_IO_OUTPUT_CONFIG[i].pin_no);
        }
    }

    return WINSENS_OK;
}

winsens_status_t digital_io_in_get_cfg(digital_io_pin_t pin, digital_io_input_pin_cfg_t* cfg)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    LOG_ERROR_BOOL_RETURN(NULL != cfg, WINSENS_INVALID_PARAMS);
    
    if (pin < DIGITAL_IO_INPUT_PINS_NUMBER)
    {
        *cfg = DIGITAL_IO_INPUT_CONFIG[pin];
        return WINSENS_OK;
    }

    return WINSENS_NOT_FOUND;
}

winsens_status_t digital_io_set(digital_io_pin_t pin, bool on)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    
    if (on)
    {
        nrf_gpio_pin_set(DIGITAL_IO_OUTPUT_CONFIG[pin].pin_no);
    }
    else
    {
        nrf_gpio_pin_clear(DIGITAL_IO_OUTPUT_CONFIG[pin].pin_no);
    }

    return WINSENS_OK;
}

bool digital_io_get(digital_io_pin_t pin)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    
    bool state = nrf_gpio_pin_out_read(DIGITAL_IO_OUTPUT_CONFIG[pin].pin_no);

    return state;
}

winsens_status_t digital_io_register_callback(digital_io_pin_t pin, digitalio_input_callback_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);

    if (pin < DIGITAL_IO_INPUT_PINS_NUMBER)
    {
        nrfx_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(false);
        config.pull = convert_pull_up_down(DIGITAL_IO_INPUT_CONFIG[pin].pullUpDown);
        nrfx_err_t ret = nrfx_gpiote_in_init((nrfx_gpiote_pin_t)DIGITAL_IO_INPUT_CONFIG[pin].pin_no, &config, digital_io_input_isr);
        LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);
        LOG_FLUSH();

        nrfx_gpiote_in_event_enable(DIGITAL_IO_INPUT_CONFIG[pin].pin_no, true);

        g_pin_callbacks[pin].pin_no     = DIGITAL_IO_INPUT_CONFIG[pin].pin_no;
        g_pin_callbacks[pin].callback   = callback;
        g_pin_callbacks[pin].status     = nrfx_gpiote_in_is_set(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);

        return WINSENS_OK;
    }

    return WINSENS_NOT_FOUND;
}

void digital_io_unregister_callback(digital_io_pin_t pin)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    if (pin < DIGITAL_IO_INPUT_PINS_NUMBER)
    {
        g_pin_callbacks[pin] = (digital_io_input_pin_callback_t) DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT;

        nrfx_gpiote_in_event_disable(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);
        nrfx_gpiote_in_uninit(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);
    }
}

void digital_io_unregister_all(void)
{
    for (digital_io_pin_t pin = 0; pin < DIGITAL_IO_INPUT_PINS_NUMBER; ++pin)
    {
        g_pin_callbacks[pin] = (digital_io_input_pin_callback_t) DIGITAL_IO_INPUT_PIN_CALLBACKS_INIT;

        nrfx_gpiote_in_event_disable(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);
        nrfx_gpiote_in_uninit(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);
    }
}

static void digital_io_input_isr(nrfx_gpiote_pin_t pin_no, nrf_gpiote_polarity_t action)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    uint32_t i = 0;
    winsens_status_t status = WINSENS_ERROR;

    for (i = 0; i < DIGITAL_IO_INPUT_PINS_NUMBER; ++i)
    {
        if (pin_no == g_pin_callbacks[i].pin_no)
        {
            status = task_queue_add(&i, sizeof(i), digital_io_input_event_handler);
            LOG_IF_WARNING(status, "WS_TaskQueueAdd failed");
            break;
        }
    }
}

static void digital_io_input_event_handler(void *p_event_data, uint16_t event_size)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );

    const uint8_t *pi =  p_event_data;
    const digital_io_pin_t pin = *pi;
    UNUSED_PARAMETER(event_size);

    g_pin_callbacks[pin].status = nrfx_gpiote_in_is_set(DIGITAL_IO_INPUT_CONFIG[pin].pin_no);
    g_pin_callbacks[pin].callback(pin, g_pin_callbacks[pin].status);
}

static nrf_gpio_pin_pull_t convert_pull_up_down(digital_io_pull_up_down_t pull)
{
    switch (pull)
    {
    case DIGITAL_IO_INPUT_PULL_UP:
        return NRF_GPIO_PIN_PULLUP;
        break;
    case DIGITAL_IO_INPUT_PULL_DOWN:
        return NRF_GPIO_PIN_PULLDOWN;
        break;
    default:
        break;
    }

    return NRF_GPIO_PIN_NOPULL;
}
