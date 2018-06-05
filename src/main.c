/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"

#include "nrf_delay.h"
#include "app_error.h"
#define NRF_LOG_MODULE_NAME "MAIN"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"

int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;
    uint32_t err_code;

    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_INFO("XXX Start\n");

    status = WINSENS_Init();
    if (WINSENS_OK != status) return -1;

    status = WINSENS_Loop();

    NRF_LOG_ERROR("WINSENS_Loop failed with %u\n", status);

    WINSENS_Deinit();
    return 0;
}
