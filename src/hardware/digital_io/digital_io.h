/*
 * digital_io.h
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */

#ifndef DIGITAL_IO_H_
#define DIGITAL_IO_H_

#include "winsens_types.h"

#define DIGITAL_INPUT_PIN_INVALID           (-1)
#define DIGITAL_INPUT_PIN_CFG_INIT          { WS_DIGITAL_INPUT_PULL_NONE }


typedef enum {
    DIGITAL_IO_INPUT_PAIR_BTN = 1,

} digital_io_input_pins_t;

typedef enum
{
    DIGITAL_IO_INPUT_PULL_NONE,
    DIGITAL_IO_INPUT_PULL_UP,
    DIGITAL_IO_INPUT_PULL_DOWN

} digital_io_pull_up_down_t;

typedef struct
{
    digital_io_input_pins_t pin;
    digital_io_pull_up_down_t pullUpDown;

} digital_io_input_pin_cfg_t;

typedef void (*digitalio_input_callback_t)(digital_io_input_pins_t pin, bool on);


winsens_status_t digital_io_init(void);

winsens_status_t digital_io_register_callback(
    digital_io_input_pins_t pin,
    digitalio_input_callback_t callback);
void digital_io_unregister_callback(
    digital_io_input_pins_t pin);

#endif /* DIGITAL_IO_H_ */
