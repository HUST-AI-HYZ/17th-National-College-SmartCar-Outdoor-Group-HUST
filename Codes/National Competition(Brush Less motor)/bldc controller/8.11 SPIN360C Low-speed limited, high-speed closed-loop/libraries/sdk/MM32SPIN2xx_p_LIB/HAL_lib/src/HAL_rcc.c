/**
******************************************************************************
* @file     HAL_rcc.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file provides all the RCC firmware functions.
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
#include "HAL_rcc.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @defgroup RCC
* @brief RCC driver modules
* @{
*/

/** @defgroup RCC_Private_TypesDefinitions
* @{
*/

/**
* @}
*/

/** @defgroup RCC_Private_Defines
* @{
*/

/* ------------ RCC registers bit address in the alias region ----------- */
#define RCC_OFFSET                (RCC_BASE - PERIPH_BASE)

/* --- CR Register ---*/

/* Alias word address of HSION bit */
#define CR_OFFSET                 (RCC_OFFSET + 0x00)
#define HSION_BitNumber           0x00
#define CR_HSION_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (HSION_BitNumber * 4))

/* Alias word address of PLLON bit */
#define PLLON_BitNumber           0x18
#define CR_PLLON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (PLLON_BitNumber * 4))

/* Alias word address of CSSON bit */
#define CSSON_BitNumber           0x13
#define CR_CSSON_BB               (PERIPH_BB_BASE + (CR_OFFSET * 32) + (CSSON_BitNumber * 4))

/* --- CFGR Register ---*/

/* Alias word address of USBPRE bit */
#define CFGR_OFFSET               (RCC_OFFSET + 0x04)
#define USBPRE_BitNumber          0x16
#define CFGR_USBPRE_BB            (PERIPH_BB_BASE + (CFGR_OFFSET * 32) + (USBPRE_BitNumber * 4))

/* --- BDCR_CS Register ---*/

/* Alias word address of RTCEN bit */
#define BDCR_CS_OFFSET               (RCC_OFFSET + 0x20)
#define RTCEN_BitNumber           0x0F
#define BDCR_CS_RTCEN_BB             (PERIPH_BB_BASE + (BDCR_CS_OFFSET * 32) + (RTCEN_BitNumber * 4))

/* Alias word address of BDRST bit */
#define BDRST_BitNumber           0x10
#define BDCR_CS_BDRST_BB             (PERIPH_BB_BASE + (BDCR_CS_OFFSET * 32) + (BDRST_BitNumber * 4))

/* --- CSR Register ---*/

/* Alias word address of LSION bit */
#define CSR_OFFSET                (RCC_OFFSET + 0x24)
#define LSION_BitNumber           0x00
#define CSR_LSION_BB              (PERIPH_BB_BASE + (CSR_OFFSET * 32) + (LSION_BitNumber * 4))

/* ---------------------- RCC registers bit mask ------------------------ */

/* CR register bit mask */
#define CR_HSEBYP_Reset           ((uint32_t)0xFFFBFFFF)
#define CR_HSEBYP_Set             ((uint32_t)0x00040000)
#define CR_HSEON_Reset            ((uint32_t)0xFFFEFFFF)
#define CR_HSEON_Set              ((uint32_t)0x00010000)
#define CR_HSITRIM_Mask           ((uint32_t)0xFFFFFF07)

/* CFGR register bit mask */
#define CFGR_PLL_Mask             ((uint32_t)0xFFC0FFFF)
#define CFGR_PLLMull_Mask         ((uint32_t)0x003C0000)
#define CFGR_PLLSRC_Mask          ((uint32_t)0x00010000)
#define CFGR_PLLXTPRE_Mask        ((uint32_t)0x00020000)
#define CFGR_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)
#define CFGR_HPRE_Set_Mask        ((uint32_t)0x000000F0)
#define CFGR_PPRE1_Reset_Mask     ((uint32_t)0xFFFFF8FF)
#define CFGR_PPRE1_Set_Mask       ((uint32_t)0x00000700)
#define CFGR_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)
#define CFGR_PPRE2_Set_Mask       ((uint32_t)0x00003800)

/* CSR register bit mask */
#define CSR_RMVF_Set              ((uint32_t)0x01000000)

