/**
******************************************************************************
* @file     HAL_comp.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file provides firmware functions to manage the following
*           functionalities of the 2 analog comparators (COMP1, COMP2) peripheral:
*           + Comparators configuration
*           + Window mode control
*
@verbatim

==============================================================================
##### COMP Peripheral features #####
==============================================================================
[..]
The device integrates 5 analog comparators COMP1, COMP2, COMP3, COMP4, COMP5:
(#) The non inverting input and inverting input can be set to GPIO pins
as shown in table1. COMP Inputs below.

(#) The COMP output can be redirected to embedded timers (TIM1, TIM2, TIM3...)
Refer to table 2. COMP Outputs redirection to embedded timers below.

(#) The seven comparators have interrupt capability with wake-up
from Sleep and Stop modes (through the EXTI controller):
(++) COMP1 is internally connected to EXTI Line 21
(++) COMP2 is internally connected to EXTI Line 22
(++) COMP3 is internally connected to EXTI Line 23
(++) COMP4 is internally connected to EXTI Line 19
(++) COMP5 is internally connected to EXTI Line 20
[..] Table 1. COMP Inputs
+--------------------------------------------------------------------------+
|                 |                | COMP1 | COMP2 | COMP3 | COMP4 | COMP5 |
|-----------------|----------------|---------------|---------------|-------|
|                 | IO0            |  PB9  |  PC13 |  PC15 |  PB12 |  PB12 |
| Inverting Input | IO1            |  PB9  |  PC13 |  PC15 |  PA8  |  PA8  |
|                 | IO2            |  PB9  |  PC13 |  PC15 |  PD2  |  PA12 |
|                 | IO3            |  PB9  |  PC13 |  PC15 |  CRV  |  CRV  |
|-----------------|----------------|-------|-------|-------|-------|-------|
|  Non Inverting  | IO0            |  PB8  |  PD7  |  PC14 |  PD3  |  PA11 |
|    Input        | IO1            |  PB8  |  PD7  |  PC14 |  PA6  |  PA6  |
|                 | IO2            |  PB8  |  PD7  |  PC14 |  PB2  |  PB2  |
|                 | IO3            |  PB8  |  PD7  |  PC14 |  PB10 |  PB10 |
+--------------------------------------------------------------------------+

##### How to use this driver #####
==============================================================================
[..]
This driver provides functions to configure and program the Comparators
of all microcontroller devices.

To use the comparator, perform the following steps:

(#) Enable the SYSCFG APB clock to get write access to comparator
register using RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

(#) Configure the comparator input in analog mode using GPIO_Init()

(#) Configure the comparator output in alternate function mode
using GPIO_Init() and use GPIO_PinAFConfig() function to map the
comparator output to the GPIO pin

(#) Configure the comparator using COMP_Init() function:
(++) Select the inverting input
(++) Select the non-inverting input
(++) Select the output polarity
(++) Select the output redirection
(++) Select the hysteresis level
(++) Select the power mode

(#) Enable the comparator using COMP_Cmd() function

(#) If required enable the COMP interrupt by configuring and enabling
EXTI line in Interrupt mode and selecting the desired sensitivity
level using EXTI_Init() function. After that enable the comparator
interrupt vector using NVIC_Init() function.

@endverbatim
*
******************************************************************************
* @attention
*
* <h2><center>&copy; COPYRIGHT 2019 MindMotion</center></h2>
*
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "HAL_comp.h"

/** @addtogroup
* @{
*/

/** @defgroup COMP
* @brief COMP driver modules
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* CSR register Mask */
#define COMP_CSR_CLEAR_MASK            ((uint32_t)0xC01FBFFD)

/*!< COMPx output level */
#define COMP_CSR_COMPxOUT              ((uint32_t)0x40000000)

/*!< COMPx lock */
#define COMP_CSR_COMPxLOCK             ((uint32_t)0x80000000)

