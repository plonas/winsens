/*
 * command.h
 *
 *  Created on: 23.04.2021
 *      Author: Damian Plonek
 */

#ifndef COMMAND_H_
#define COMMAND_H_


struct command_struct_t;

typedef void (*command_execute_t)(struct command_struct_t const* command);

typedef struct command_struct_t
{
    command_execute_t command;
} command_t;


#endif /* COMMAND_H_ */
