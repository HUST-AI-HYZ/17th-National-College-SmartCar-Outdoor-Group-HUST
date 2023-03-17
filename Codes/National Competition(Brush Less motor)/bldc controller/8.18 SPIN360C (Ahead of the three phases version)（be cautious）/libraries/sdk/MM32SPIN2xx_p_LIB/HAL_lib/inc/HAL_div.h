/**
******************************************************************************
* @file     HAL_div.h
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file contains all the functions prototypes for the DIV
*           firmware library.
******************************************************************************
* @copy
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
* TIME. AS A RESULT, MindMotion SHALL NOT BE HELD LIABLE FOR ANY
* DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
* FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
* CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __HAL_DIV_H
#define __HAL_DIV_H

/* Includes ------------------------------------------------------------------*/
#include "HAL_device.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @addtogroup DIV
* @{
*/

/** @defgroup DIV_Exported_Types
* @{
*/

#define DIV_UNSIGN 0x01

#define DIV_IRQ_ENABLE 0x02

#define DIV_OVERFLOW 0x01

#define SET_HWDivider(x, y) \
    HWDIV->DVD = x;         \
    HWDIV->DVS = y;

#define GET_HWDivider HWDIV->QUOT;



void HWDivider_UnsignInit(void);
u32 Divider_Calc(u32 DVD, u32 DVS);
u32 HWDivider_Calc(u32 DVD, u32 DVS);

#endif /* __HAL_DIV_H */

/**
* @}
*/

/**
* @}
*/

/**
* @}
*/

/*-------------------------(C) COPYRIGHT 2019 MindMotion ----------------------*/
