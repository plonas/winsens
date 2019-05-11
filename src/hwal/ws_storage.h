/*
 * ws_storage.h
 *
 *  Created on: 30.04.2019
 *      Author: Damian Plonek
 */

#ifndef WS_STORAGE_H_
#define WS_STORAGE_H_

#include "winsens_types.h"


typedef uint16_t WS_StorageRecordId_t;


WINSENS_Status_e WS_StorageInit(void);
//void WS_StorageDeInit(void);

WINSENS_Status_e WS_StorageRead(
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data);
WINSENS_Status_e WS_StorageWrite(
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data);


#endif /* WS_STORAGE_H_ */
