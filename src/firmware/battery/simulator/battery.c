/*
 * battery.c
 *
 *  Created on: 20.11.2021
 *      Author: Damian.Plonek
 */

#include "battery.h"
#include "timer.h"


#define BATTERY_CB_NUMBER           (2)
#define BATTERY_UPDATE_INTERVAL     (2000)


/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void timer_evt_handler(winsens_event_t evt);
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
static void timer_evt_handler(winsens_event_t evt)
{
    g_level += 1;
    if (100 < g_level)
    {
        g_level = 0;
    }

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