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

#define WS_LOG_ERROR_CHECK(s)           do{ if (WINSENS_OK != s) { WS_LOG_ERROR_INTERNAL(#s "(%u) != WINSENS_OK\r\n", s) } }while(0)
#define WS_LOG_WARNING_CHECK(s)         do{ if (WINSENS_OK != s) { WS_LOG_WARNING_INTERNAL(#s "(%u) != WINSENS_OK\r\n", s) } }while(0)
#define WS_LOG_INFO_CHECK(s)            do{ if (WINSENS_OK != s) { WS_LOG_INFO_INTERNAL(#s "(%u) != WINSENS_OK\r\n", s) } }while(0)
#define WS_LOG_DEBUG_CHECK(s)           do{ if (WINSENS_OK != s) { WS_LOG_DEBUG_INTERNAL(#s "(%u) != WINSENS_OK\r\n", s) } }while(0)

#endif /* WS_LOG_H_ */
