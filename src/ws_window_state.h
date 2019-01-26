/*
 * ws_window_state.h
 *
 *  Created on: 16.10.2018
 *      Author: Damian.Plonek
 */

#ifndef WS_WINDOW_STATE_H_
#define WS_WINDOW_STATE_H_

#include "winsens_types.h"


typedef void (*WS_WindowStateCallback_f)(WS_Window_e window, WS_WindowState_e state);


WINSENS_Status_e WS_WindowStateInit(
    WS_WindowStateCallback_f callback);

void WS_WindowStateDeinit(void);

WS_WindowState_e WS_WindowStateGet(
    WS_Window_e windowsId);

#endif /* WS_WINDOW_STATE_H_ */
