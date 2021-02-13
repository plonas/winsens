/*
 * LogInternal.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef LOG_INTERNAL_H_
#define LOG_INTERNAL_H_

#include "app_util.h"
#include "nordic_common.h"
#define NRF_LOG_MODULE_NAME                             ILOG_MODULE_NAME
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"

NRF_LOG_MODULE_REGISTER();

#define LOG_INIT_INTERNAL(timestamp_func)               NRF_LOG_INIT(timestamp_func);NRF_LOG_DEFAULT_BACKENDS_INIT()
#define LOG_FLUSH_INTERNAL()                            NRF_LOG_FLUSH()

#define LOG_INTERNAL_NRF(T, ...)                        LOG_INTERNAL_NRF_X(T, NUM_VA_ARGS_LESS_1(__VA_ARGS__), __VA_ARGS__)

#define LOG_INTERNAL_NRF_X(T, N, ...)                   CONCAT_2(LOG_INTERNAL_NRF_, N) (T, __VA_ARGS__)

#define LOG_ERROR_INTERNAL(...)                         LOG_INTERNAL_NRF(LOG_ERROR, __VA_ARGS__)
#define LOG_WARNING_INTERNAL(...)                       LOG_INTERNAL_NRF(LOG_WARNING, __VA_ARGS__)
#define LOG_INFO_INTERNAL(...)                          LOG_INTERNAL_NRF(LOG_INFO, __VA_ARGS__)
#define LOG_DEBUG_INTERNAL(...)                         LOG_INTERNAL_NRF(LOG_DEBUG, __VA_ARGS__)

#define LOG_INTERNAL_NRF_0(T, s)                        CONCAT_2(NRF_, T) (s)
#define LOG_INTERNAL_NRF_1(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define LOG_INTERNAL_NRF_2(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define LOG_INTERNAL_NRF_3(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define LOG_INTERNAL_NRF_4(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define LOG_INTERNAL_NRF_5(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)
#define LOG_INTERNAL_NRF_6(T, s, ...)                   CONCAT_2(NRF_, T) (s, __VA_ARGS__)

#endif /* LOG_INTERNAL_H_ */
