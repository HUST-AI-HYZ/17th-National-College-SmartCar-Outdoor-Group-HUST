/*
 * Copyright 2021 MindMotion Microelectronics Co., Ltd.
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include "hal_device_registers.h"

void SystemInit(void)
{
    extern void clock_init (uint32_t clock);
    clock_init(96000000);
}

/* EOF. */
