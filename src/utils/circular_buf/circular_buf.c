/*
 * circular_buf.c
 *
 *  Created on: 05.05.2021
 *      Author: Damian Plonek
 */


#include "circular_buf.h"


uint32_t circular_buf_size(const circular_buf_t* buf)
{
    return buf->mif.size(buf);
}

uint32_t circular_buf_push(circular_buf_t *buf, const uint8_t *data, uint32_t len)
{
    return buf->mif.push(buf, data, len);
}

uint32_t circular_buf_pop(circular_buf_t *buf, uint8_t *data, uint32_t len)
{
    return buf->mif.pop(buf, data, len);
}

