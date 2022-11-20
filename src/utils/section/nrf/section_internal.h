/*
 * section_internal.h
 *
 *  Created on: 15.11.2022
 *      Author: Damian Plonek
 */

#ifndef SECTION_INTERNAL_H_
#define SECTION_INTERNAL_H_


#define SECTION_INTERNAL_START_ADDR(section_name)                   NRF_SECTION_START_ADDR(section_name)
#define SECTION_INTERNAL_END_ADDR(section_name)                     NRF_SECTION_END_ADDR(section_name)
#define SECTION_INTERNAL_LENGTH(section_name)                       NRF_SECTION_LENGTH(section_name)
#define SECTION_INTERNAL_DEF(section_name, data_type)               NRF_SECTION_DEF(section_name, data_type)
#define SECTION_INTERNAL_ITEM_REGISTER(section_name, section_var)   NRF_SECTION_ITEM_REGISTER(section_name, section_var)
#define SECTION_INTERNAL_ITEM_GET(section_name, data_type, i)       NRF_SECTION_ITEM_GET(section_name, data_type, i)
#define SECTION_INTERNAL_ITEM_COUNT(section_name, data_type)        NRF_SECTION_ITEM_COUNT(section_name, data_type)


#endif /* SECTION_INTERNAL_H_ */