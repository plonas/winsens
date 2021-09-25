/*
 * window_state_observer_cfg.h
 *
 *  Created on: 25.09.2021
 *      Author: Damian.Plonek
 */

#ifndef WINDOW_STATE_OBSERVER_CFG_H_
#define WINDOW_STATE_OBSERVER_CFG_H_


#include "ble_peripheral_cfg.h"
#include "window_state_cfg.h"


#define WINDOW_STATE_OBSERVER_CFG_MAP_INIT      { \
    { WINDOW_STATE_CFG_WINDOW, BLE_PERIPERAL_CHAR_CS_THRESHOLD_WINDOW, BLE_PERIPERAL_CHAR_WMS_STATE }, \
 }


#endif /* WINDOW_STATE_OBSERVER_CFG_H_ */