/* RCC Flag Mask */
#define FLAG_Mask                 ((uint8_t)0x1F)

/* Typical Value of the HSI in Hz */
#define HSI_Value                 ((uint32_t)8000000)

/* CIR register byte 2 (Bits[15:8]) base address */
#define CIR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* CIR register byte 3 (Bits[23:16]) base address */
#define CIR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR register byte 4 (Bits[31:24]) base address */
#define CFGR_BYTE4_ADDRESS        ((uint32_t)0x40021007)

/* BDCR_CS register base address */
#define BDCR_CS_ADDRESS              (PERIPH_BASE + BDCR_CS_OFFSET)

#ifndef HSEStartUp_TimeOut
/* Time out for HSE start up */
#define HSEStartUp_TimeOut        ((uint16_t)0x0500)
#endif

/**
* @}
*/

/** @defgroup RCC_Private_Macros
* @{
*/

/**
* @}
*/

/** @defgroup RCC_Private_Variables
* @{
*/

static __I uint8_t APBAHBPrescTable[16] = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};

/**
* @}
*/

/** @defgroup RCC_Private_FunctionPrototypes
* @{
*/

/**
* @}
*/

/** @defgroup RCC_Private_Functions
* @{
*/

/**
* @brief  Resets the RCC clock configuration to the default reset state.
* @param  None
* @retval : None
*/
void RCC_DeInit(void)
{
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;
    /* Reset SW[1:0], HPRE[3:0], PPRE1[2:0], PPRE2[2:0], ADCPRE[1:0] and MCO[2:0] bits */
    RCC->CFGR &= (uint32_t)0xF8FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;
    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;
    /* Reset PLLSRC, PLLXTPRE, PLLMUL[3:0] and USBPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;
    /* Disable all interrupts */
    RCC->CIR = 0x00000000;
}

/**
* @brief  Configures the External High Speed oscillator (HSE).
*   HSE can not be stopped if it is used directly or through the
*   PLL as system clock.
* @param RCC_HSE: specifies the new state of the HSE.
*   This parameter can be one of the following values:
* @arg RCC_HSE_OFF: HSE oscillator OFF
* @arg RCC_HSE_ON: HSE oscillator ON
* @arg RCC_HSE_Bypass: HSE oscillator bypassed with external
*   clock
* @retval : None
*/
void RCC_HSEConfig(uint32_t RCC_HSE)
{
    /* Check the parameters */
    assert_param(IS_RCC_HSE(RCC_HSE));
    /* Reset HSEON and HSEBYP bits before configuring the HSE ------------------*/
    /* Reset HSEON bit */
    RCC->CR &= CR_HSEON_Reset;
    /* Reset HSEBYP bit */
    RCC->CR &= CR_HSEBYP_Reset;
    /* Configure HSE (RCC_HSE_OFF is already covered by the code section above) */
    if ( RCC_HSE_ON == RCC_HSE )
    {
        /* Set HSEON bit */
        RCC->CR |= CR_HSEON_Set;
    }
    else if ( RCC_HSE_Bypass == RCC_HSE )
    {
        /* Set HSEBYP and HSEON bits */
        RCC->CR |= CR_HSEBYP_Set | CR_HSEON_Set;
    }
    else
    {
    }
}

/**
* @brief  Waits for HSE start-up.
* @param  None
* @retval : An ErrorStatus enumuration value:
* - SUCCESS: HSE oscillator is stable and ready to use
* - ERROR: HSE oscillator not yet ready
*/
ErrorStatus RCC_WaitForHSEStartUp(void)
{
    __IO uint32_t StartUpCounter = 0;
    ErrorStatus status = ERROR;
    FlagStatus HSEStatus = RESET;

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
        StartUpCounter++;
    }
    while((HSEStatus == RESET) && (StartUpCounter != HSEStartUp_TimeOut));
    if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
    {
        status = SUCCESS;
    }
    else
    {
        status = ERROR;
    }
    return (status);
}

