/*
 * ws_log_internal.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef WS_LOG_INTERNAL_H_
#define WS_LOG_INTERNAL_H_

#define NRF_LOG_MODULE_NAME                             WS_LOG_MODULE_NAME
#include "nrf_log.h"
#include "nrf_log_ctrl.h"


#define WS_LOG_INIT_INTERNAL(timestamp_func)            NRF_LOG_INIT(timestamp_func)
#define WS_LOG_FLUSH_INTERNAL()                         NRF_LOG_FLUSH()

#define WS_LOG_ERROR_INTERNAL(...)                      NRF_LOG_ERROR(__VA_ARGS__)
#define WS_LOG_WARNING_INTERNAL(...)                    NRF_LOG_WARNING(__VA_ARGS__)
#define WS_LOG_INFO_INTERNAL(...)                       NRF_LOG_INFO(__VA_ARGS__)
#define WS_LOG_DEBUG_INTERNAL(...)                      NRF_LOG_DEBUG(__VA_ARGS__)

#endif /* WS_LOG_INTERNAL_H_ */
