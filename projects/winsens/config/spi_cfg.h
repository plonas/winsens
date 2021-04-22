/*
 * spi_cfg.h
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */

#ifndef SPI_CFG_H_
#define SPI_CFG_H_


#include "nrf_gpio.h"


#define SPI_CFG_MAX_SUBSCRIBERS     (2)

#define SPI_CFG_FREQUENCY           NRF_SPI_FREQ_125K

#define SPI_CFG_SCK_PIN             NRF_GPIO_PIN_MAP(1, 15)
#define SPI_CFG_MISO_PIN            NRF_GPIO_PIN_MAP(1, 13)
#define SPI_CFG_MOSI_PIN            NRF_GPIO_PIN_MAP(0, 2)
#define SPI_CFG_SS_PIN              NRF_GPIO_PIN_MAP(1, 10)


#endif /* SPI_CFG_H_ */
