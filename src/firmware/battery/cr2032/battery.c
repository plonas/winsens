/*
 * battery.c
 *
 *  Created on: 20.11.2021
 *      Author: Damian.Plonek
 */

#include "battery.h"
#include "battery_cfg.h"
#include "timer.h"
#include "adc.h"
#define ILOG_MODULE_NAME battery
#include "log.h"


#define BATTERY_CB_NUMBER           (2)
#define BATTERY_UPDATE_INTERVAL     (3333)


LOG_REGISTER();


/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void check_lvl(void);
static uint8_t calc_battery_lvl(int16_t voltage);
static void timer_evt_handler(winsens_event_t evt);
static void adc_callback(adc_channel_id_t id, int16_t value);
static void update_subscribers(void);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static uint8_t                  g_level;
static winsens_event_handler_t  g_callbacks[BATTERY_CB_NUMBER] = {NULL};
static timer_ws_t               g_timer;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t battery_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;
        g_level = 0;

        timer_init();
        timer_create(&g_timer, timer_evt_handler, NULL);
        timer_start(&g_timer, BATTERY_UPDATE_INTERVAL, true);

        adc_init();
        adc_start(BATTERY_CFG_ADC_ID, adc_callback);
    }

    return WINSENS_OK;
}

winsens_status_t battery_subscribe(winsens_event_handler_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    for (uint8_t i = 0; i < BATTERY_CB_NUMBER; ++i)
    {
        if (NULL == g_callbacks[i])
        {
            g_callbacks[i] = callback;
            return WINSENS_OK;
        }
    }

    return WINSENS_NO_RESOURCES;
}

winsens_status_t battery_get_level(battery_level_t *level)
{
    if (level)
    {
        *level = g_level;
    }

    return WINSENS_OK;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void check_lvl(void)
{
    const adc_channel_id_t id = BATTERY_CFG_ADC_ID;
    adc_probe(&id, 1);
}

static uint8_t calc_battery_lvl(int16_t voltage)
{
    return voltage / 100; //todo calculate percentage
}

static void timer_evt_handler(winsens_event_t evt)
{
    check_lvl();
}

static void adc_callback(adc_channel_id_t id, int16_t value)
{
    LOG_DEBUG("adc %u val %d", id, value);
    g_level = calc_battery_lvl(adc_get_voltage(value));
    update_subscribers();
}

static void update_subscribers(void)
{
    winsens_event_t e = { .id = WINSENS_IDS_MODULE_BATTERY, .data = g_level };

    for (uint8_t i = 0; i < BATTERY_CB_NUMBER; ++i)
    {
        if (g_callbacks[i])
        {
            g_callbacks[i](e);
        }
    }
}