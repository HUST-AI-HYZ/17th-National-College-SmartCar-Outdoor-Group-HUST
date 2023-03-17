/**
******************************************************************************
* @file     HAL_div.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file provides all the DIV firmware functions.
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

/* Includes ------------------------------------------------------------------*/
#include "HAL_div.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @defgroup DIV
* @brief DIV driver modules
* @{
*/

/** @defgroup DIV_Private_TypesDefinitions
* @{
*/

/**
* @}
*/

/** @defgroup DIV_Private_Defines
* @{
*/

void HWDivider_UnsignInit(void)
{
    HWDIV->DIVCON |= (DIV_UNSIGN | DIV_IRQ_ENABLE);
}

/*******************************************************************************
* @name   : HWDivider_Calc
* @brief  : Calculate HWDIV
* @param  : DVD, DVS
* @retval : HWDIV->QUOT
*******************************************************************************/
u32 HWDivider_Calc(u32 DVD, u32 DVS)
{
    HWDIV->DVD = DVD;
    HWDIV->DVS = DVS;

    //overflow
    if (HWDIV->STATUS & DIV_OVERFLOW)
    {
        return 0xffffffff;
    }
    return HWDIV->QUOT;
}

/*******************************************************************************
* @name   : Divider_Calc
* @brief  : Calculate Divider
* @param  : DVD, DVS
* @retval : DVD/DVS
*******************************************************************************/
u32 Divider_Calc(u32 DVD, u32 DVS)
{
    return DVD / DVS;
}

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

