/*
 * circular_buf.h
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */

#ifndef CIRCULAR_BUF_H_
#define CIRCULAR_BUF_H_


#include "winsens_types.h"


struct circular_buf_struct_t;


typedef uint32_t (*circular_buf_size_t)(const struct circular_buf_struct_t* buf);
typedef uint32_t (*circular_buf_push_t)(struct circular_buf_struct_t* buf, const uint8_t* data, uint32_t len);
typedef uint32_t (*circular_buf_pop_t)(struct circular_buf_struct_t* buf, uint8_t* data, uint32_t len);

typedef struct
{
    circular_buf_size_t size;
    circular_buf_push_t push;
    circular_buf_pop_t  pop;
} circular_buf_mif_t;

typedef struct circular_buf_struct_t
{
    circular_buf_mif_t   mif;

    uint32_t            capacity;
    bool                full;
    uint32_t            head;
    uint32_t            tail;
    uint8_t*            buffer;
} circular_buf_t;


uint32_t circular_buf_size(const circular_buf_t* buf);

uint32_t circular_buf_push(circular_buf_t* buf, const uint8_t* data, uint32_t len);

uint32_t circular_buf_pop(circular_buf_t* buf, uint8_t* data, uint32_t len);


#endif /* CIRCULAR_BUF_H_ */