/**
* @brief  Adjusts the Internal High Speed oscillator (HSI) calibration
*   value.
* @param HSICalibrationValue: specifies the calibration trimming value.
*   This parameter must be a number between 0 and 0x1F.
* @retval : None
*/
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_CALIBRATION_VALUE(HSICalibrationValue));
    tmpreg = RCC->CR;
    /* Clear HSITRIM[4:0] bits */
    tmpreg &= CR_HSITRIM_Mask;
    /* Set the HSITRIM[4:0] bits according to HSICalibrationValue value */
    tmpreg |= (uint32_t)HSICalibrationValue << 3;
    /* Store the new value */
    RCC->CR = tmpreg;
}

/**
* @brief  Enables or disables the Internal High Speed oscillator (HSI).
*   HSI can not be stopped if it is used directly or through the
*   PLL as system clock.
* @param NewState: new state of the HSI.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_HSICmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if(NewState == ENABLE)
    {
        RCC->CR |= 0x01;
    }
    else
    {
        RCC->CR &= 0xfffffffe;
    }
}
/**
* @brief  Configures the PLL clock source and DM DN factor.
*   This function must be used only when the PLL is disabled.
* @param RCC_PLLSource: specifies the PLL entry clock source.
*   This parameter can be one of the following values:
* @arg RCC_PLLSource_HSI_Div2: HSI oscillator clock divided
*   by 2 selected as PLL clock entry
* @arg RCC_PLLSource_HSE_Div1: HSE oscillator clock selected
*   as PLL clock entry
* @arg RCC_PLLSource_HSE_Div2: HSE oscillator clock divided
*   by 2 selected as PLL clock entry
* @param RCC_PLLDN: specifies the PLL multiplication factor.
*   This parameter can be RCC_PLLMul_x where x:[31:26]
* @param RCC_PLLDM: specifies the PLL Divsior factor.
*   This parameter can be RCC_Divsior_x where x:[22:20]
* @retval : None
*/
void RCC_PLLDMDNConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLDN, uint32_t RCC_PLLDM)
{
    uint32_t tmpreg0 = 0;

    /* Check the parameters */
    assert_param(IS_RCC_PLL_SOURCE(RCC_PLLSource));
    assert_param(IS_RCC_PLL_MUL(RCC_PLLMul));
    tmpreg0 = RCC->CR;

    /* Clear PLLDN, PLLDM bits */
    /* Clear PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
    tmpreg0 &= 0x038fffff;

    /* Set the PLL configuration bits */
    tmpreg0 |= (RCC_PLLDN << 26) | (RCC_PLLDM << 20);

    RCC->CR = tmpreg0;
}


/**
* @brief  Configures the PLL clock source and multiplication factor.
*   This function must be used only when the PLL is disabled.
* @param RCC_PLLSource: specifies the PLL entry clock source.
*   This parameter can be one of the following values:
* @arg RCC_PLLSource_HSI_Div2: HSI oscillator clock divided
*   by 2 selected as PLL clock entry
* @arg RCC_PLLSource_HSE_Div1: HSE oscillator clock selected
*   as PLL clock entry
* @arg RCC_PLLSource_HSE_Div2: HSE oscillator clock divided
*   by 2 selected as PLL clock entry
* @param RCC_PLLMul: specifies the PLL multiplication factor.
*   This parameter can be RCC_PLLMul_x where x:[31:26][22:20]
* @retval : None
*/
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLMul)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_PLL_SOURCE(RCC_PLLSource));
    assert_param(IS_RCC_PLL_MUL(RCC_PLLMul));
    tmpreg = RCC->CFGR;
    /* Clear PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
    tmpreg &= CFGR_PLL_Mask;
    /* Set the PLL configuration bits */
    tmpreg |= RCC_PLLSource;
    /* Store the new value */
    RCC->CFGR = tmpreg;


    if(RCC_PLLMul == RCC_PLLMul_2)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000007, 0x00000003); //Frclk*8/4
    }
    if(RCC_PLLMul == RCC_PLLMul_3)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000005, 0x00000001);//Frclk*6/2
    }
    if(RCC_PLLMul == RCC_PLLMul_4)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000007, 0x00000001);//Frclk*8/2
    }
    if(RCC_PLLMul == RCC_PLLMul_5)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000009, 0x00000001);//Frclk*10/2
    }
    if(RCC_PLLMul == RCC_PLLMul_6)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x0000000B, 0x00000001);//Frclk*12/2
    }
    if(RCC_PLLMul == RCC_PLLMul_7)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x0000000D, 0x00000001);//Frclk*14/2
    }
    if(RCC_PLLMul == RCC_PLLMul_8)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x0000000F, 0x00000001);//Frclk*16/2
    }
    if(RCC_PLLMul == RCC_PLLMul_9)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000011, 0x00000001);//Frclk*18/2
    }
    if(RCC_PLLMul == RCC_PLLMul_10)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000013, 0x00000001);//Frclk*20/2
    }
    if(RCC_PLLMul == RCC_PLLMul_11)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000015, 0x00000001);//Frclk*22/2
    }
    if(RCC_PLLMul == RCC_PLLMul_12)
    {
        RCC_PLLDMDNConfig(RCC_PLLSource, 0x00000017, 0x00000001);//Frclk*24/2
    }
}


