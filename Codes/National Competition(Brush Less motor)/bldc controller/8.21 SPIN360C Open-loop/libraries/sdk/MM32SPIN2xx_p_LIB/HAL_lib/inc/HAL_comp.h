/**
******************************************************************************
* @file     HAL_comp.h
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file contains all the functions prototypes for the COMP firmware
*           library.
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
#ifndef __HAL_COMP_H
#define __HAL_COMP_H

/* Includes ------------------------------------------------------------------*/
#include "HAL_device.h"

/** @addtogroup
  * @{
  */

/** @addtogroup COMP
  * @{
  */

/* Exported types ------------------------------------------------------------*/

/**
* @brief  COMP Init structure definition
*/

typedef struct
{

    uint32_t COMP_InvertingInput;     /*!< Selects the inverting input of the comparator.
    This parameter can be a value of @ref COMP_InvertingInput */

    uint32_t COMP_NonInvertingInput;  /*!< Selects the non inverting input of the comparator.
    This parameter can be a value of @ref COMP_NonInvertingInput */

    uint32_t COMP_Output;             /*!< Selects the output redirection of the comparator.
    This parameter can be a value of @ref COMP_Output */

    uint32_t COMP_OutputPol;           /*!< Selects the output polarity of the comparator.
    This parameter can be a value of @ref COMP_OutputPoloarity */

    uint32_t COMP_Hysteresis;         /*!< Selects the hysteresis voltage of the comparator.
    This parameter can be a value of @ref COMP_Hysteresis */

    uint32_t COMP_Mode;               /*!< Selects the operating mode of the comparator
    and allows to adjust the speed/consumption.
    This parameter can be a value of @ref COMP_Mode */

    uint32_t COMP_Filter;

} COMP_InitTypeDef;

typedef struct
{

    FunctionalState COMP_Poll_En;     /*!< Selects the inverting input of the comparator.
    This parameter can be a value of @ref COMP_Poll_En */

    uint32_t COMP_Poll_Ch;  /*!< Selects the non inverting input of the comparator.
    This parameter can be a value of @ref COMP_NonInvertingInput */

    uint32_t COMP_Poll_Fixn;             /*!< Selects the output redirection of the comparator.
    This parameter can be a value of @ref COMP_Poll_Fixn */

    uint32_t COMP_Poll_Period;           /*!< Selects the output polarity of the comparator.
    This parameter can be a value of @ref COMP_Poll_Period */

    uint32_t COMP_Poll_Pout;         /*!< Selects the hysteresis voltage of the comparator.
    This parameter can be a value of @ref COMP_Poll_Pout */

} COMP_POLL_InitTypeDef;

/* Exported constants --------------------------------------------------------*/

/** @defgroup COMP_Exported_Constants
* @{
*/

/** @defgroup COMP_Selection
* @{
*/

#define COMP_Selection_COMP1                    ((uint32_t)0x00000000) /*!< COMP1 Selection */
#define COMP_Selection_COMP2                    ((uint32_t)0x00000004) /*!< COMP2 Selection */
#define COMP_Selection_COMP3                    ((uint32_t)0x00000008) /*!< COMP3 Selection */
#define COMP_Selection_COMP4                    ((uint32_t)0x0000000C) /*!< COMP4 Selection */
#define COMP_Selection_COMP5                    ((uint32_t)0x00000010) /*!< COMP5 Selection */


#define IS_COMP_ALL_PERIPH(PERIPH) (((PERIPH) == COMP_Selection_COMP1) || \
                                    ((PERIPH) == COMP_Selection_COMP2) ||((PERIPH) == COMP_Selection_COMP3) || \
                                    ((PERIPH) == COMP_Selection_COMP4) || ((PERIPH) == COMP_Selection_COMP5))

/**
* @}
*/

/** @defgroup COMP_Poll_Ch
* @{
*/
#define COMP_Poll_Ch_1_2                        ((uint32_t)0x00000000)
#define COMP_Poll_Ch_1_2_3                      ((uint32_t)0x00000002)

/**
* @}
*/
/** @defgroup COMP_InvertingInput
* @{
*/

#define COMP_InvertingInput_IO0                 ((uint32_t)0x00000000)
#define COMP_InvertingInput_IO1                 ((uint32_t)0x00000010)
#define COMP_InvertingInput_IO2                 ((uint32_t)0x00000020)
#define COMP_InvertingInput_IO3                 ((uint32_t)0x00000030)
#define COMP_InvertingInput_CRV                 ((uint32_t)0x00000030)

#define IS_COMP_INVERTING_INPUT(INPUT) (((INPUT) == COMP_InvertingInput_IO0) || \
                                        ((INPUT) == COMP_InvertingInput_IO1) || \
                                        ((INPUT) == COMP_InvertingInput_IO2) || \
                                        ((INPUT) == COMP_InvertingInput_IO3)    || \
                                        ((INPUT) == COMP_InvertingInput_CRV))
/**
* @}
*/

