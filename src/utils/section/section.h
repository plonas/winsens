/*
 * section.h
 *
 *  Created on: 15.11.2022
 *      Author: Damian Plonek
 */

#ifndef SECTION_H_
#define SECTION_H_


#define SECTION_START_ADDR(section_name)                    SECTION_INTERNAL_START_ADDR(section_name)
#define SECTION_END_ADDR(section_name)                      SECTION_INTERNAL_END_ADDR(section_name)
#define SECTION_LENGTH(section_name)                        SECTION_INTERNAL_LENGTH(section_name)
#define SECTION_DEF(section_name, data_type)                SECTION_INTERNAL_DEF(section_name, data_type)
#define SECTION_ITEM_REGISTER(section_name, section_var)    SECTION_INTERNAL_ITEM_REGISTER(section_name, section_var)
#define SECTION_ITEM_GET(section_name, data_type, i)        SECTION_INTERNAL_ITEM_GET(section_name, data_type, i)
#define SECTION_ITEM_COUNT(section_name, data_type)         SECTION_INTERNAL_ITEM_COUNT(section_name, data_type)


#endif /* SECTION_H_ */