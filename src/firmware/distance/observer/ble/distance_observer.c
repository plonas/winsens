/*
 * distance_observer.c
 *
 *  Created on: 31.07.2023
 *      Author: Damian.Plonek
 */

#include "ble_peripheral.h"
#include "ble_peripheral_cfg.h"
#include "distance_observer.h"
#include "distance_observer_cfg.h"
#include "distance.h"
#define ILOG_MODULE_NAME dist_obr
#include "log.h"


#define DISTANCE_OBSERVER_IDS_LEN       (sizeof(g_adc_ids)/sizeof(g_adc_ids[0]))


static void distance_evt_handler(winsens_event_t evt);
static void update_distance(distance_sensor_id_t id);


static adc_channel_id_t g_adc_ids[] = DISTANCE_OBSERVER_CFG_IDS_INIT;


LOG_REGISTER();


winsens_status_t distance_observer_init(void)
{
    winsens_status_t status = ble_peripheral_init();
    LOG_ERROR_RETURN(status, status);

    status = distance_init();
    LOG_ERROR_RETURN(status, status);

    status = distance_subscribe(distance_evt_handler);
    LOG_ERROR_RETURN(status, status);

    for (uint8_t i = 0; i < DISTANCE_OBSERVER_IDS_LEN; ++i)
    {
        update_distance(g_adc_ids[i]);
    }

    return WINSENS_OK;
}

void distance_evt_handler(winsens_event_t evt)
{
    update_distance(evt.data);
}

void update_distance(distance_sensor_id_t id)
{
    int16_t value = 0;

    winsens_status_t status = distance_get(id, &value);
    LOG_WARNING_RETURN(status, ;);

    status = ble_peripheral_update(BLE_PERIPERAL_SVC_CS, BLE_PERIPERAL_CHAR_DISTANCE, sizeof(value), (uint8_t *)&value);
    LOG_ERROR_RETURN(status, ;);
}