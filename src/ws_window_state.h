/*
 * ws_window_state.h
 *
 *  Created on: 16.10.2018
 *      Author: Damian.Plonek
 */

#ifndef WS_WINDOW_STATE_H_
#define WS_WINDOW_STATE_H_

typedef enum
{
    WS_WINDOW_STATE_UNKNOWN,
    WS_WINDOW_STATE_OPEN,
    WS_WINDOW_STATE_CLOSED

} WS_WindowState_e;

typedef enum
{
    WS_WINDOW_1,
    WS_WINDOW_2,
    WS_WINDOW_3,

    WS_WINDOWS_NUMBER
} WS_Window_e;


WINSENS_Status_e WS_WindowStateInit(void);

void WS_WindowStateDeinit(void);

WS_WindowState_e WS_WindowStateGet(
    WS_Window_e windowsId);

#endif /* WS_WINDOW_STATE_H_ */
