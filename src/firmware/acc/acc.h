/*
 * acc.h
 *
 *  Created on: 16.04.2021
 *      Author: Damian Plonek
 */

#ifndef ACC_H_
#define ACC_H_


#include "winsens_types.h"


typedef enum
{
    ACC_EVT_NEW_DATA = (WINSENS_IDS_MODULE_ACC << 16),
} acc_evt_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t z;
} acc_data_t;


winsens_status_t acc_init(void);

winsens_status_t acc_subscribe(winsens_event_handler_t event_handler);

const acc_data_t* acc_get_data(uint16_t* len);


#endif /* ACC_H_ */
