/**
******************************************************************************
* @file     HAL_op.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file provides all the OP firmware functions.
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
#include "HAL_op.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @defgroup OP
* @brief OP driver modules
* @{
*/

/** @defgroup OP_Private_TypesDefinitions
* @{
*/

/**
* @}
*/

/** @defgroup OP_Private_Defines
* @{
*/


/*******************************************************************************
* @name   : OPAMP_Configure
* @brief  : Enable or Disable OPAMP
* @param  : Opx: the selected OPAMP.
            This parameter can be OPx where x can be 1 to 4
* @retval : void
*******************************************************************************/
void OPAMP_Configure(emOPAMP_OutEn Opx, FunctionalState state)
{
    (state != DISABLE) ? (OPAMP->CSR |= Opx) : (OPAMP->CSR &= ~Opx);
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
