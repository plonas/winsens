/*
 * circular_buf.c
 *
 *  Created on: 15.04.2021
 *      Author: Damian Plonek
 */


#include "circular_buf.h"
#include "critical_region.h"
#include "nordic_common.h"
#include "string.h"


winsens_status_t circular_buf_init(circular_buf_t* buf, uint8_t* data_buffer, uint32_t size)
{
    buf->full       = false;
    buf->head       = 0;
    buf->tail       = 0;
    buf->buffer     = data_buffer;
    buf->capacity   = size;

    return WINSENS_OK;
}

uint32_t circular_buf_size(const circular_buf_t* buf)
{
    CRITICAL_REGION_ENTER();

    uint32_t size = buf->capacity;

    if (!buf->full)
    {
        if (buf->head > buf->tail)
        {
            size = buf->head - buf->tail;
        }
        else
        {
            size = buf->capacity - (buf->head - buf->tail);
        }
    }

    CRITICAL_REGION_EXIT();
    return size;
}

uint32_t circular_buf_push(circular_buf_t *buf, const uint8_t *data, uint32_t len)
{
    CRITICAL_REGION_ENTER();

    uint32_t copied = 0;

    if (!buf->full)
    {
        if (buf->head > buf->tail)
        {
            uint32_t to_copy = MIN(len, buf->capacity - buf->head);
            memcpy(&buf->buffer[buf->head], data, to_copy);
            copied = to_copy;

            if (copied < len)
            {
                to_copy = MIN(len - copied, buf->tail);
                memcpy(&buf->buffer[0], data, to_copy);
                copied += to_copy;
            }
        }
        else
        {
            uint32_t to_copy = MIN(len, buf->tail - buf->head);
            memcpy(&buf->buffer[buf->head], data, to_copy);
            copied = to_copy;
        }

        buf->head = (buf->head + copied) % buf->capacity;

        if (buf->head == buf->tail)
        {
            buf->full = true;
        }
    }

    CRITICAL_REGION_EXIT();
    return copied;
}

uint32_t circular_buf_pop(circular_buf_t *buf, uint8_t *data, uint32_t len)
{
    CRITICAL_REGION_ENTER();

    uint32_t copied = 0;

    if (buf->head > buf->tail)
    {
        uint32_t to_copy = MIN(len, buf->head - buf->tail);
        memcpy(data, &buf->buffer[buf->tail], to_copy);
        copied = to_copy;
    }
    else
    {
        uint32_t to_copy = MIN(len, buf->capacity - buf->tail);
        memcpy(data, &buf->buffer[buf->tail], to_copy);
        copied = to_copy;

        if (copied < len)
        {
            to_copy = MIN(len - copied, buf->head);
            memcpy(data + copied, &buf->buffer[0], to_copy);
            copied += to_copy;
        }
    }

    buf->tail = (buf->tail + copied) % buf->capacity;

    if (buf->head == buf->tail)
    {
        buf->full = true;
        buf->head = 0;
        buf->tail = 0;
    }

    CRITICAL_REGION_EXIT();
    return copied;
}

