/*
 * i2c.h
 *
 *  Created on: 02.07.2022
 *      Author: Damian Plonek
 */

#ifndef I2C_H_
#define I2C_H_


#include "winsens_types.h"


typedef uint8_t i2c_t;

typedef enum
{
    I2C_EVT_TRANSFER_DONE = (WINSENS_IDS_MODULE_I2C << 16),
} i2c_evt_t;

typedef struct
{
    winsens_event_handler_t evt_handler;
    uint32_t                freq;
    uint8_t                 addr;
    uint8_t                 scl_pin;
    uint8_t                 sda_pin;
} i2c_cfg_t;


winsens_status_t i2c_init(void);

winsens_status_t i2c_subscribe(i2c_t i2c, winsens_event_handler_t evt_handler);

winsens_status_t i2c_tx(i2c_t i2c, uint8_t *tx, uint16_t len);
winsens_status_t i2c_rx(i2c_t i2c, uint8_t *rx, uint16_t len);
winsens_status_t i2c_txtx(i2c_t i2c, uint8_t *tx1, uint16_t len1, uint8_t *tx2, uint16_t len2);
winsens_status_t i2c_txrx(i2c_t i2c, uint8_t *tx, uint16_t len_tx, uint8_t *rx, uint16_t len_rx);


#endif /* I2C_H_ */
