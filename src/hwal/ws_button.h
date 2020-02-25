/*
 * ws_button.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef WS_BUTTON_H_
#define WS_BUTTON_H_

#include "ws_digital_input.h"


typedef enum {
    WS_BUTTON_PUSH_NORMAL, // 100ms
    WS_BUTTON_PUSH_LONG, // 1500ms
    WS_BUTTON_PUSH_VERY_LONG // 2500ms

} WS_ButtonPushType_e;

typedef void (*WS_ButtonCallback_f)(WS_DigitalInputPins_e pin, WS_ButtonPushType_e pushType);

WINSENS_Status_e WS_ButtonInit(void);

void WS_ButtonDeinit(void);

WINSENS_Status_e WS_ButtonRegisterCallback(
    WS_DigitalInputPins_e pin,
    WS_ButtonCallback_f callback);
void WS_ButtonUnregisterCallback(
    WS_DigitalInputPins_e pin);

#endif /* WS_BUTTON_H_ */
