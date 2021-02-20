/*
 * storage.h
 *
 *  Created on: 30.04.2019
 *      Author: Damian Plonek
 */

#ifndef STORAGE_H_
#define STORAGE_H_

#include "winsens_types.h"


typedef uint16_t storage_file_id_t;
typedef uint16_t storage_record_id_t;


winsens_status_t storage_init(void);

winsens_status_t storage_read(
    storage_file_id_t file_id,
    storage_record_id_t record_id,
    uint32_t size,
    uint8_t* data);
winsens_status_t storage_write(
    storage_file_id_t file_id,
    storage_record_id_t record_id,
    uint32_t size,
    uint8_t* data);


#endif /* STORAGE_H_ */
