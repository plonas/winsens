/*
 * pwr_mgr.c
 *
 *  Created on: 07.10.2021
 *      Author: Damian Plonek
 */

#include "pwr_mgr.h"
#include "pwr_mgr_cfg.h"
#include "subscribers.h"
#include "digital_io.h"
#define ILOG_MODULE_NAME PWRM
#include "log.h"
#include "log_internal_nrf52.h"

#include "nrf_pwr_mgmt.h"
#include "nrf_gpio.h"


#define PWR_MGR_ACTIVITY_MAP_LEN    ( sizeof(g_activity_map)/sizeof(g_activity_map[0]) )
#define PWR_MGR_WAKEUP_PINS_LEN     ( sizeof(g_wakeup_pins)/sizeof(g_wakeup_pins[0]) )


typedef struct
{
    winsens_event_id_t  event_id;
    pwr_mgr_activity_t  activity;
} pwr_mgr_activity_pair_t;

typedef struct 
{
    digital_io_pin_t    pin;
    bool                hi;
} pwr_mgr_wake_up_pin_t;


static winsens_status_t shutdown(pwr_mgr_activity_t a);
static void set_wkup_pins(void);
static bool shutdown_handler(nrf_pwr_mgmt_evt_t event);
static nrf_gpio_pin_pull_t convert_pull_up_down(digital_io_pull_up_down_t pull);


static pwr_mgr_activity_t               g_activity = PWR_MGR_ACTIVITY_IDLE;
static bool                             g_prevent = false;
static uint32_t                         g_subs_index = 0;
static const pwr_mgr_activity_pair_t    g_activity_map[] = PWR_MGR_CFG_ACTIVITY_MAP_INIT;
static subscribers_t                    g_subscribers;
static winsens_event_handler_t          g_evt_handlers[PWR_MGR_CFG_SUBSCRIBERS_NUM];
static const pwr_mgr_wake_up_pin_t      g_wakeup_pins[] = PWR_MGR_CFG_WAKEUP_PINS;


// Register application shutdown handler with priority 0
NRF_PWR_MGMT_HANDLER_REGISTER(shutdown_handler, 0);
LOG_REGISTER();


winsens_status_t pwr_mgr_init(void)
{
    ret_code_t ret = nrf_pwr_mgmt_init();
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    ret = subscribers_init(&g_subscribers, g_evt_handlers, PWR_MGR_CFG_SUBSCRIBERS_NUM);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    ret = digital_io_init();
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    return WINSENS_OK;
}

winsens_status_t pwr_mgr_subscribe(winsens_event_handler_t callback)
{
    return subscribers_add(&g_subscribers, callback);
}

void pwr_mgr_callback(winsens_event_t event)
{
    for (uint32_t i = 0; i < PWR_MGR_ACTIVITY_MAP_LEN; ++i)
    {
        if (g_activity_map[i].event_id == event.id)
        {
            shutdown(g_activity_map[i].activity);
            break;
        }
    }
}

winsens_status_t pwr_mgr_sleep(void)
{
    g_prevent = false;
    g_subs_index = 0;
    return shutdown(PWR_MGR_ACTIVITY_GOTO_SLEEP);
}

winsens_status_t pwr_mgr_shutdown(void)
{
    g_prevent = false;
    g_subs_index = 0;
    return shutdown(PWR_MGR_ACTIVITY_GOTO_SHUTDOWN);
}

winsens_status_t pwr_mgr_prevent(void)
{
    g_prevent = true;
    return WINSENS_OK;
}

winsens_status_t pwr_mgr_continue(void)
{
    g_prevent = false;
    shutdown(g_activity);
    return WINSENS_OK;
}

static winsens_status_t shutdown(pwr_mgr_activity_t a)
{
    nrf_pwr_mgmt_shutdown_t nrf_shutdown;
    pwr_mgr_evt_t event_id;

    if (PWR_MGR_ACTIVITY_GOTO_SLEEP == a)
    {
        nrf_shutdown = NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF;
        event_id = PWR_MGR_EVT_PREPARE_TO_SLEEP;
    }
    else if (PWR_MGR_ACTIVITY_GOTO_SHUTDOWN == a)
    {
        nrf_shutdown = NRF_PWR_MGMT_SHUTDOWN_STAY_IN_SYSOFF;
        event_id = PWR_MGR_EVT_PREPARE_TO_SHUTDOWN;
    }
    else
    {
        return WINSENS_INVALID_PARAMS;
    }

    g_activity = a;

    //go thru subscribers
    const winsens_event_t e = { .id = event_id, .data = 0 };
    for (; g_subs_index < PWR_MGR_CFG_SUBSCRIBERS_NUM; ++g_subs_index)
    {
        if (g_evt_handlers[g_subs_index])
        {
            g_evt_handlers[g_subs_index](e);
            if (g_prevent)
            {
                break;
            }
        }
    }

    if (!g_prevent)
    {
        g_subs_index = 0;
        nrf_pwr_mgmt_shutdown(nrf_shutdown);
    }

    return WINSENS_OK;
}

static void set_wkup_pins(void)
{
    for (uint8_t i = 0; i < PWR_MGR_WAKEUP_PINS_LEN; ++i)
    {
        const nrf_gpio_pin_sense_t sense = g_wakeup_pins[i].hi ? NRF_GPIO_PIN_SENSE_HIGH : NRF_GPIO_PIN_SENSE_LOW;
        digital_io_input_pin_cfg_t pin_cfg;

        winsens_status_t status = digital_io_in_get_cfg(g_wakeup_pins[i].pin, &pin_cfg);
        LOG_ERROR_RETURN(status, );

        (void)convert_pull_up_down;
        (void)sense;
        nrf_gpio_cfg_sense_input(pin_cfg.pin_no, convert_pull_up_down(pin_cfg.pullUpDown), sense);
    }
}

static bool shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_SYSOFF:
            LOG_INFO("Prepare for power off");
            digital_io_unregister_all();
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_WAKEUP:
            LOG_INFO("Prepare for sleep");
            digital_io_unregister_all();
            set_wkup_pins();
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            LOG_INFO("Prepare for DFU");
            break;

        case NRF_PWR_MGMT_EVT_PREPARE_RESET:
            LOG_INFO("Prepare for reset");
            break;

        default:
            break;
    }

    return true;
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
