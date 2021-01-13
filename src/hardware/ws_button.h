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
    WS_BUTTON_EVENT_NORMAL = (WS_MODULE_ID_BUTTON << 16), // 100ms
    WS_BUTTON_EVENT_LONG, // 1500ms
    WS_BUTTON_EVENT_VERY_LONG // 2500ms

} WS_ButtonEvent_e;

WINSENS_Status_e WS_ButtonInit(void);

void WS_ButtonDeinit(void);

WINSENS_Status_e WS_ButtonRegisterCallback(
    WS_DigitalInputPins_e pin,
    WS_EventHandler_f eventHandler);
void WS_ButtonUnregisterCallback(
    WS_DigitalInputPins_e pin);

#endif /* WS_BUTTON_H_ */
