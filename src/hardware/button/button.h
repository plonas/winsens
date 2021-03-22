/*
 * button.h
 *
 *  Created on: 25.02.2020
 *      Author: Damian Plonek
 */

#ifndef BUTTON_H_
#define BUTTON_H_

#include "digital_io.h"


typedef enum {
    BUTTON_EVENT_NORMAL = (WINSENS_IDS_MODULE_BUTTON << 16), // 100ms
    BUTTON_EVENT_LONG, // 1500ms
    BUTTON_EVENT_VERY_LONG // 2500ms

} button_event_t;

typedef uint8_t button_id_t;

typedef struct
{
    digital_io_input_pin_t  pin;
    bool                    active_low;
} button_cfg_t;

winsens_status_t button_init(void);

winsens_status_t button_register_callback(
        button_id_t btn,
    winsens_event_handler_t eventHandler);
void button_unregister_callback(
        button_id_t btn);

#endif /* BUTTON_H_ */
