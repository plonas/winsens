/*
 * i2c_cfg.h
 *
 *  Created on: 06.07.2022
 *      Author: Damian Plonek
 */

#ifndef I2C_CFG_H_
#define I2C_CFG_H_


#include "nrf_gpio.h"


#define I2C_CFG_NRF_INIT    { \
    NRFX_TWI_INSTANCE(0), \
}

#define I2C_CFG_INIT        { \
    { \
    .evt_handler    = NULL, \
    .freq           = NRF_TWI_FREQ_400K, \
    .addr           = 0x18, \
    .scl_pin        = NRF_GPIO_PIN_MAP(0, 22), \
    .sda_pin        = NRF_GPIO_PIN_MAP(0, 20), \
    } \
}


typedef enum
{
    I2C_CFG_ACC
} i2c_cfg_enum_t;


#endif /* I2C_CFG_H_ */
