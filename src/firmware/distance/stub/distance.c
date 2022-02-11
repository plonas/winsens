/*
 * distance.c
 *
 *  Created on: 05.01.2018
 *      Author: Damian.Plonek
 */

#include "distance.h"
#include "winsens_types.h"
#define ILOG_MODULE_NAME distance
#include "log.h"


static bool g_initialized = false;
static bool g_enabled = false;


LOG_REGISTER();


winsens_status_t distance_init(void)
{
    winsens_status_t status = WINSENS_OK;

    if (!g_initialized)
    {
        g_initialized = true;
        g_enabled = false;
    }

    return status;
}

winsens_status_t distance_subscribe(winsens_event_handler_t callback)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

winsens_status_t distance_get(distance_sensor_id_t id, int16_t* value)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    *value = 8;
    return WINSENS_OK;
}

winsens_status_t distance_enable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, WINSENS_NOT_INITIALIZED);
    return WINSENS_OK;
}

void distance_disable(distance_sensor_id_t id)
{
    LOG_ERROR_BOOL_RETURN(g_initialized, );
}
