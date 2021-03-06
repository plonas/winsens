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
    ACC_EVT_HIPASS_INT,
    ACC_EVT_FREEFALL_INT,
} acc_evt_t;

#pragma pack(push, 1)
typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
} acc_t;
#pragma pack(pop)


winsens_status_t acc_init(void);

winsens_status_t acc_subscribe(winsens_event_handler_t event_handler);

winsens_status_t acc_get_data(acc_t* data, uint16_t len);

uint16_t acc_get_data_len(void);


#endif /* ACC_H_ */
