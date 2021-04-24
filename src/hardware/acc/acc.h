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
    int16_t x;
    int16_t y;
    int16_t z;
} acc_t;

typedef struct
{
    acc_t acc;
    uint16_t time_delta;
} acc_data_t;


winsens_status_t acc_init(void);

winsens_status_t acc_subscribe(winsens_event_handler_t event_handler);

winsens_status_t acc_set_high_pass(int16_t x, int16_t y, int16_t z);

winsens_status_t acc_get_data(acc_data_t* data);


#endif /* ACC_H_ */
