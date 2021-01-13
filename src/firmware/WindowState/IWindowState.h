/*
 * IWindowState.h
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#ifndef IWINDOW_STATE_H_
#define IWINDOW_STATE_H_

#include "winsens_types.h"


typedef enum
{
    IWINDOW_STATE_UNKNOWN,
    IWINDOW_STATE_OPEN,
    IWINDOW_STATE_CLOSED

} IWindowState_e;

typedef uint8_t IWindowId_t;

typedef void (*IWindowStateCallback_f)(IWindowId_t window, IWindowState_e state);


WINSENS_Status_e IWindowStateInit(void);

WINSENS_Status_e IWindowStateSubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback);
void IWindowStateUnsubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback);

#endif /* IWINDOW_STATE_H_ */
