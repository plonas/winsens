/*
 * storage.c
 *
 *  Created on: 30.04.2019
 *      Author: Damian Plonek
 */


#include "storage.h"
#include "utils/utils.h"
#define ILOG_MODULE_NAME STRG
#include "log.h"
#include "log_internal_nrf52.h"

#include "fds.h"

#include <stdlib.h>


#define STORAGE_WORD_SIZE                                   (sizeof(uint32_t))
#define STORAGE_DATA_BUF_SIZE                               (10 * (STORAGE_WORD_SIZE))
#define STORAGE_FILE_ID                                     0x000B
#define STORAGE_RECORDS_BUFFER_LENGTH                       FDS_OP_QUEUE_SIZE

#define STORAGE_FDS_RECORD_INIT                             { STORAGE_FILE_ID, 0x0000, { NULL, 0 } }
#define STORAGE_DATA_WRAPPER_INIT(p_wrapper)                ((storage_data_wrapper_t) { { 0x00 }, STORAGE_FDS_RECORD_INIT, false })
#define STORAGE_FDS_RECORD_GET_KEY(p_wrapper_m)             ((p_wrapper_m)->record.key)

typedef struct
{
    uint8_t         data[STORAGE_DATA_BUF_SIZE];
    fds_record_t    record;
    bool            used;

} storage_data_wrapper_t;

static bool is_valid_record_id(
    storage_record_id_t recordId);
static bool is_valid_data_size(
    uint32_t size);
static bool set_write_data(
    storage_data_wrapper_t* wrapper,
    storage_record_id_t recordId,
    uint32_t size,
    uint8_t* data);

static void fds_event_handler(
    fds_evt_t const * const p_fds_evt);

static storage_data_wrapper_t g_records_buffer[STORAGE_RECORDS_BUFFER_LENGTH];

winsens_status_t storage_init(void)
{
    uint_fast8_t i = 0;

    ret_code_t ret = fds_register(fds_event_handler);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    ret = fds_init();
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    for (i = 0; i < STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        g_records_buffer[i] = STORAGE_DATA_WRAPPER_INIT(&g_records_buffer[i]);
    }

    fds_gc();

    return WINSENS_OK;
}

winsens_status_t storage_read(
    storage_record_id_t recordId,
    uint32_t size,
    uint8_t* data)
{
    fds_record_desc_t record_desc;
    fds_flash_record_t flash_record;
    fds_find_token_t ftok = { 0, 0 };

    if (!is_valid_record_id(recordId) ||
        !is_valid_data_size(size))
    {
        LOG_WARNING("Params validation failed");
        return WINSENS_INVALID_PARAMS;
    }


    ret_code_t ret = fds_record_find(STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_NOT_FOUND);

    if (fds_record_open(&record_desc, &flash_record) != NRF_SUCCESS)
    {
        return WINSENS_ERROR;
    }

    memcpy(data, flash_record.p_data, MIN(size, (flash_record.p_header->length_words * STORAGE_WORD_SIZE)));

    // Close the record when done.
    ret = fds_record_close(&record_desc);
    LOG_NRF_ERROR_RETURN(ret, WINSENS_ERROR);

    return WINSENS_OK;
}

winsens_status_t storage_write(
    storage_record_id_t recordId,
    uint32_t size,
    uint8_t* data)
{
    ret_code_t          ret = FDS_ERR_INTERNAL;
    fds_record_desc_t   record_desc;
    fds_find_token_t    ftok = { 0, 0 };
    uint_fast8_t        i = 0;

    if (!is_valid_record_id(recordId) ||
        !is_valid_data_size(size))
    {
        return WINSENS_ERROR;
    }

    // Store the record and free it in the callback
    for (i = 0; i < STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        if (!g_records_buffer[i].used)
        {
            UTILS_ASSERT(set_write_data(&g_records_buffer[i], recordId, size, data));
            g_records_buffer[i].used = true;
            break;
        }
    }

    if (STORAGE_RECORDS_BUFFER_LENGTH == i)
    {
        LOG_ERROR("Queue is full");
        return WINSENS_BUSY;
    }

    ret = fds_record_find(STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    if (NRF_SUCCESS == ret)
    {
        ret = fds_record_update(&record_desc, &g_records_buffer[i].record);
        if (NRF_SUCCESS != ret)
        {
            g_records_buffer[i].used = false;
            LOG_ERROR("fds_record_update");
            return WINSENS_ERROR;
        }
    }
    else
    {
        ret = fds_record_write(&record_desc, &g_records_buffer[i].record);
        if (NRF_SUCCESS != ret)
        {
            g_records_buffer[i].used = false;
            LOG_ERROR("fds_record_write %d", ret);
            return WINSENS_NO_RESOURCES;
        }
    }

    return WINSENS_OK;
}

static bool is_valid_record_id(
    storage_record_id_t recordId)
{
    if (FDS_RECORD_KEY_DIRTY == recordId)
    {
        return false;
    }

    return true;
}

static bool is_valid_data_size(
    uint32_t size)
{
    if ((FDS_VIRTUAL_PAGE_SIZE - 14) < size)
    {
        return false;
    }

    return true;
}

static bool set_write_data(
    storage_data_wrapper_t* wrapper,
    storage_record_id_t recordId,
    uint32_t size,
    uint8_t* data)
{
    if (STORAGE_DATA_BUF_SIZE < size)
    {
        return false;
    }

    memcpy(wrapper->data, data, size);
    wrapper->record.data.p_data = wrapper->data;
    wrapper->record.data.length_words = (size + (STORAGE_WORD_SIZE - 1)) / STORAGE_WORD_SIZE;
    wrapper->record.file_id = STORAGE_FILE_ID;
    wrapper->record.key = recordId;

    return true;
}

static void fds_event_handler(
    fds_evt_t const * const p_fds_evt)
{
    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            LOG_IF_WARNING(NRF_SUCCESS != p_fds_evt->result, "FDS_EVT_INIT failed");
            break;

        case FDS_EVT_WRITE:
        {
            uint_fast8_t i = 0;
            for (i = 0; i < STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == STORAGE_FDS_RECORD_GET_KEY(&g_records_buffer[i]))
                {
                    g_records_buffer[i].used = false;
                }
            }

            LOG_IF_WARNING(NRF_SUCCESS != p_fds_evt->result, "FDS_EVT_WRITE failed");

            (void) fds_gc();
            break;
        }

        case FDS_EVT_UPDATE:
        {
            uint_fast8_t i = 0;

            for (i = 0; i < STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == STORAGE_FDS_RECORD_GET_KEY(&g_records_buffer[i]))
                {
                    g_records_buffer[i].used = false;
                }
            }

            LOG_IF_WARNING(NRF_SUCCESS != p_fds_evt->result, "FDS_EVT_UPDATE failed");

            (void) fds_gc();
            break;
        }

        case FDS_EVT_DEL_FILE:
        case FDS_EVT_DEL_RECORD:
            (void) fds_gc();
            break;

        case FDS_EVT_GC:
            LOG_IF_WARNING(NRF_SUCCESS != p_fds_evt->result, "FDS_EVT_GC failed");
            break;

        default:
            break;
    }
}