/** @defgroup COMP_NonInvertingInput
* @{
*/
#define COMP_NonInvertingInput_IO0                 ((uint32_t)0x00000000)
#define COMP_NonInvertingInput_IO1                 ((uint32_t)0x00000080)
#define COMP_NonInvertingInput_IO2                 ((uint32_t)0x00000100)
#define COMP_NonInvertingInput_IO3                 ((uint32_t)0x00000180)

#define IS_COMP_NONINVERTING_INPUT(INPUT)     (((INPUT) == COMP_NonInvertingInput_IO1)  || \
        ((INPUT) == COMP_NonInvertingInput_IO1)  || \
        ((INPUT) == COMP_NonInvertingInput_IO2)  || \
        ((INPUT) == COMP_NonInvertingInput_IO3))

/**
* @}
*/

/** @defgroup COMP_Output
* @{
*/

#define COMP_Output_None                  ((uint32_t)0x00000000)   /*!< COMP output isn't connected to other peripherals */

/* Output Redirection common for all comparators COMP1...COMP5 */
#define COMP_Output_TIM1BKIN              ((uint32_t)0x00000800) /*!< COMP output connected to TIM1 Brake Input (BKIN) */
#define COMP_Output_TIM8BKIN              ((uint32_t)0x00000C00) /*!< COMP output connected to TIM8 Brake Input (BKIN) */
#define COMP_Output_TIM1OCREFCLR          ((uint32_t)0x00001800)   /*!< COMP output connected to TIM1 OCREF Clear */
#define COMP_Output_TIM1IC1               ((uint32_t)0x00001C00)   /*!< COMP output connected to TIM1 Input Capture 1 */
#define COMP_Output_TIM2IC4               ((uint32_t)0x00002000)   /*!< COMP output connected to TIM2 Input Capture 4 */
#define COMP_Output_TIM2OCREFCLR          ((uint32_t)0x00002400)   /*!< COMP output connected to TIM2 OCREF Clear */
#define COMP_Output_TIM3IC1               ((uint32_t)0x00002800)   /*!< COMP output connected to TIM3 Input Capture 1 */
#define COMP_Output_TIM3OCREFCLR          ((uint32_t)0x00002C00)   /*!< COMP output connected to TIM3 OCREF Clear */
#define COMP_Output_TIM8OCREFCLR          ((uint32_t)0x00003C00)   /*!< COMP output connected to TIM8 OCREF Clear */


#define IS_COMP_OUTPUT(OUTPUT) 		(((OUTPUT) == COMP_Output_None)  || \
                                     ((OUTPUT) == COMP_Output_TIM8BKIN)  || \
                                     ((OUTPUT) == COMP_Output_TIM1IC1)  || \
                                     ((OUTPUT) == COMP_Output_TIM1OCREFCLR)  || \
                                     ((OUTPUT) == COMP_Output_TIM1IC1)  || \
                                     ((OUTPUT) == COMP_Output_TIM2IC4)  || \
                                     ((OUTPUT) == COMP_Output_TIM2OCREFCLR)  || \
                                     ((OUTPUT) == COMP_Output_TIM3IC1)  || \
                                     ((OUTPUT) == COMP_Output_TIM3OCREFCLR)  || \
                                     ((OUTPUT) == COMP_Output_TIM8OCREFCLR))

#define COMP_CRV_MASK                       ((uint32_t)0x0000003F)

/** @defgroup COMP_CRV_Sele_xx
* @{
*/
#define  COMP_CRV_Sele_Vref1_2              ((uint32_t)0x00000000)
#define  COMP_CRV_Sele_AVDD                 ((uint32_t)0x00000020)
/**
* @}
*/

/** @defgroup COMP_CRV_Level_x_x
* @{
*/
#define  COMP_CRV_Level_1_20                ((uint32_t)0x00000000)
#define  COMP_CRV_Level_2_20                ((uint32_t)0x00000001)
#define  COMP_CRV_Level_3_20                ((uint32_t)0x00000002)
#define  COMP_CRV_Level_4_20                ((uint32_t)0x00000003)
#define  COMP_CRV_Level_5_20                ((uint32_t)0x00000004)
#define  COMP_CRV_Level_6_20                ((uint32_t)0x00000005)
#define  COMP_CRV_Level_7_20                ((uint32_t)0x00000006)
#define  COMP_CRV_Level_8_20                ((uint32_t)0x00000007)
#define  COMP_CRV_Level_9_20                ((uint32_t)0x00000008)
#define  COMP_CRV_Level_10_20               ((uint32_t)0x00000009)
#define  COMP_CRV_Level_11_20               ((uint32_t)0x0000000A)
#define  COMP_CRV_Level_12_20               ((uint32_t)0x0000000B)
#define  COMP_CRV_Level_13_20               ((uint32_t)0x0000000C)
#define  COMP_CRV_Level_14_20               ((uint32_t)0x0000000D)
#define  COMP_CRV_Level_15_20               ((uint32_t)0x0000000E)
#define  COMP_CRV_Level_16_20               ((uint32_t)0x0000000F)
/**
* @}
*/