/**
* @brief  Enables or disables the PLL.
*   The PLL can not be disabled if it is used as system clock.
* @param NewState: new state of the PLL.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_PLLCmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        RCC->CR |= 0x01000000;
    }
    else
    {
        RCC->CR &= 0xfeffffff;
    }
}

/**
* @brief  Configures the system clock (SYSCLK).
* @param RCC_SYSCLKSource: specifies the clock source used as system
*   clock. This parameter can be one of the following values:
* @arg RCC_SYSCLKSource_HSI: HSI selected as system clock
* @arg RCC_SYSCLKSource_HSE: HSE selected as system clock
* @arg RCC_SYSCLKSource_PLLCLK: PLL selected as system clock
* @retval : None
*/
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_SYSCLK_SOURCE(RCC_SYSCLKSource));
    tmpreg = RCC->CFGR;
    /* Clear SW[1:0] bits */
    tmpreg &= CFGR_SW_Mask;
    /* Set SW[1:0] bits according to RCC_SYSCLKSource value */
    tmpreg |= RCC_SYSCLKSource;
    /* Store the new value */
    RCC->CFGR = tmpreg;
}

/**
* @brief  Returns the clock source used as system clock.
* @param  None
* @retval : The clock source used as system clock. The returned value can
*   be one of the following:
* - 0x00: HSI/6 used as system clock
* - 0x04: HSE used as system clock
* - 0x08: PLL used as system clock
*/
uint8_t RCC_GetSYSCLKSource(void)
{
    return ((uint8_t)(RCC->CFGR & CFGR_SWS_Mask));
}

/**
* @brief  Configures the AHB clock (HCLK).
* @param RCC_SYSCLK: defines the AHB clock divider. This clock is derived from
*                    the system clock (SYSCLK).
*   This parameter can be one of the following values:
* @arg RCC_SYSCLK_Div1: AHB clock = SYSCLK
* @arg RCC_SYSCLK_Div2: AHB clock = SYSCLK/2
* @arg RCC_SYSCLK_Div4: AHB clock = SYSCLK/4
* @arg RCC_SYSCLK_Div8: AHB clock = SYSCLK/8
* @arg RCC_SYSCLK_Div16: AHB clock = SYSCLK/16
* @arg RCC_SYSCLK_Div64: AHB clock = SYSCLK/64
* @arg RCC_SYSCLK_Div128: AHB clock = SYSCLK/128
* @arg RCC_SYSCLK_Div256: AHB clock = SYSCLK/256
* @arg RCC_SYSCLK_Div512: AHB clock = SYSCLK/512
* @retval : None
*/
void RCC_HCLKConfig(uint32_t RCC_SYSCLK)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_HCLK(RCC_SYSCLK));
    tmpreg = RCC->CFGR;
    /* Clear HPRE[3:0] bits */
    tmpreg &= CFGR_HPRE_Reset_Mask;
    /* Set HPRE[3:0] bits according to RCC_SYSCLK value */
    tmpreg |= RCC_SYSCLK;
    /* Store the new value */
    RCC->CFGR = tmpreg;
}

