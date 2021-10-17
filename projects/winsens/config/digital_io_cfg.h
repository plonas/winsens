/*
 * digital_io_cfg.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef DIGITAL_IO_CFG_H_
#define DIGITAL_IO_CFG_H_


typedef enum {
    DIGITAL_IO_INPUT_PAIR_BTN,
    DIGITAL_IO_INPUT_ACC_INT_HPF,
    DIGITAL_IO_INPUT_ACC_INT_FF,
} digital_io_input_pin_enum_t;

#define DIGITAL_IO_CFG_INPUT_INIT { \
    { NRF_GPIO_PIN_MAP(1, 6), DIGITAL_IO_INPUT_PULL_UP }, \
    { NRF_GPIO_PIN_MAP(0, 15), DIGITAL_IO_INPUT_PULL_NONE }, \
    { NRF_GPIO_PIN_MAP(0, 17), DIGITAL_IO_INPUT_PULL_NONE }, \
}

typedef enum {
    DIGITAL_IO_OUTPUT_DIST_SENS_POWER,
} digital_io_output_pin_enum_t;

#define DIGITAL_IO_CFG_OUTPUT_INIT { \
    { NRF_GPIO_PIN_MAP(0, 20) }, \
}


#endif /* DIGITAL_IO_CFG_H_ */
