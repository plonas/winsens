/*
 * spi_cfg.h
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */

#ifndef SPI_CFG_H_
#define SPI_CFG_H_


#include "nrf_gpio.h"


#define SPI_CFG_NRF_INIT    { \
    NRFX_SPI_INSTANCE(0), \
}

#define SPI_CFG_INIT        { \
    { \
    .evt_handler    = NULL, \
    .freq           = NRF_SPI_FREQ_125K, \
    .sck_pin        = NRF_GPIO_PIN_MAP(1, 15), \
    .miso_pin       = NRF_GPIO_PIN_MAP(1, 13), \
    .mosi_pin       = NRF_GPIO_PIN_MAP(0, 2), \
    .ss_pin         = NRF_GPIO_PIN_MAP(1, 10) \
    } \
}


typedef enum
{
    SPI_CFG_ACC
} spi_cfg_enum_t;


#endif /* SPI_CFG_H_ */
