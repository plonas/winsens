/*
 * WindowState.h
 *
 *  Created on: 09.01.2021
 *      Author: Damian.Plonek
 */

#ifndef WINDOW_STATE_DIST_H_
#define WINDOW_STATE_DIST_H_

#include "IWindowState.h"


WINSENS_Status_e WindowStateDistInit(void);

WINSENS_Status_e WindowStateDistSubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback);
void WindowStateDistUnsubscribe(
    IWindowId_t windowsId,
    IWindowStateCallback_f callback);

#endif /* WINDOW_STATE_DIST_H_ */
