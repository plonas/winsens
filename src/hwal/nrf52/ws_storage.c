/*
 * ws_storage.c
 *
 *  Created on: 30.04.2019
 *      Author: Damian Plonek
 */


#include "ws_storage.h"
#include "utils/utils.h"
#define WS_LOG_MODULE_NAME STRG
#include "ws_log.h"

#include "fds.h"

#include <stdlib.h>


#define WS_STORAGE_WORD_SIZE                (sizeof(uint32_t))
#define WS_STORAGE_DATA_BUF_SIZE            (10 * (WS_STORAGE_WORD_SIZE))
#define WS_STORAGE_FILE_ID                  0x000B
#define WS_STORAGE_RECORDS_BUFFER_LENGTH    FDS_OP_QUEUE_SIZE

#define WS_STORAGE_FDS_RECORD_INIT                                                  { WS_STORAGE_FILE_ID, 0x0000, { NULL, 0 } }
#define WS_STORAGE_DATA_WRAPPER_INIT(p_wrapper)                                     ((WS_StorageDataWrapper_t) { { 0x00 }, WS_STORAGE_FDS_RECORD_INIT, false })
#define WS_STORAGE_FDS_RECORD_GET_KEY(p_wrapper_m)                                  ((p_wrapper_m)->record.key)

typedef struct
{
    uint8_t data[WS_STORAGE_DATA_BUF_SIZE];
    fds_record_t record;
    bool used;

} WS_StorageDataWrapper_t;

static bool isValidRecordId(
    WS_StorageRecordId_t recordId);
static bool isValidDataSize(
    uint32_t size);
static bool setWriteData(
    WS_StorageDataWrapper_t* wrapper,
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data);

static void ws_fds_evt_handler(
    fds_evt_t const * const p_fds_evt);

static WS_StorageDataWrapper_t ws_recordsBuffer[WS_STORAGE_RECORDS_BUFFER_LENGTH];

WINSENS_Status_e WS_StorageInit(void)
{
    uint_fast8_t i = 0;

    ret_code_t ret = fds_register(ws_fds_evt_handler);
    if (ret != NRF_SUCCESS)
    {
        return WINSENS_ERROR;
    }

    ret = fds_init();
    if (ret != NRF_SUCCESS)
    {
        return WINSENS_ERROR;
    }

    for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        ws_recordsBuffer[i] = WS_STORAGE_DATA_WRAPPER_INIT(&ws_recordsBuffer[i]);
    }

    fds_gc();

    return WINSENS_OK;
}

//void WS_StorageDeInit(void)
//{
//}

