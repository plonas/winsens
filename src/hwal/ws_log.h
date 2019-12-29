/*
 * ws_log.h
 *
 *  Created on: 29.12.2019
 *      Author: Damian Plonek
 */

#ifndef WS_LOG_H_
#define WS_LOG_H_

#ifndef WS_LOG_MODULE_NAME
    #define WS_LOG_MODULE_NAME          ""
#endif

#include "ws_log_internal.h"

#define WS_LOG_INIT(timestamp_func)     WS_LOG_INIT_INTERNAL(timestamp_func)
#define WS_LOG_FLUSH()                  WS_LOG_FLUSH_INTERNAL()

#define WS_LOG_ERROR(...)               WS_LOG_ERROR_INTERNAL(__VA_ARGS__)
#define WS_LOG_WARNING(...)             WS_LOG_WARNING_INTERNAL(__VA_ARGS__)
#define WS_LOG_INFO(...)                WS_LOG_INFO_INTERNAL(__VA_ARGS__)
#define WS_LOG_DEBUG(...)               WS_LOG_DEBUG_INTERNAL(__VA_ARGS__)

#endif /* WS_LOG_H_ */
