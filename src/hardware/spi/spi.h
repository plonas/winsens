/*
 * spi.h
 *
 *  Created on: 14.04.2021
 *      Author: Damian Plonek
 */

#ifndef SPI_H_
#define SPI_H_


#include "winsens_types.h"


typedef enum
{
    SPI_EVT_TRANSFER_DONE = (WINSENS_IDS_MODULE_SPI << 16),
} spi_evt_t;


winsens_status_t spi_init(void);

winsens_status_t spi_subscribe(winsens_event_handler_t event_handler);

winsens_status_t spi_transfer(uint8_t *tx, uint16_t tx_len, uint8_t *rx, uint16_t rx_len);


#endif /* SPI_H_ */
