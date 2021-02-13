/*
 * log_internal.c
 *
 *  Created on: 24.11.2020
 *      Author: Damian Plonek
 */

#include "log.h"

/**@brief Function for error handling, which is called when an error has occurred.
 *
 * @param[in] error_code  Error code supplied to the handler.
 * @param[in] line_num    Line number where the handler is called.
 * @param[in] p_file_name Pointer to the file name.
 */
void app_error_handler(uint32_t error_code, uint32_t line_num, const uint8_t * p_file_name)
{
    if(error_code)
    {
        LOG_ERROR("Error %u in %s:%u", error_code, p_file_name, line_num);
    }
}