/**
* @brief  Configures the Low Speed APB clock (PCLK1).
* @param RCC_HCLK: defines the APB1 clock divider. This clock is derived from
*                  the AHB clock (HCLK).
*   This parameter can be one of the following values:
* @arg RCC_HCLK_Div1: APB1 clock = HCLK
* @arg RCC_HCLK_Div2: APB1 clock = HCLK/2
* @arg RCC_HCLK_Div4: APB1 clock = HCLK/4
* @arg RCC_HCLK_Div8: APB1 clock = HCLK/8
* @arg RCC_HCLK_Div16: APB1 clock = HCLK/16
* @retval : None
*/
void RCC_PCLK1Config(uint32_t RCC_HCLK)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_PCLK(RCC_HCLK));
    tmpreg = RCC->CFGR;
    /* Clear PPRE1[2:0] bits */
    tmpreg &= CFGR_PPRE1_Reset_Mask;
    /* Set PPRE1[2:0] bits according to RCC_HCLK value */
    tmpreg |= RCC_HCLK;
    /* Store the new value */
    RCC->CFGR = tmpreg;
}

/**
* @brief  Configures the High Speed APB clock (PCLK2).
* @param RCC_HCLK: defines the APB2 clock divider. This clock is derived from
*                  the AHB clock (HCLK).
*   This parameter can be one of the following values:
* @arg RCC_HCLK_Div1: APB2 clock = HCLK
* @arg RCC_HCLK_Div2: APB2 clock = HCLK/2
* @arg RCC_HCLK_Div4: APB2 clock = HCLK/4
* @arg RCC_HCLK_Div8: APB2 clock = HCLK/8
* @arg RCC_HCLK_Div16: APB2 clock = HCLK/16
* @retval : None
*/
void RCC_PCLK2Config(uint32_t RCC_HCLK)
{
    uint32_t tmpreg = 0;
    /* Check the parameters */
    assert_param(IS_RCC_PCLK(RCC_HCLK));
    tmpreg = RCC->CFGR;
    /* Clear PPRE2[2:0] bits */
    tmpreg &= CFGR_PPRE2_Reset_Mask;
    /* Set PPRE2[2:0] bits according to RCC_HCLK value */
    tmpreg |= RCC_HCLK << 3;
    /* Store the new value */
    RCC->CFGR = tmpreg;
}

/**
* @brief  Enables or disables the specified RCC interrupts.
* @param RCC_IT: specifies the RCC interrupt sources to be enabled or disabled.
*   This parameter can be any combination of the following values:
* @arg RCC_IT_LSIRDY: LSI ready interrupt
* @arg RCC_IT_LSERDY: LSE ready interrupt
* @arg RCC_IT_HSIRDY: HSI ready interrupt
* @arg RCC_IT_HSERDY: HSE ready interrupt
* @arg RCC_IT_PLLRDY: PLL ready interrupt
* @param NewState: new state of the specified RCC interrupts.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_IT(RCC_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        /* Perform Byte access to RCC_CIR[12:8] bits to enable the selected interrupts */

        RCC->CIR |= ((uint32_t)RCC_IT) << 8;
    }
    else
    {
        /* Perform Byte access to RCC_CIR[12:8] bits to disable the selected interrupts */

        RCC->CIR &= ~((uint32_t)RCC_IT << 8);
    }
}

