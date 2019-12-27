/*
 * ws_storage.c
 *
 *  Created on: 30.04.2019
 *      Author: Damian Plonek
 */


#include "hwal/ws_storage.h"
#include "fds.h"
#define NRF_LOG_MODULE_NAME "STORAGE"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include <stdlib.h>
#include "fstorage.h"


#define WS_STORAGE_WORD_SIZE                (sizeof(uint32_t))
#define WS_STORAGE_FILE_ID                  0x000B
#define WS_STORAGE_RECORDS_BUFFER_LENGTH    8


static bool isValidRecordId(
    WS_StorageRecordId_t recordId);
static bool isValidDataSize(
    uint32_t size);

static void ws_fds_evt_handler(
    fds_evt_t const * const p_fds_evt);

static fds_record_t *ws_recordsBuffer[WS_STORAGE_RECORDS_BUFFER_LENGTH] = {NULL};


WINSENS_Status_e WS_StorageInit(void)
{
    uint_fast8_t i = 0;

    ret_code_t ret = fds_register(ws_fds_evt_handler);
    if (ret != FDS_SUCCESS)
    {
        return WINSENS_ERROR;
    }

    ret = fds_init();
    if (ret != FDS_SUCCESS)
    {
        return WINSENS_ERROR;
    }
//    while (!ws_operationComplete);

    for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        ws_recordsBuffer[i] = NULL;
    }

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

    if (fds_record_find(WS_STORAGE_FILE_ID, recordId, &record_desc, &ftok) == FDS_SUCCESS)
    {
        if (fds_record_open(&record_desc, &flash_record) != FDS_SUCCESS)
        {
            return WINSENS_ERROR;
        }

        // Access the record through the flash_record structure.
        memcpy(data, flash_record.p_data, MIN(size, flash_record.p_header->tl.length_words * WS_STORAGE_WORD_SIZE));

        // Close the record when done.
        if (fds_record_close(&record_desc) != FDS_SUCCESS)
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
    fds_record_t        *record = NULL;
    fds_record_desc_t   record_desc;
    fds_record_chunk_t  *record_chunk = NULL;
    fds_find_token_t    ftok = { 0, 0 };
    void                *dataCopy = NULL;
    uint_fast8_t        i = 0;
    uint32_t            sizeInWords;

    if (!isValidRecordId(recordId) ||
        !isValidDataSize(size))
    {
        return WINSENS_ERROR;
    }

    sizeInWords = size / WS_STORAGE_WORD_SIZE;
    if (size % WS_STORAGE_WORD_SIZE)
    {
        sizeInWords++;
    }

    record = malloc(sizeof(fds_record_t) + sizeof(fds_record_chunk_t) + (sizeInWords * WS_STORAGE_WORD_SIZE));
    record_chunk = ((void *) record) + sizeof(fds_record_t);
    dataCopy = ((void *) record_chunk) + sizeof(fds_record_chunk_t);

    memset(dataCopy, 0, sizeInWords * WS_STORAGE_WORD_SIZE);
    memcpy(dataCopy, data, size);

    // Set up data
    record_chunk->p_data         = dataCopy;
    record_chunk->length_words   = sizeInWords;

    // Set up record
    record->file_id         = WS_STORAGE_FILE_ID;
    record->key             = recordId;
    record->data.p_chunks   = record_chunk;
    record->data.num_chunks = 1;

    // Store the record and free it in the callback
    for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        if (NULL == ws_recordsBuffer[i])
        {
            ws_recordsBuffer[i] = record;
            break;
        }
    }
    if (WS_STORAGE_RECORDS_BUFFER_LENGTH == i)
    {
        NRF_LOG_ERROR("Memory leak while writing to a storage (%u bytes)\n", sizeof(fds_record_t) + sizeof(fds_record_chunk_t) + size);
    }

    ret = fds_record_find(WS_STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    if (FDS_SUCCESS == ret)
    {
        ret = fds_record_update(&record_desc, record);
        NRF_LOG_FLUSH();
        if (FDS_SUCCESS != ret)
        {
            free(record);
            return WINSENS_ERROR;
        }
    }
    else
    {
        ret = fds_record_write(&record_desc, record);
        if (FDS_SUCCESS != ret)
        {
            free(record);
            return WINSENS_ERROR;
        }
    }

    NRF_LOG_FLUSH();

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

static void ws_fds_evt_handler(
    fds_evt_t const * const p_fds_evt)
{
    NRF_LOG_DEBUG("ws_fds_evt_handler %d\n", p_fds_evt->id);

    switch (p_fds_evt->id)
    {
        case FDS_EVT_INIT:
            if (FDS_SUCCESS != p_fds_evt->result)
            {
                NRF_LOG_ERROR("Initialization failed\n");
            }
            break;

        case FDS_EVT_WRITE:
        {
            uint_fast8_t i = 0;
            for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == ws_recordsBuffer[i]->key)
                {
                    free(ws_recordsBuffer[i]);
                    ws_recordsBuffer[i] = NULL;
                }
            }

            if (FDS_SUCCESS != p_fds_evt->result)
            {
                NRF_LOG_ERROR("Storage write failed\n");
            }

            (void) fds_gc();
            break;
        }

        case FDS_EVT_UPDATE:
        {
            uint_fast8_t i = 0;

            for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
            {
                if (p_fds_evt->write.record_key == ws_recordsBuffer[i]->key)
                {
                    free(ws_recordsBuffer[i]);
                    ws_recordsBuffer[i] = NULL;
                }
            }

            if (FDS_SUCCESS != p_fds_evt->result)
            {
                NRF_LOG_ERROR("Storage update failed\n");
            }

            (void) fds_gc();
            break;
        }

        case FDS_EVT_DEL_FILE:
        case FDS_EVT_DEL_RECORD:
            (void) fds_gc();
            break;

        case FDS_EVT_GC:
            if (FDS_SUCCESS != p_fds_evt->result)
            {
                NRF_LOG_ERROR("Storage GC failed\n");
            }
            break;

        default:
            break;
    }

    NRF_LOG_FLUSH();
}