WINSENS_Status_e WS_StorageRead(
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data)
{
    fds_record_desc_t record_desc;
    fds_flash_record_t flash_record;
    fds_find_token_t ftok = { 0, 0 };

    if (!isValidRecordId(recordId) ||
        !isValidDataSize(size))
    {
        return WINSENS_ERROR;
    }


    ret_code_t ret = fds_record_find(WS_STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    if (ret == NRF_SUCCESS)
    {
        if (fds_record_open(&record_desc, &flash_record) != NRF_SUCCESS)
        {
            return WINSENS_ERROR;
        }

        memcpy(data, flash_record.p_data, MIN(size, (flash_record.p_header->length_words * WS_STORAGE_WORD_SIZE)));

        // Close the record when done.
        if (fds_record_close(&record_desc) != NRF_SUCCESS)
        {
            return WINSENS_ERROR;
        }
    }
    else
    {
        return WINSENS_NOT_FOUND;
    }

    return WINSENS_OK;
}

WINSENS_Status_e WS_StorageWrite(
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data)
{
    ret_code_t          ret = FDS_ERR_INTERNAL;
    fds_record_desc_t   record_desc;
    fds_find_token_t    ftok = { 0, 0 };
    uint_fast8_t        i = 0;

    if (!isValidRecordId(recordId) ||
        !isValidDataSize(size))
    {
        return WINSENS_ERROR;
    }

    // Store the record and free it in the callback
    for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        if (!ws_recordsBuffer[i].used)
        {
            WS_ASSERT(setWriteData(&ws_recordsBuffer[i], recordId, size, data));
            ws_recordsBuffer[i].used = true;
            break;
        }
    }

    if (WS_STORAGE_RECORDS_BUFFER_LENGTH == i)
    {
        WS_LOG_ERROR("Queue is full\r\n");
        return WINSENS_BUSY;
    }

    ret = fds_record_find(WS_STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    if (NRF_SUCCESS == ret)
    {
        ret = fds_record_update(&record_desc, &ws_recordsBuffer[i].record);
        if (NRF_SUCCESS != ret)
        {
            ws_recordsBuffer[i].used = false;
            WS_LOG_ERROR("fds_record_update\r\n");
            return WINSENS_ERROR;
        }
    }
    else
    {
        ret = fds_record_write(&record_desc, &ws_recordsBuffer[i].record);
        if (NRF_SUCCESS != ret)
        {
            ws_recordsBuffer[i].used = false;
            WS_LOG_ERROR("fds_record_write %d\r\n", ret);
            return WINSENS_NO_RESOURCES;
        }
    }

    return WINSENS_OK;
}

static bool isValidRecordId(
    WS_StorageRecordId_t recordId)
{
    if (FDS_RECORD_KEY_DIRTY == recordId)
    {
        return false;
    }

    return true;
}

static bool isValidDataSize(
    uint32_t size)
{
    if ((FDS_VIRTUAL_PAGE_SIZE - 14) < size)
    {
        return false;
    }

    return true;
}

static bool setWriteData(
    WS_StorageDataWrapper_t* wrapper,
    WS_StorageRecordId_t recordId,
    uint32_t size,
    uint8_t* data)
{
    if (WS_STORAGE_DATA_BUF_SIZE < size)
    {
        return false;
    }

    memcpy(wrapper->data, data, size);
    wrapper->record.data.p_data = wrapper->data;
    wrapper->record.data.length_words = (size + (WS_STORAGE_WORD_SIZE - 1)) / WS_STORAGE_WORD_SIZE;
    wrapper->record.file_id = WS_STORAGE_FILE_ID;
    wrapper->record.key = recordId;

    return true;
}

static void ws_fds_evt_handler(
    fds_evt_t const * const p_fds_evt)
{
//    WS_LOG_DEBUG("ws_fds_evt_handler %d\r\n", p_fds_evt->id);

    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (NRF_SUCCESS != p_fds_evt->result)
            {
//                WS_LOG_ERROR("Initialization failed\r\n");
            }
            break;

        case FDS_EVT_WRITE:
        {
            uint_fast8_t i = 0;
            for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == WS_STORAGE_FDS_RECORD_GET_KEY(&ws_recordsBuffer[i]))
                {
                    ws_recordsBuffer[i].used = false;
                }
            }

            if (NRF_SUCCESS != p_fds_evt->result)
            {
//                WS_LOG_ERROR("Storage write failed\r\n");
            }

            (void) fds_gc();
            break;
        }

        case FDS_EVT_UPDATE:
        {
            uint_fast8_t i = 0;

            for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == WS_STORAGE_FDS_RECORD_GET_KEY(&ws_recordsBuffer[i]))
                {
                    ws_recordsBuffer[i].used = false;
                }
            }

            if (NRF_SUCCESS != p_fds_evt->result)
            {
//                WS_LOG_ERROR("Storage update failed\r\n");
            }

            (void) fds_gc();
            break;
        }

        case FDS_EVT_DEL_FILE:
        case FDS_EVT_DEL_RECORD:
            (void) fds_gc();
            break;

        case FDS_EVT_GC:
            if (NRF_SUCCESS != p_fds_evt->result)
            {
//                WS_LOG_ERROR("Storage GC failed\r\n");
            }
            break;

        default:
            break;
    }
}