/**
* @brief  Enables or disables the Internal Low Speed oscillator (LSI).
*   LSI can not be disabled if the IWDG is running.
* @param NewState: new state of the LSI.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_LSICmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        RCC->CSR |= 0x00000001;
    }
    else
    {
        RCC->CSR &= 0xfffffffe;
    }
}


/**
* @brief  Returns the frequencies of different on chip clocks.
* @param RCC_Clocks: pointer to a RCC_ClocksTypeDef structure which
*   will hold the clocks frequencies.
* @retval : None
*/
void RCC_GetClocksFreq(RCC_ClocksTypeDef* RCC_Clocks)
{
    uint32_t tmp = 0, pllmull1 = 0, pllmull2 = 0, pllsource = 0, presc = 0;
    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & CFGR_SWS_Mask;

    if ( 0x04 == tmp )
    {
        /* HSE used as system clock */
        RCC_Clocks->SYSCLK_Frequency = HSE_Value;
    }
    else if ( 0x08 == tmp )
    {
        /* PLL used as system clock */
        /* Get PLL clock source and multiplication factor ----------------------*/

        pllmull1 = ((RCC->CR & 0xfc000000) >> 26) + 1;
        pllmull2 = ((RCC->CR & 0x00700000) >> 20) + 1;
        pllsource = RCC->CFGR & CFGR_PLLSRC_Mask;
        if (pllsource == 0x00)
        {
            /* HSI oscillator clock divided by 2 selected as PLL clock entry */
            RCC_Clocks->SYSCLK_Frequency = 2 * (HSI_Value_Pll_ON >> 1) * pllmull1 / pllmull2;
        }
        else
        {
            /* HSE selected as PLL clock entry */
            if ((RCC->CFGR & CFGR_PLLXTPRE_Mask) != (uint32_t)RESET)
            {
                /* HSE oscillator clock divided by 2 */
                RCC_Clocks->SYSCLK_Frequency = (HSE_Value >> 1) * pllmull1 / pllmull2;
            }
            else
            {
                RCC_Clocks->SYSCLK_Frequency = HSE_Value * pllmull1 / pllmull2;
            }
        }
    }
    else if ( 0x0C == tmp )
    {
        /* LSI used as system clock */
        RCC_Clocks->SYSCLK_Frequency = 40000;
    }
    else
    {
        RCC_Clocks->SYSCLK_Frequency = HSI_Value_Pll_OFF;
    }
    /* Compute HCLK, PCLK1, PCLK2 and ADCCLK clocks frequencies ----------------*/
    /* Get HCLK prescaler */
    tmp = RCC->CFGR & CFGR_HPRE_Set_Mask;
    tmp = tmp >> 4;
    presc = APBAHBPrescTable[tmp];
    /* HCLK clock frequency */
    RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc;
    /* Get PCLK1 prescaler */
    tmp = RCC->CFGR & CFGR_PPRE1_Set_Mask;
    tmp = tmp >> 8;
    presc = APBAHBPrescTable[tmp];
    /* PCLK1 clock frequency */
    RCC_Clocks->PCLK1_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
    /* Get PCLK2 prescaler */
    tmp = RCC->CFGR & CFGR_PPRE2_Set_Mask;
    tmp = tmp >> 11;
    presc = APBAHBPrescTable[tmp];
    /* PCLK2 clock frequency */
    RCC_Clocks->PCLK2_Frequency = RCC_Clocks->HCLK_Frequency >> presc;
}

