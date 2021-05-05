/*
 * spi.h
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */

#ifndef SPI_H_
#define SPI_H_


#include "winsens_types.h"


typedef uint8_t spi_t;

typedef enum
{
    SPI_EVT_TRANSFER_DONE = (WINSENS_IDS_MODULE_SPI << 16),
} spi_evt_t;

typedef struct
{
    winsens_event_handler_t evt_handler;
    uint32_t                freq;
    uint8_t                 sck_pin;
    uint8_t                 miso_pin;
    uint8_t                 mosi_pin;
    uint8_t                 ss_pin;
} spi_cfg_t;


winsens_status_t spi_init(void);

winsens_status_t spi_subscribe(spi_t spi, winsens_event_handler_t evt_handler);

winsens_status_t spi_transfer(spi_t spi, uint8_t *tx, uint16_t tx_len, uint8_t *rx, uint16_t rx_len);


#endif /* SPI_H_ */
