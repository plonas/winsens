/*
 * button_cfg.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef BUTTON_CFG_H_
#define BUTTON_CFG_H_


typedef enum {
    BUTTON_ID_PAIR,

} button_id_enum_t;

//typedef struct
//{
//    digital_io_input_pin_t  pin;
//    bool                    active_low;
//} button_cfg_t;
#define BUTTONS_COOFIG_INIT     { \
    { DIGITAL_IO_INPUT_PAIR_BTN, true }, \
}

#endif /* BUTTON_CFG_H_ */
