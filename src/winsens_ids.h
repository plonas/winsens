/*
 * winsens_ids.h
 *
 *  Created on: 30.07.2020
 *      Author: Damian Plonek
 */

#ifndef WINSENS_IDS_H_
#define WINSENS_IDS_H_

#include <stdint.h>


typedef enum
{
    WINSENS_IDS_MODULE_BUTTON = 0x0001,
    WINSENS_IDS_MODULE_SERVER,
    WINSENS_IDS_MODULE_BLE_PERIPHERAL,
    WINSENS_IDS_MODULE_SPI,
    WINSENS_IDS_MODULE_I2C,
    WINSENS_IDS_MODULE_ACC,
    WINSENS_IDS_MODULE_WINDOW_STATE,
    WINSENS_IDS_MODULE_TIMER,
    WINSENS_IDS_MODULE_DISTANCE,
    WINSENS_IDS_MODULE_PWR_MGR,
    WINSENS_IDS_MODULE_WINSENS,
    WINSENS_IDS_MODULE_BATTERY,
} winsens_ids_module_enum_t;

typedef uint16_t winsens_ids_module_t;


#endif /* WINSENS_IDS_H_ */
