/*
 * spi_cfg.h
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */

#ifndef SPI_CFG_H_
#define SPI_CFG_H_


#define SPI_CFG_MAX_SUBSCRIBERS     (2)
#define SPI_CFG_RX_BUFFER_SIZE      (128)
#define SPI_CFG_TX_BUFFER_SIZE      (128)

#define SPI_CFG_FREQUENCY           NRF_SPI_FREQ_125K

#define SPI_CFG_SCK_PIN             (1)
#define SPI_CFG_MISO_PIN            (1)
#define SPI_CFG_MOSI_PIN            (1)
#define SPI_CFG_SS_PIN              NRFX_SPI_PIN_NOT_USED


#endif /* SPI_CFG_H_ */
