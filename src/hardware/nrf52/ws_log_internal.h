/*
 * ws_log_internal.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef WS_LOG_INTERNAL_H_
#define WS_LOG_INTERNAL_H_

#include "app_util.h"
#include "nordic_common.h"
#define NRF_LOG_MODULE_NAME                             WS_LOG_MODULE_NAME
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

NRF_LOG_MODULE_REGISTER();

#define WS_LOG_INIT_INTERNAL(timestamp_func)            NRF_LOG_INIT(timestamp_func);NRF_LOG_DEFAULT_BACKENDS_INIT()
#define WS_LOG_FLUSH_INTERNAL()                         NRF_LOG_FLUSH()

#define WS_LOG_INTERNAL(T, ...)                         WS_LOG_INTERNAL_X(T, NUM_VA_ARGS_LESS_1(__VA_ARGS__), __VA_ARGS__)

#define WS_LOG_INTERNAL_X(T, N, ...)                    CONCAT_2(WS_LOG_INTERNAL_, N) (T, __VA_ARGS__)

#define WS_LOG_ERROR_INTERNAL(...)                      WS_LOG_INTERNAL(LOG_ERROR, __VA_ARGS__)
#define WS_LOG_WARNING_INTERNAL(...)                    WS_LOG_INTERNAL(LOG_WARNING, __VA_ARGS__)
#define WS_LOG_INFO_INTERNAL(...)                       WS_LOG_INTERNAL(LOG_INFO, __VA_ARGS__)
#define WS_LOG_DEBUG_INTERNAL(...)                      WS_LOG_INTERNAL(LOG_DEBUG, __VA_ARGS__)

#define WS_LOG_INTERNAL_0(T, s)                         CONCAT_2(NRF_, T) (s)
#define WS_LOG_INTERNAL_1(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define WS_LOG_INTERNAL_2(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define WS_LOG_INTERNAL_3(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define WS_LOG_INTERNAL_4(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define WS_LOG_INTERNAL_5(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define WS_LOG_INTERNAL_6(T, s, ...)                    CONCAT_2(NRF_, T) (s, __VA_ARGS__)

#endif /* WS_LOG_INTERNAL_H_ */
