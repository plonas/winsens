/*
 * ws_ble_wms.h
 *
 *  Created on: 9 cze 2018
 *      Author: Damian.Plonek
 */

#ifndef WS_BLE_WMS_H_
#define WS_BLE_WMS_H_

#include <stdint.h>

typedef struct
{
    uint32_t service_handle;

} ws_ble_wms_t;

uint32_t ws_ble_wms_init(ws_ble_wms_t *p_wms);

#endif /* WS_BLE_WMS_H_ */
