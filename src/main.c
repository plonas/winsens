/*
 * main.c
 *
 *  Created on: 30.12.2017
 *      Author: Damian.Plonek
 */

#include "winsens.h"

int main(void)
{
    WINSENS_Status_e status = WINSENS_ERROR;

    status = WINSENS_Init();
    if (WINSENS_OK != status) return -1;

    status = WINSENS_Loop();

    WINSENS_Deinit();
    return 0;
}
