/*
 * button.c
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#include "button.h"
#include "button_cfg.h"
#include "digital_io_cfg.h"
#define ILOG_MODULE_NAME BUTN
#include "log.h"
#include "system.h"


#define BUTTON_PINS_NUMBER                  (sizeof(g_buttons_config) / sizeof(digital_io_input_pins_t))
#define BUTTON_CALLBACK_INIT                { DIGITAL_INPUT_PIN_INVALID, 0, NULL }

#define BUTTON_PUSH_NORMAL_DURATION         1   // 100ms
#define BUTTON_PUSH_LONG_DURATION           15  // 1500ms
#define BUTTON_PUSH_VERY_LONG_DURATION      25  // 2500ms

typedef struct {
    digital_io_input_pins_t pin;
    uint32_t onTime;
    winsens_event_handler_t eventHandler;

} button_pin_callback_t;

static void digital_input_callback(
    digital_io_input_pins_t pin,
    bool on);

static const digital_io_input_pins_t g_buttons_config[] = BUTTONS_COOFIG_INIT;
static button_pin_callback_t g_button_callbacks[BUTTON_PINS_NUMBER];


winsens_status_t button_init(void)
{
    uint32_t i;
    for (i = 0; i < BUTTON_PINS_NUMBER; ++i)
    {
        g_button_callbacks[i] = (button_pin_callback_t) BUTTON_CALLBACK_INIT;
    }

    return WINSENS_OK;
}

winsens_status_t button_register_callback(
    digital_io_input_pins_t pin,
    winsens_event_handler_t eventHandler)
{
    uint32_t i;
    for (i = 0; i < BUTTON_PINS_NUMBER; ++i)
    {
        if (g_buttons_config[i] == pin)
        {
            winsens_status_t status = digital_io_register_callback(pin, digital_input_callback);
            LOG_ERROR_RETURN(status, status);

            g_button_callbacks[i].pin = pin;
            g_button_callbacks[i].eventHandler = eventHandler;

            return WINSENS_OK;
        }
    }
    return WINSENS_NOT_FOUND;
}

void button_unregister_callback(
    digital_io_input_pins_t pin)
{
    uint32_t i;
    for (i = 0; i < BUTTON_PINS_NUMBER; ++i)
    {
        if (g_button_callbacks[i].pin == pin)
        {
            g_button_callbacks[i] = (button_pin_callback_t) BUTTON_CALLBACK_INIT;
            return;
        }
    }
}

static void digital_input_callback(
    digital_io_input_pins_t pin,
    bool on)
{
    uint32_t i;
    for (i = 0; i < BUTTON_PINS_NUMBER; ++i)
    {
        if (g_button_callbacks[i].pin == pin &&
            g_button_callbacks[i].eventHandler)
        {
            uint32_t duration = 0;

            if (on)
            {
                g_button_callbacks[i].onTime = system_get_time();
                return;
            }
            else
            {
                duration = system_get_time() - g_button_callbacks[i].onTime;
            }

            button_event_t push = BUTTON_EVENT_NORMAL;
            if (BUTTON_PUSH_VERY_LONG_DURATION <= duration)
            {
                push = BUTTON_EVENT_VERY_LONG;
            }
            else if (BUTTON_PUSH_LONG_DURATION <= duration)
            {
                push = BUTTON_EVENT_LONG;
            }
            else if (BUTTON_PUSH_NORMAL_DURATION <= duration)
            {
                push = BUTTON_EVENT_NORMAL;
            }
            else
            {
                // do nothing
                return;
            }

            winsens_event_t e = { { push }, { pin } };
            g_button_callbacks[i].eventHandler(e);
        }
    }
}
