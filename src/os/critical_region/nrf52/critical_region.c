/*
 * critical_region.c
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */


#include "critical_region.h"
#include "log.h"
#include "nrf_nvic.h"


static uint8_t g_nested = 0;


winsens_status_t critical_region_init(void)
{
    return WINSENS_OK;
}

winsens_status_t critical_region_enter(void)
{
    uint8_t nested = 0;
    sd_nvic_critical_region_enter(&nested);
    g_nested++;
    return WINSENS_OK;
}

winsens_status_t critical_region_exit(void)
{
    if (0 == g_nested)
    {
        LOG_WARNING("Critical region exited too many times");
        return WINSENS_ERROR;
    }

    uint8_t nested = (1 < g_nested ? 1 : 0);
    g_nested--;
    sd_nvic_critical_region_exit(nested);
    return WINSENS_OK;
}