/**********************  Bit definition for COMP_CSR register  ****************/
#define COMP_CSR_COMPxEN               ((uint32_t)0x00000001) /*!< COMPx enable */
#define COMP_CSR_COMP1SW1              ((uint32_t)0x00000002) /*!< COMP1 SW1 ON&OFF control */
#define COMP_CSR_COMPxMODE             ((uint32_t)0x0000000C) /*!< COMPx power mode */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup COMP_Private_Functions
* @{
*/

/** @defgroup COMP_Group1 Initialization and Configuration functions
*  @brief   Initialization and Configuration functions
*
@verbatim
===============================================================================
##### Initialization and Configuration functions #####
===============================================================================

@endverbatim
* @{
*/

/**
* @brief  Deinitializes COMP peripheral registers to their default reset values.
* @note   Deinitialization can't be performed if the COMP configuration is locked.
*         To unlock the configuration, perform a system reset.
* @param  COMP_Selection: the selected comparator.
*          This parameter can be COMP_Selection_COMPx where x can be 1 to 7
*          to select the COMP peripheral.
* @param  None
* @retval None
*/
void COMP_DeInit(uint32_t COMP_Selection)
{
    /*!< Set COMP_CSR register to reset value */
    *(__IO uint32_t *)(COMP_BASE + COMP_Selection) = ((uint32_t)0x00000000);
}

/**
* @brief  Initializes the COMP peripheral according to the specified parameters
*         in COMP_InitStruct
* @note   If the selected comparator is locked, initialization can't be performed.
*         To unlock the configuration, perform a system reset.
* @note   By default, PA1 is selected as COMP1 non inverting input.
*         To use PA4 as COMP1 non inverting input call COMP_SwitchCmd() after COMP_Init()
* @param  COMP_Selection: the selected comparator.
*          This parameter can be COMP_Selection_COMPx where x can be 1 to 2
*          to select the COMP peripheral.
* @param  COMP_InitStruct: pointer to an COMP_InitTypeDef structure that contains
*         the configuration information for the specified COMP peripheral.
*           - COMP_InvertingInput specifies the inverting input of COMP
*           - COMP_NonInvertingInput specifies the non inverting input of COMP
*           - COMP_Output connect COMP output to selected timer
*             input (Input capture / Output Compare Reference Clear / Break Input)
*           - COMP_BlankingSrce specifies the blanking source of COMP
*           - COMP_OutputPol select output polarity
*           - COMP_Hysteresis configures COMP hysteresis value
*           - COMP_Mode configures COMP power mode
* @retval None
*/
void COMP_Init(uint32_t COMP_Selection, COMP_InitTypeDef *COMP_InitStruct)
{
    uint32_t tmpreg = 0;
    uint32_t temadd;
    /* Check the parameters */
    assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
    assert_param(IS_COMP_INVERTING_INPUT(COMP_InitStruct->COMP_InvertingInput));
    assert_param(IS_COMP_NONINVERTING_INPUT(COMP_InitStruct->COMP_NonInvertingInput));
    assert_param(IS_COMP_OUTPUT(COMP_InitStruct->COMP_Output));
    assert_param(IS_COMP_BLANKING_SOURCE(COMP_InitStruct->COMP_BlankingSrce));
    assert_param(IS_COMP_OUTPUT_POL(COMP_InitStruct->COMP_OutputPol));
    assert_param(IS_COMP_HYSTERESIS(COMP_InitStruct->COMP_Hysteresis));
    assert_param(IS_COMP_MODE(COMP_InitStruct->COMP_Mode));

    if(COMP_Selection == COMP_Selection_COMP1)
    {
        temadd = (u32) & (COMP->COMP1_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP2)
    {
        temadd = (u32) & (COMP->COMP2_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP3)
    {
        temadd = (u32) & (COMP->COMP3_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP4)
    {
        temadd = (u32) & (COMP->COMP4_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP5)
    {
        temadd = (u32) & (COMP->COMP5_CSR);
    }

    /*!< Configure COMP: inverting input, output redirection, hysteresis value and power mode */
    /*!< Set COMPxINSEL bits according to COMP_InitStruct->COMP_InvertingInput value */
    /*!< Set COMPxNONINSEL bits according to COMP_InitStruct->COMP_NonInvertingInput value */
    /*!< Set COMPxBLANKING bits according to COMP_InitStruct->COMP_BlankingSrce value */
    /*!< Set COMPxOUTSEL bits according to COMP_InitStruct->COMP_Output value */
    /*!< Set COMPxPOL bit according to COMP_InitStruct->COMP_OutputPol value */
    /*!< Set COMPxHYST bits according to COMP_InitStruct->COMP_Hysteresis value */
    /*!< Set COMPxMODE bits according to COMP_InitStruct->COMP_Mode value */
    if((COMP_Selection != COMP_Selection_COMP4) && (COMP_Selection != COMP_Selection_COMP5))
    {
        /*!< Get the COMPx_CSR register value */
        tmpreg = *(u32*)temadd;
        /*!< Clear the COMP1SW1, COMPxINSEL, COMPxOUTSEL, COMPxPOL, COMPxHYST and COMPxMODE bits */
        tmpreg &= (uint32_t) (~COMP_CSR_CLEAR_MASK);
        tmpreg |= (uint32_t)(COMP_InitStruct->COMP_Output | COMP_InitStruct->COMP_OutputPol | \
                             COMP_InitStruct->COMP_Hysteresis | COMP_InitStruct->COMP_Mode);

        *(u32*)temadd = tmpreg;
    }
    else
    {

        if(COMP_InitStruct->COMP_InvertingInput == COMP_InvertingInput_CRV)
        {
            COMP->COMP_CRV |= 1 << 4;
        }
        /*!< Get the COMPx_CSR register value */
        tmpreg = *(u32*)temadd;
        /*!< Clear the COMP1SW1, COMPxINSEL, COMPxOUTSEL, COMPxPOL, COMPxHYST and COMPxMODE bits */
        tmpreg &= (uint32_t) (~COMP_CSR_CLEAR_MASK);
        tmpreg |= (uint32_t)( COMP_InitStruct->COMP_InvertingInput | COMP_InitStruct->COMP_NonInvertingInput | \
                              COMP_InitStruct->COMP_Output | COMP_InitStruct->COMP_OutputPol |  COMP_InitStruct->COMP_Filter | \
                              COMP_InitStruct->COMP_Hysteresis | COMP_InitStruct->COMP_Mode);
        *(u32*)temadd = tmpreg;
    }
}

/**
* @brief  Fills each COMP_InitStruct member with its default value.
* @param  COMP_InitStruct: pointer to an COMP_InitTypeDef structure which will
*         be initialized.
* @retval None
*/
void COMP_StructInit(COMP_InitTypeDef* COMP_InitStruct)
{
    COMP_InitStruct->COMP_InvertingInput = COMP_InvertingInput_IO1;
    COMP_InitStruct->COMP_NonInvertingInput = COMP_NonInvertingInput_IO1;
    COMP_InitStruct->COMP_Output = COMP_Output_None;
    COMP_InitStruct->COMP_OutputPol = COMP_OutputPol_NonInverted;
    COMP_InitStruct->COMP_Hysteresis = COMP_Hysteresis_No;
    COMP_InitStruct->COMP_Mode = COMP_Mode_UltraLowPower;
    COMP_InitStruct->COMP_Filter = COMP_Filter_1_Period;
}
/**
* @brief  Enable or disable the COMP peripheral.
* @note   If the selected comparator is locked, enable/disable can't be performed.
*         To unlock the configuration, perform a system reset.
* @param  COMP_Selection: the selected comparator.
*          This parameter can be COMP_Selection_COMPx where x can be 1 to 2
*          to select the COMP peripheral.
* @param  NewState: new state of the COMP peripheral.
*         This parameter can be: ENABLE or DISABLE.
*         When enabled, the comparator compares the non inverting input with
*                       the inverting input and the comparison result is available
*                       on comparator output.
*         When disabled, the comparator doesn't perform comparison and the
*                        output level is low.
* @retval None
*/
void COMP_Cmd(uint32_t COMP_Selection, FunctionalState NewState)
{
    uint32_t temadd;
    /* Check the parameters */
    assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(COMP_Selection == COMP_Selection_COMP1)
    {
        temadd = (u32) & (COMP->COMP1_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP2)
    {
        temadd = (u32) & (COMP->COMP2_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP3)
    {
        temadd = (u32) & (COMP->COMP3_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP4)
    {
        temadd = (u32) & (COMP->COMP4_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP5)
    {
        temadd = (u32) & (COMP->COMP5_CSR);
    }

    if (NewState != DISABLE)
    {
        /* Enable the selected COMPx peripheral */
        *(__IO uint32_t *)(temadd) |= (uint32_t)(COMP_CSR_COMPxEN);
    }
    else
    {
        /* Disable the selected COMP peripheral  */
        *(__IO uint32_t *)(temadd) &= (uint32_t)(~COMP_CSR_COMPxEN);
    }
}

/**
* @brief  Select CRV param.
* @note
* @param  COMP_CRV_Sele_xx: Select source for CRV.
          COMP_CRV_Level_x_x: Set level for CRV.
* @retval None
*/
void SET_COMP_CRV(uint8_t COMP_CRV_Sele_xx, uint8_t COMP_CRV_Level_x_x)
{
    uint32_t temreg = 0;
    temreg = COMP->COMP_CRV;
    temreg &= ~COMP_CRV_MASK;
    /* Load config to CRV and enable */
    temreg |= COMP_CRV_Sele_xx | COMP_CRV_Level_x_x | (1 << 4);
    COMP->COMP_CRV = temreg;
}
/**
* @brief  Return the output level (high or low) of the selected comparator.
*         The output level depends on the selected polarity.
*         If the polarity is not inverted:
*           - Comparator output is low when the non-inverting input is at a lower
*             voltage than the inverting input
*           - Comparator output is high when the non-inverting input is at a higher
*             voltage than the inverting input
*         If the polarity is inverted:
*           - Comparator output is high when the non-inverting input is at a lower
*             voltage than the inverting input
*           - Comparator output is low when the non-inverting input is at a higher
*             voltage than the inverting input
* @param  COMP_Selection: the selected comparator.
*          This parameter can be COMP_Selection_COMPx where x can be 1 to 2
*          to select the COMP peripheral.
* @retval Returns the selected comparator output level: low or high.
*
*/
uint32_t COMP_GetOutputLevel(uint32_t COMP_Selection)
{
    uint32_t compout = 0x0;
    uint32_t temadd;
    /* Check the parameters */
    assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
    if(COMP_Selection == COMP_Selection_COMP1)
    {
        temadd = (u32) & (COMP->COMP1_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP2)
    {
        temadd = (u32) & (COMP->COMP2_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP3)
    {
        temadd = (u32) & (COMP->COMP3_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP4)
    {
        temadd = (u32) & (COMP->COMP4_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP5)
    {
        temadd = (u32) & (COMP->COMP5_CSR);
    }

    /* Check if selected comparator output is high */
    if ((*(__IO uint32_t *)(temadd) & (COMP_CSR_COMPxOUT)) != 0)
    {
        compout = COMP_OutputLevel_High;
    }
    else
    {
        compout = COMP_OutputLevel_Low;
    }

    /* Return the comparator output level */
    return (uint32_t)(compout);
}

/**
* @}
*/

/** @defgroup COMP_Group3 COMP configuration locking function
*  @brief   COMP1, COMP2 configuration locking function
*           COMP1, COMP2 configuration can be locked each separately.
*           Unlocking is performed by system reset.
*
@verbatim
===============================================================================
##### Configuration Lock function #####
===============================================================================

@endverbatim
* @{
*/

/**
* @brief  Lock the selected comparator (COMP1/COMP2) configuration.
* @note   Locking the configuration means that all control bits are read-only.
*         To unlock the comparator configuration, perform a system reset.
* @param  COMP_Selection: the selected comparator.
*          This parameter can be COMP_Selection_COMPx where x can be 1 to 7
*          to select the COMP peripheral.
* @retval None
*/
void COMP_LockConfig(uint32_t COMP_Selection)
{
    uint32_t temadd;
    /* Check the parameter */
    assert_param(IS_COMP_ALL_PERIPH(COMP_Selection));
    if (COMP_Selection == COMP_Selection_COMP1)
    {
        temadd = (u32) & (COMP->COMP1_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP2)
    {
        temadd = (u32) & (COMP->COMP2_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP3)
    {
        temadd = (u32) & (COMP->COMP3_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP4)
    {
        temadd = (u32) & (COMP->COMP4_CSR);
    }
    else if(COMP_Selection == COMP_Selection_COMP5)
    {
        temadd = (u32) & (COMP->COMP5_CSR);
    }

    /* Set the lock bit corresponding to selected comparator */
    *(__IO uint32_t *)(temadd) |= (uint32_t)(COMP_CSR_COMPxLOCK);
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
