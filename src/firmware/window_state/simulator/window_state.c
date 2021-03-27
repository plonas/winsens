/*
 * window_state.c
 *
 *  Created on: 27.03.2021
 *      Author: Damian.Plonek
 */

#include "window_state.h"
#include "window_state_cfg.h"
#include "timer.h"



/*
 ******************************************************************************
 * Function prototypes
 ******************************************************************************
 */
static void update_state(void);

static void timer_callback(WS_TimerId_t timerId);

/*
 ******************************************************************************
 * Variables
 ******************************************************************************
 */
static bool                     g_initialized = false;
static window_state_type_t      g_window_state[WINDOW_STATE_CFG_NUMBER] = {WINDOW_STATE_UNKNOWN};
static window_state_callback_t  g_callbacks[WINDOW_STATE_CFG_NUMBER] = {NULL};
static WS_TimerId_t             g_timer_id;

/*
 ******************************************************************************
 * Interface functions
 ******************************************************************************
 */
winsens_status_t window_state_init(void)
{
    if (!g_initialized)
    {
        g_initialized = true;

        for (int i = 0; i < WINDOW_STATE_CFG_NUMBER; ++i)
        {
            g_window_state[i] = (i % 2 == 0) ? WINDOW_STATE_OPEN : WINDOW_STATE_CLOSED;
        }

        ITimer_Init();
        ITimer_SetTimer(WINDOW_STATE_CFG_UPDATE_INTERVAL, true, timer_callback, &g_timer_id);
    }

    return WINSENS_OK;
}

winsens_status_t window_state_subscribe( window_id_t windowsId, window_state_callback_t callback)
{
    if (!g_initialized)
    {
        return WINSENS_NOT_INITIALIZED;
    }

    if (windowsId >= WINDOW_STATE_CFG_NUMBER)
    {
        return WINSENS_ERROR;
    }

    if (g_callbacks[windowsId])
    {
        return WINSENS_NO_RESOURCES;
    }

    g_callbacks[windowsId] = callback;

    return WINSENS_OK;
}

void window_state_unsubscribe( window_id_t windowsId, window_state_callback_t callback)
{
    (void) callback;

    if (!g_initialized)
    {
        return;
    }

    if (windowsId >= WINDOW_STATE_CFG_NUMBER)
    {
        return;
    }

    g_callbacks[windowsId] = NULL;
}

/*
 ******************************************************************************
 * Private functions
 ******************************************************************************
 */
static void update_state(void)
{
    for (int i = 0; i < WINDOW_STATE_CFG_NUMBER; ++i)
    {
        g_window_state[i] = (WINDOW_STATE_OPEN == g_window_state[i]) ? WINDOW_STATE_CLOSED : WINDOW_STATE_OPEN;
        if (g_callbacks[i])
        {
            g_callbacks[i](i, g_window_state[i]);
        }
    }
}

static void timer_callback(WS_TimerId_t timerId)
{
    if (timerId == g_timer_id)
    {
        update_state();
    }
}
