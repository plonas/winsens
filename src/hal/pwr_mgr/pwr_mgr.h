/*
 * pwr_mgr.h
 *
 *  Created on: 07.10.2021
 *      Author: Damian.Plonek
 */

#ifndef PWR_MGR_H_
#define PWR_MGR_H_

#include "winsens_types.h"
#include "winsens_ids.h"


typedef enum
{
    PWR_MGR_EVT_PREPARE_TO_SLEEP = (WINSENS_IDS_MODULE_PWR_MGR << 16),
    PWR_MGR_EVT_PREPARE_TO_SHUTDOWN,
} pwr_mgr_evt_t;

typedef enum
{
    PWR_MGR_ACTIVITY_IDLE,
    PWR_MGR_ACTIVITY_GOTO_SLEEP,
    PWR_MGR_ACTIVITY_GOTO_SHUTDOWN,
} pwr_mgr_activity_t;


winsens_status_t pwr_mgr_init(void);

winsens_status_t pwr_mgr_subscribe(winsens_event_handler_t callback);
void pwr_mgr_callback(winsens_event_t event);

winsens_status_t pwr_mgr_sleep(void);
winsens_status_t pwr_mgr_shutdown(void);

winsens_status_t pwr_mgr_prevent(void);


#endif /* PWR_MGR_H_ */
