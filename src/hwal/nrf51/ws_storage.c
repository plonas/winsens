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


#define WS_STORAGE_WORD_SIZE                (sizeof(uint32_t))
#define WS_STORAGE_FILE_ID                  0xB00B
#define WS_STORAGE_RECORDS_BUFFER_LENGTH    8


static bool isValidRecordId(
    WS_StorageRecordId_t recordId);
bool isValidDataSize(
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
    fds_find_token_t ftok;

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

        if (size < (flash_record.p_header->tl.length_words * WS_STORAGE_WORD_SIZE))
        {
            (void) fds_record_close(&record_desc);
            return WINSENS_ERROR;
        }

        // Access the record through the flash_record structure.
        memcpy(data, flash_record.p_data, flash_record.p_header->tl.length_words * WS_STORAGE_WORD_SIZE);

        // Close the record when done.
        if (fds_record_close(&record_desc) != FDS_SUCCESS)
        {
            return WINSENS_ERROR;
        }
    }
    else
    {
        return WINSENS_ERROR;
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
    fds_find_token_t    ftok;
    void                *dataCopy = NULL;
    uint_fast8_t        i = 0;

    if (!isValidRecordId(recordId) ||
        !isValidDataSize(size))
    {
        return WINSENS_ERROR;
    }

    record = malloc(sizeof(fds_record_t) + sizeof(fds_record_chunk_t) + size);
    record_chunk = (fds_record_chunk_t *) (record + sizeof(fds_record_t));
    dataCopy = record_chunk + sizeof(fds_record_chunk_t);

    memcpy(dataCopy, data, size);

    // Set up data
    record_chunk->p_data         = dataCopy;
    record_chunk->length_words   = size / WS_STORAGE_WORD_SIZE;

    // Set up record
    record->file_id         = WS_STORAGE_FILE_ID;
    record->key             = recordId;
    record->data.p_chunks   = record_chunk;
    record->data.num_chunks = 1;

    ret = fds_record_find(WS_STORAGE_FILE_ID, recordId, &record_desc, &ftok);
    if (FDS_SUCCESS == ret)
    {
        ret = fds_record_update(&record_desc, record);
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

    // Store the record and free it in the callback
    for (i = 0; i < WS_STORAGE_RECORDS_BUFFER_LENGTH; ++i)
    {
        if (NULL == ws_recordsBuffer[i])
        {
            ws_recordsBuffer[i] = record;
        }
    }
    if (WS_STORAGE_RECORDS_BUFFER_LENGTH == i)
    {
        NRF_LOG_ERROR("Memory leak while writing to a storage (%u bytes)\n", sizeof(fds_record_t) + sizeof(fds_record_chunk_t) + size);
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

bool isValidDataSize(
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

        case FDS_EVT_GC:
            if (FDS_SUCCESS != p_fds_evt->result)
            {
                NRF_LOG_ERROR("Storage GC failed\n");
            }
            break;

        default:
            break;
    }
}
