/*
 * circular_buf.h
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */

#ifndef CIRCULAR_BUF_H_
#define CIRCULAR_BUF_H_


#include "winsens_types.h"


typedef struct
{
    uint32_t    capacity;
    bool        full;
    uint32_t    head;
    uint32_t    tail;
    uint8_t*    buffer;
} circular_buf_t;


winsens_status_t circular_buf_init(circular_buf_t* buf, uint8_t* data_buffer, uint32_t size);

uint32_t circular_buf_size(const circular_buf_t* buf);

uint32_t circular_buf_push(circular_buf_t* buf, const uint8_t* data, uint32_t len);

uint32_t circular_buf_pop(circular_buf_t* buf, uint8_t* data, uint32_t len);


#endif /* CIRCULAR_BUF_H_ */
