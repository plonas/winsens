/*
 * critical_region.c
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */


#include "critical_region.h"
#include "nrf_nvic.h"


static uint8_t g_nested = 0;


winsens_status_t critical_region_init(void)
{
    return WINSENS_OK;
}

winsens_status_t critical_region_enter(void)
{

    sd_nvic_critical_region_enter(&g_nested);
    return WINSENS_OK;
}

winsens_status_t critical_region_exit(void)
{
    sd_nvic_critical_region_exit(g_nested);
    return WINSENS_OK;
}
