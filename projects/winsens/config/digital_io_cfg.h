/*
 * digital_io_cfg.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef DIGITAL_IO_CFG_H_
#define DIGITAL_IO_CFG_H_


typedef enum {
    DIGITAL_IO_INPUT_PAIR_BTN = 1,

    DIGITAL_IO_INPUT_NUMBER

} digital_io_input_pins_enum_t;

#define DIGITAL_IO_CFG_INPUT_INIT { \
    { DIGITAL_IO_INPUT_PAIR_BTN, DIGITAL_IO_INPUT_PULL_DOWN }, \
}

#endif /* DIGITAL_IO_CFG_H_ */
