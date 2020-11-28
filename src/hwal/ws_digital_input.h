/*
 * ws_digital_input.h
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */

#ifndef WS_DIGITAL_INPUT_H_
#define WS_DIGITAL_INPUT_H_

#include "winsens_types.h"

#define WS_DIGITAL_INPUT_PIN_INVALID        (-1)
#define WS_DIGITAL_INPUT_PIN_CFG_INIT       { WS_DIGITAL_INPUT_PULL_NONE }


typedef enum {
    WS_DIGITAL_INPUT_PAIR_BTN = 1,

} WS_DigitalInputPins_e;

typedef enum
{
    WS_DIGITAL_INPUT_PULL_NONE,
    WS_DIGITAL_INPUT_PULL_UP,
    WS_DIGITAL_INPUT_PULL_DOWN

} WS_DigitalInputPullUpDown_e;

typedef struct
{
    WS_DigitalInputPins_e pin;
    WS_DigitalInputPullUpDown_e pullUpDown;

} WS_DigitalInputPinCfg_t;

typedef void (*WS_DigitalInputCallback_f)(WS_DigitalInputPins_e pin, bool on);


WINSENS_Status_e WS_DigitalInputInit(void);

void WS_DigitalInputDeinit(void);

WINSENS_Status_e WS_DigitalInputRegisterCallback(
    WS_DigitalInputPins_e pin,
    WS_DigitalInputCallback_f callback);
void WS_DigitalInputUnregisterCallback(
    WS_DigitalInputPins_e pin);

#endif /* WS_DIGITAL_INPUT_H_ */