/**
* @brief  Enables or disables the AHB peripheral clock.
* @param RCC_AHBPeriph: specifies the AHB peripheral to gates its clock.
*   This parameter can be any combination of the following values:
* @arg RCC_AHBPeriph_DMA1
* @arg RCC_AHBPeriph_SRAM
* @arg RCC_AHBPeriph_FLITF
* @arg RCC_AHBPeriph_CRC
* @arg RCC_AHBPeriph_GPIOA
* @arg RCC_AHBPeriph_GPIOB
* @arg RCC_AHBPeriph_GPIOC
* @arg RCC_AHBPeriph_GPIOD
* @arg RCC_AHBPeriph_HWROOT
* @arg RCC_AHBPeriph_HWDIV
*   SRAM and FLITF clock can be disabled only during sleep mode.
* @param NewState: new state of the specified peripheral clock.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/

void RCC_AHBPeriphClockCmd(uint32_t RCC_AHBPeriph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_AHB_PERIPH(RCC_AHBPeriph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        RCC->AHBENR |= RCC_AHBPeriph;
    }
    else
    {
        RCC->AHBENR &= ~RCC_AHBPeriph;
    }
}

/**
* @brief  Enables or disables the High Speed APB (APB2) peripheral clock.
* @param RCC_APB2Periph: specifies the APB2 peripheral to gates its
*   clock.
*   This parameter can be any combination of the following values:
* @arg RCC_APB2Periph_SYSCFG, RCC_APB2Periph_ADC1,
*   RCC_APB2Periph_ADC2, RCC_APB2Periph_TIM1, RCC_APB2Periph_SPI1,
*   RCC_APB2Periph_TIM8, RCC_APB2Periph_UART1,RCC_APB2Periph_CPT,
*   RCC_APB2Periph_TIM14, RCC_APB2Periph_TIM16, RCC_APB2Periph_TIM17,
*   RCC_APB2Periph_DBGMCU, RCC_APB2Periph_PWM, RCC_APB2Periph_ALL
* @param NewState: new state of the specified peripheral clock.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_APB2PeriphClockCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        RCC->APB2ENR |= RCC_APB2Periph;
    }
    else
    {
        RCC->APB2ENR &= ~RCC_APB2Periph;
    }
}

/**
* @brief  Enables or disables the Low Speed APB (APB1) peripheral clock.
* @param RCC_APB1Periph: specifies the APB1 peripheral to gates its
*   clock.
*   This parameter can be any combination of the following values:
* @arg RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3,
*   RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2,
*   RCC_APB1Periph_UART2, RCC_APB1Periph_UART3, RCC_APB1Periph_UART4,
*   RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1,
*   RCC_APB1Periph_PWR, RCC_APB1Periph_ALL
* @param NewState: new state of the specified peripheral clock.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_APB1PeriphClockCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        RCC->APB1ENR |= RCC_APB1Periph;
    }
    else
    {
        RCC->APB1ENR &= ~RCC_APB1Periph;
    }
}

/**
* @brief  Forces or releases High Speed APB (APB2) peripheral reset.
* @param RCC_APB2Periph: specifies the APB2 peripheral to reset.
*   This parameter can be any combination of the following values:
* @arg RCC_APB2Periph_SYSCFG, RCC_APB2Periph_ADC1,
*   RCC_APB2Periph_ADC2, RCC_APB2Periph_TIM1, RCC_APB2Periph_SPI1,
*   RCC_APB2Periph_TIM8, RCC_APB2Periph_UART1,RCC_APB2Periph_CPT,
*   RCC_APB2Periph_TIM14, RCC_APB2Periph_TIM16, RCC_APB2Periph_TIM17,
*   RCC_APB2Periph_DBGMCU, RCC_APB2Periph_PWM, RCC_APB2Periph_ALL
* @param NewState: new state of the specified peripheral reset.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_APB2PeriphResetCmd(uint32_t RCC_APB2Periph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_APB2_PERIPH(RCC_APB2Periph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        RCC->APB2RSTR |= RCC_APB2Periph;
    }
    else
    {
        RCC->APB2RSTR &= ~RCC_APB2Periph;
    }
}

/**
* @brief  Forces or releases Low Speed APB (APB1) peripheral reset.
* @param RCC_APB1Periph: specifies the APB1 peripheral to reset.
*   This parameter can be any combination of the following values:
* @arg RCC_APB1Periph_TIM2, RCC_APB1Periph_TIM3,
*   RCC_APB1Periph_WWDG, RCC_APB1Periph_SPI2,
*   RCC_APB1Periph_UART2, RCC_APB1Periph_UART3, RCC_APB1Periph_UART4,
*   RCC_APB1Periph_UART5, RCC_APB1Periph_I2C1,
*   RCC_APB1Periph_PWR, RCC_APB1Periph_ALL
* @param NewState: new state of the specified peripheral clock.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_APB1PeriphResetCmd(uint32_t RCC_APB1Periph, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_RCC_APB1_PERIPH(RCC_APB1Periph));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        RCC->APB1RSTR |= RCC_APB1Periph;
    }
    else
    {
        RCC->APB1RSTR &= ~RCC_APB1Periph;
    }
}

/**
* @brief  Enables or disables the Clock Security System.
* @param NewState: new state of the Clock Security System..
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState == ENABLE)
    {
        RCC->CR |= (uint32_t)0x00080000;
    }
    else
    {
        RCC->CR &= (uint32_t)0xfff7ffff;
    }
}

/**
* @brief  Selects the clock source to output on MCO pin.
* @param RCC_MCO: specifies the clock source to output.
*   This parameter can be one of the following values:
* @arg RCC_MCO_NoClock: No clock selected
* @arg RCC_MCO_SYSCLK: System clock selected
* @arg RCC_MCO_HSI: HSI oscillator clock selected
* @arg RCC_MCO_HSE: HSE oscillator clock selected
* @arg RCC_MCO_PLLCLK_Div2: PLL clock divided by 2 selected
* @retval : None
*/
void RCC_MCOConfig(uint8_t RCC_MCO)
{
    /* Check the parameters */
    assert_param(IS_RCC_MCO(RCC_MCO));
    /* Perform Byte access to MCO[2:0] bits to select the MCO source */
    RCC->CFGR &= 0xF8FFFFFF;
    RCC->CFGR |= (RCC_MCO << 24);
}

