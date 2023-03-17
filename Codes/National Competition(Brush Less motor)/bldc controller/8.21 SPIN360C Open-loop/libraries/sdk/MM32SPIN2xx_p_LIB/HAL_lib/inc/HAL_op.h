/**
******************************************************************************
* @file  HAL_op.h
* @author  AE team
* @version  V1.1.0
* @date  09/09/2019
* @brief  This file contains all the functions prototypes for the OP
*         firmware library.
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
#ifndef __HAL_OP_H
#define __HAL_OP_H

/* Includes ------------------------------------------------------------------*/
#include "HAL_device.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @addtogroup OP
* @{
*/

/** @defgroup OP_Exported_Types
* @{
*/
#define OP_OutputLevel_High 0x01
#define OP_OutputLevel_LOW 0x00

typedef enum
{
    OPAMP1 = OPAMP_CSR_OP1, //  OPAMP1_EN,
    OPAMP2 = OPAMP_CSR_OP2,
    OPAMP3 = OPAMP_CSR_OP3,
    OPAMP4 = OPAMP_CSR_OP4
} emOPAMP_OutEn;

void OPAMP_Configure(emOPAMP_OutEn Opx, FunctionalState state);


#endif /* __HAL_OP_H */
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
