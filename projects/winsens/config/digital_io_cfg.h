/*
 * digital_io_cfg.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef DIGITAL_IO_CONFIG_H_
#define DIGITAL_IO_CONFIG_H_

#include "digital_io.h"


digital_io_input_pin_cfg_t const g_digital_io_input_config[] = {
    { DIGITAL_IO_INPUT_PAIR_BTN, DIGITAL_IO_INPUT_PULL_DOWN }
    };

#endif /* DIGITAL_IO_CONFIG_H_ */
