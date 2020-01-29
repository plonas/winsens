/*
 * ws_digital_input.h
 *
 *  Created on: 17.01.2020
 *      Author: Damian Plonek
 */

#ifndef WS_DIGITAL_INPUT_H_
#define WS_DIGITAL_INPUT_H_

#include "winsens_types.h"

#define WS_DIGITAL_INPUT_PIN_INVALID        0xFFFFFFFF
#define WS_DIGITAL_INPUT_PIN_CFG_INIT       { WS_DIGITAL_INPUT_PULL_NONE }


typedef uint32_t WS_DigitalInputPin_t;

typedef enum
{
    WS_DIGITAL_INPUT_PULL_NONE,
    WS_DIGITAL_INPUT_PULL_UP,
    WS_DIGITAL_INPUT_PULL_DOWN

} WS_DigitalInputPullUpDown_e;

typedef struct
{
    WS_DigitalInputPullUpDown_e pullUpDown;

} WS_DigitalInputPinCfg_t;

typedef void (*WS_DigitalInputCallback_f)(WS_DigitalInputPin_t pin, bool on);


WINSENS_Status_e WS_DigitalInputInit(void);

void WS_DigitalInputDeinit(void);

WINSENS_Status_e WS_DigitalInputSetPinConfig(
    WS_DigitalInputPin_t pin,
    WS_DigitalInputPinCfg_t pinCfg);

WINSENS_Status_e WS_DigitalInputRegisterCallback(
    WS_DigitalInputPin_t pin,
    WS_DigitalInputCallback_f callback);
void WS_DigitalInputUnregisterCallback(
    WS_DigitalInputPin_t pin);

#endif /* WS_DIGITAL_INPUT_H_ */