/** @defgroup COMP_OutputPoloarity
* @{
*/
#define COMP_OutputPol_NonInverted          ((uint32_t)0x00000000)  /*!< COMP output on GPIO isn't inverted */
#define COMP_OutputPol_Inverted             ((uint32_t)0x00008000)  /*!< COMP output on GPIO is inverted */

#define IS_COMP_OUTPUT_POL(POL) (((POL) == COMP_OutputPol_NonInverted)  || \
                                 ((POL) == COMP_OutputPol_Inverted))

/**
* @}
*/

/** @defgroup COMP_Hysteresis
* @{
*/
/* Please refer to the electrical characteristics in the device datasheet for
the hysteresis level */
#define COMP_Hysteresis_No                         ((uint32_t)0x00000000) /*!< No hysteresis */
#define COMP_Hysteresis_Low                        ((uint32_t)0x00010000) /*!< Hysteresis level low */
#define COMP_Hysteresis_Medium                     ((uint32_t)0x00020000) /*!< Hysteresis level medium */
#define COMP_Hysteresis_High                       ((uint32_t)0x00030000) /*!< Hysteresis level high */

#define IS_COMP_HYSTERESIS(HYSTERESIS)    (((HYSTERESIS) == COMP_Hysteresis_No)     || \
        ((HYSTERESIS) == COMP_Hysteresis_Low)    || \
        ((HYSTERESIS) == COMP_Hysteresis_Medium) || \
        ((HYSTERESIS) == COMP_Hysteresis_High))
/**
* @}
*/

/** @defgroup COMP_Mode
* @{
*/
/* Please refer to the electrical characteristics in the device datasheet for
the power consumption values */
#define COMP_Mode_HighSpeed                     ((uint32_t)0x0000000C)  /*!< High Speed */
#define COMP_Mode_MediumSpeed                   ((uint32_t)0x00000008)  /*!< Medium Speed */
#define COMP_Mode_LowPower                      ((uint32_t)0x00000004)  /*!< Low power mode */
#define COMP_Mode_UltraLowPower                 ((uint32_t)0x00000000)  /*!< Ultra-low power mode */

#define IS_COMP_MODE(MODE)    (((MODE) == COMP_Mode_UltraLowPower) || \
                               ((MODE) == COMP_Mode_LowPower)      || \
                               ((MODE) == COMP_Mode_MediumSpeed)   || \
                               ((MODE) == COMP_Mode_HighSpeed))
/**
* @}
*/
/** @defgroup COMP_Filter
* @{
*/
#define COMP_Filter_1_Period                        ((uint32_t)0x00000000)
#define COMP_Filter_4_Period                        ((uint32_t)0x00040000)
#define COMP_Filter_16_Period                       ((uint32_t)0x00080000)
#define COMP_Filter_32_Period                       ((uint32_t)0x000C0000)
#define COMP_Filter_64_Period                       ((uint32_t)0x00100000)
#define COMP_Filter_128_Period                      ((uint32_t)0x00140000)
#define COMP_Filter_256_Period                      ((uint32_t)0x00180000)
#define COMP_Filter_512_Period                      ((uint32_t)0x001C0000)
/** @defgroup COMP_OutputLevel
* @{
*/
/* When output polarity is not inverted, comparator output is high when
the non-inverting input is at a higher voltage than the inverting input */
#define COMP_OutputLevel_High                   ((uint32_t)0x40000000)
/* When output polarity is not inverted, comparator output is low when
the non-inverting input is at a lower voltage than the inverting input*/
#define COMP_OutputLevel_Low                    ((uint32_t)0x00000000)

/**
* @}
*/

/** @defgroup COMP_WindowMode
* @{
*/
#define IS_COMP_WINDOW(WINDOW)  (((WINDOW) == COMP_Selection_COMP2) || \
                                 ((WINDOW) == COMP_Selection_COMP4) || \
                                 ((WINDOW) == COMP_Selection_COMP6))
/**
* @}
*/

/**
* @}
*/

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/*  Function used to set the COMP configuration to the default reset state ****/
void COMP_DeInit(uint32_t COMP_Selection);

/* Initialization and Configuration functions *********************************/
void COMP_Init(uint32_t COMP_Selection, COMP_InitTypeDef *COMP_InitStruct);
void COMP_StructInit(COMP_InitTypeDef *COMP_InitStruct);
void COMP_Cmd(uint32_t COMP_Selection, FunctionalState NewState);
void COMP_SwitchCmd(uint32_t COMP_Selection, FunctionalState NewState);
void SET_COMP_CRV(uint8_t COMP_CRV_Sele_xx, uint8_t COMP_CRV_Level_x_x);
uint32_t COMP_GetOutputLevel(uint32_t COMP_Selection);

/* COMP configuration locking function ****************************************/
void COMP_LockConfig(uint32_t COMP_Selection);

#endif /*__HAL_COMP_H */
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