/**
* @brief  Checks whether the specified RCC flag is set or not.
* @param RCC_FLAG: specifies the flag to check.
*   This parameter can be one of the following values:
* @arg RCC_FLAG_HSIRDY: HSI oscillator clock ready
* @arg RCC_FLAG_HSERDY: HSE oscillator clock ready
* @arg RCC_FLAG_PLLRDY: PLL clock ready
* @arg RCC_FLAG_LSERDY: LSE oscillator clock ready
* @arg RCC_FLAG_LSIRDY: LSI oscillator clock ready
* @arg RCC_FLAG_PINRST: Pin reset
* @arg RCC_FLAG_PORRST: POR/PDR reset
* @arg RCC_FLAG_SFTRST: Software reset
* @arg RCC_FLAG_IWDGRST: Independent Watchdog reset
* @arg RCC_FLAG_WWDGRST: Window Watchdog reset
* @arg RCC_FLAG_LPWRRST: Low Power reset
* @retval : The new state of RCC_FLAG (SET or RESET).
*/
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
    uint32_t tmp = 0;
    uint32_t statusreg = 0;
    FlagStatus bitstatus = RESET;
    /* Check the parameters */
    assert_param(IS_RCC_FLAG(RCC_FLAG));
    /* Get the RCC register index */
    tmp = RCC_FLAG >> 5;
    if (tmp == 1)               /* The flag to check is in CR register */
    {
        statusreg = RCC->CR;
    }
    else if (tmp == 2)          /* The flag to check is in BDCR_CS register */
    {

    }
    else                       /* The flag to check is in CSR register */
    {
        statusreg = RCC->CSR;
    }
    /* Get the flag position */
    tmp = RCC_FLAG & FLAG_Mask;
    if ((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    /* Return the flag status */
    return bitstatus;
}

/**
* @brief  Clears the RCC reset flags.
*   The reset flags are: RCC_FLAG_PINRST, RCC_FLAG_PORRST,
*   RCC_FLAG_SFTRST, RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST,
*   RCC_FLAG_LPWRRST
* @param  None
* @retval : None
*/
void RCC_ClearFlag(void)
{
    /* Set RMVF bit to clear the reset flags */
    RCC->CSR |= CSR_RMVF_Set;
}

/**
* @brief  Checks whether the specified RCC interrupt has occurred or not.
* @param RCC_IT: specifies the RCC interrupt source to check.
*   This parameter can be one of the following values:
* @arg RCC_IT_LSIRDY: LSI ready interrupt
* @arg RCC_IT_LSERDY: LSE ready interrupt
* @arg RCC_IT_HSIRDY: HSI ready interrupt
* @arg RCC_IT_HSERDY: HSE ready interrupt
* @arg RCC_IT_PLLRDY: PLL ready interrupt
* @arg RCC_IT_CSS: Clock Security System interrupt
* @retval : The new state of RCC_IT (SET or RESET).
*/
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
    ITStatus bitstatus = RESET;
    /* Check the parameters */
    assert_param(IS_RCC_GET_IT(RCC_IT));
    /* Check the status of the specified RCC interrupt */
    if ((RCC->CIR & RCC_IT) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    /* Return the RCC_IT status */
    return  bitstatus;
}

/**
* @brief  Clears the RCC’s interrupt pending bits.
* @param RCC_IT: specifies the interrupt pending bit to clear.
*   This parameter can be any combination of the following values:
* @arg RCC_IT_LSIRDY: LSI ready interrupt
* @arg RCC_IT_LSERDY: LSE ready interrupt
* @arg RCC_IT_HSIRDY: HSI ready interrupt
* @arg RCC_IT_HSERDY: HSE ready interrupt
* @arg RCC_IT_PLLRDY: PLL ready interrupt
* @arg RCC_IT_CSS: Clock Security System interrupt
* @retval : None
*/
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
    /* Check the parameters */
    assert_param(IS_RCC_CLEAR_IT(RCC_IT));
    /* Perform Byte access to RCC_CIR[23:16] bits to clear the selected interrupt
    pending bits */

    RCC->CIR |= (uint32_t)RCC_IT << 16;
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
