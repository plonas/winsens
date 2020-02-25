/*
 * digital_input_config.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef DIGITAL_INPUT_CONFIG_H_
#define DIGITAL_INPUT_CONFIG_H_

#include "ws_digital_input.h"


WS_DigitalInputPinCfg_t const ws_digitalInputConfig[] = {
    { WS_DIGITAL_INPUT_PAIR_BTN, WS_DIGITAL_INPUT_PULL_DOWN }
    };

#endif /* DIGITAL_INPUT_CONFIG_H_ */
