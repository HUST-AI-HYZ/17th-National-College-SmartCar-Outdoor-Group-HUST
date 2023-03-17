/**
******************************************************************************
* @file     system_MM32SPIN2xx_p.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    CMSIS Cortex-M0 Device Peripheral Access Layer System Source File.
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

/** @addtogroup CMSIS
* @{
*/
#include "HAL_device.h"

/**
* @}
*/

/**
* @}
*/

/*!< Uncomment the line corresponding to the desired System clock (SYSCLK)
frequency (after reset the HSI is used as SYSCLK source)

IMPORTANT NOTE:
==============
1. After each device reset the HSI is used as System clock source.

2. Please make sure that the selected System clock doesn't exceed your device's
maximum frequency.

3. If none of the define below is enabled, the HSI is used as System clock
source.

4. The System clock configuration functions provided within this file assume that:
- For Low, Medium and High density Value line devices an external 8MHz
crystal is used to drive the System clock.
- For Low, Medium and High density devices an external 8MHz crystal is
used to drive the System clock.
- For Connectivity line devices an external 25MHz crystal is used to drive
the System clock.
If you are using different crystal you have to adapt those functions accordingly.
*/

//#define SYSCLK_FREQ_HSE    HSE_VALUE
//#define SYSCLK_FREQ_24MHz  24000000
//#define SYSCLK_FREQ_36MHz  36000000
//#define SYSCLK_FREQ_48MHz  48000000
//#define SYSCLK_FREQ_XXMHz  96000000

//#define SYSCLK_HSI_24MHz  24000000
//#define SYSCLK_HSI_36MHz  36000000
//#define SYSCLK_HSI_48MHz  48000000
#define SYSCLK_HSI_XXMHz  96000000

/*!< Uncomment the following line if you need to relocate your vector Table in
Internal SRAM. */
/* #define VECT_TAB_SRAM */
#define VECT_TAB_OFFSET  0x0 /*!< Vector Table base offset field. 
This value must be a multiple of 0x200. */

/**
* @}
*/

/*******************************************************************************
*  Clock Definitions
*******************************************************************************/
#ifdef SYSCLK_FREQ_HSE
uint32_t SystemCoreClock         = SYSCLK_FREQ_HSE;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_24MHz
uint32_t SystemCoreClock         = SYSCLK_FREQ_24MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_36MHz
uint32_t SystemCoreClock         = SYSCLK_FREQ_36MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_48MHz
uint32_t SystemCoreClock         = SYSCLK_FREQ_48MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_FREQ_XXMHz
uint32_t SystemCoreClock         = SYSCLK_FREQ_XXMHz;        /*!< System Clock Frequency (Core Clock) */


#elif defined SYSCLK_HSI_24MHz
uint32_t SystemCoreClock         = SYSCLK_HSI_24MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_HSI_36MHz
uint32_t SystemCoreClock         = SYSCLK_HSI_36MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_HSI_48MHz
uint32_t SystemCoreClock         = SYSCLK_HSI_48MHz;        /*!< System Clock Frequency (Core Clock) */
#elif defined SYSCLK_HSI_XXMHz
uint32_t SystemCoreClock         = SYSCLK_HSI_XXMHz;        /*!< System Clock Frequency (Core Clock) */
#else /*!< HSI Selected as System Clock source */
uint32_t SystemCoreClock         = HSI_VALUE;        /*!< System Clock Frequency (Core Clock) */
#endif

__I uint8_t AHBPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
/**
* @}
*/

static void SetSysClock(void);

#ifdef SYSCLK_FREQ_HSE
static void SetSysClockToHSE(void);
#elif defined SYSCLK_FREQ_24MHz
static void SetSysClockTo24(void);
#elif defined SYSCLK_FREQ_36MHz
static void SetSysClockTo36(void);
#elif defined SYSCLK_FREQ_48MHz
static void SetSysClockTo48(void);
#elif defined SYSCLK_FREQ_XXMHz
static void SetSysClockToXX(void);

#elif defined SYSCLK_HSI_24MHz
static void SetSysClockTo24_HSI(void);
#elif defined SYSCLK_HSI_36MHz
static void SetSysClockTo36_HSI(void);
#elif defined SYSCLK_HSI_48MHz
static void SetSysClockTo48_HSI(void);
#elif defined SYSCLK_HSI_XXMHz
static void SetSysClockToXX_HSI(void);
#endif

#ifdef DATA_IN_ExtSRAM
static void SystemInit_ExtMemCtl(void);
#endif /* DATA_IN_ExtSRAM */

/**
* @}
*/

/**
* @brief  Setup the microcontroller system
*         Initialize the Embedded Flash Interface, the PLL and update the
*         SystemCoreClock variable.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
void SystemInit (void)
{
    /* Reset the RCC clock configuration to the default reset state(for debug purpose) */
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset SW, HPRE, PPRE1, PPRE2, ADCPRE and MCO bits */
    RCC->CFGR &= (uint32_t)0xF0FF0000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Reset PLLSRC, PLLXTPRE, PLLMUL and USBPRE/OTGFSPRE bits */
    RCC->CFGR &= (uint32_t)0xFF80FFFF;
    RCC->CR &= (uint32_t)0x000FFFFF;

    /* Disable all interrupts and clear pending bits  */
    RCC->CIR = 0x009F0000;
    extern void uart_init(unsigned long baudrate);
    uart_init(460800);
    /* Configure the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers */
    /* Configure the Flash Latency cycles and enable prefetch buffer */
    SetSysClock();
}

/**
* @brief  use to return the pllm&plln.
* @param  pllclkSourceFrq : PLL source clock frquency;
          pllclkFrq : Target PLL clock frquency;
		  plln : PLL factor PLLN
		  pllm : PLL factor PLLM
* @retval amount of error
*/
u32 AutoCalPllFactor(u32 pllclkSourceFrq, u32 pllclkFrq, u8 *plln, u8* pllm)
{
    u32 n, m;
    u32 tempFrq;
    u32 minDiff = pllclkFrq;
    u8  flag = 0;
    for(m = 0; m < 4 ; m++)
    {
        for(n = 0; n < 64 ; n++)
        {
            tempFrq =  pllclkSourceFrq * (n + 1) / (m + 1);
            tempFrq = (tempFrq >  pllclkFrq) ? (tempFrq - pllclkFrq) : (pllclkFrq - tempFrq) ;

            if(minDiff > tempFrq)
            {
                minDiff =  tempFrq;
                *plln = n;
                *pllm = m;
            }
            if(minDiff == 0)
            {
                flag = 1;
                break;
            }
        }
        if(flag != 0)
        {
            break;
        }
    }
    return  minDiff;
}
/**
* @brief  Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
* @param  None
* @retval None
*/
static void SetSysClock(void)
{
#ifdef SYSCLK_FREQ_HSE
    SetSysClockToHSE();
#elif defined SYSCLK_FREQ_24MHz
    SetSysClockTo24();
#elif defined SYSCLK_FREQ_36MHz
    SetSysClockTo36();
#elif defined SYSCLK_FREQ_48MHz
    SetSysClockTo48();
#elif defined SYSCLK_FREQ_XXMHz
    SetSysClockToXX();

#elif defined SYSCLK_HSI_24MHz
    SetSysClockTo24_HSI();
#elif defined SYSCLK_HSI_36MHz
    SetSysClockTo36_HSI();
#elif defined SYSCLK_HSI_48MHz
    SetSysClockTo48_HSI();
#elif defined SYSCLK_HSI_XXMHz
    SetSysClockToXX_HSI();
#endif

    /* If none of the define above is enabled, the HSI is used as System clock
    source (default after reset) */
}

#ifdef SYSCLK_FREQ_HSE
/**
* @brief  Selects HSE as System clock source and configure HCLK, PCLK2
*          and PCLK1 prescalers.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
static void SetSysClockToHSE(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    int i;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
        i = 1000; while(i--);
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 0 wait state ,bit0~2*/
        FLASH->ACR &= ~0x07;
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /* Select HSE as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSE;

        /* Wait till HSE is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x04)
        {
        }
    }
    else
    {
        /* If HSE fails to start-up, the application will have wrong clock
          configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_24MHz
/**
* @brief  Sets System clock frequency to 24MHz and configure HCLK, PCLK2
*          and PCLK1 prescalers.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
static void SetSysClockTo24(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    int i;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
        i = 1000; while(i--);
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 0 wait state ,bit0~2*/
        FLASH->ACR &= ~0x07;
        FLASH->ACR |= 0x01;
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /*  PLL configuration:  = (HSE ) * (2+1) = 24 MHz */
        RCC->CFGR &= (uint32_t)0xFFFCFFFF;
        RCC->CR &= (uint32_t)0x000FFFFF;

        RCC->CFGR |= (uint32_t)RCC_CFGR_PLLSRC ;
        RCC->CR |= 0x08000000;//pll=3/1

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /* If HSE fails to start-up, the application will have wrong clock
          configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_36MHz
/**
* @brief  Sets System clock frequency to 36MHz and configure HCLK, PCLK2
*          and PCLK1 prescalers.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
static void SetSysClockTo36(void)
{
    int i;
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;

    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
        i = 1000; while(i--);
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;

        /* Flash 0 wait state ,bit0~2*/
        FLASH->ACR &= ~0x07;
        FLASH->ACR |= 0x01;
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV1;

        /*  PLL configuration:  = (HSE ) * (8+1)/(1+1) = 36 MHz */
        RCC->CFGR &= (uint32_t)0xFFFCFFFF;
        RCC->CR &= (uint32_t)0x000FFFFF;

        RCC->CFGR |= (uint32_t)RCC_CFGR_PLLSRC ;
        RCC->CR |= 0x20100000;//pll = 9/2

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /* If HSE fails to start-up, the application will have wrong clock
          configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_48MHz
/**
* @brief  Sets System clock frequency to 48MHz and configure HCLK, PCLK2
*          and PCLK1 prescalers.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
static void SetSysClockTo48(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    int i;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
        i = 1000; while(i--);
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 0 wait state ,bit0~2*/
        FLASH->ACR &= ~0x07;
        FLASH->ACR |= 0x02;
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;

        /*  PLL configuration:  = (HSE ) * (5+1) = 48MHz */
        RCC->CFGR &= (uint32_t)0xFFFCFFFF;
        RCC->CR &= (uint32_t)0x000FFFFF;

        RCC->CFGR |= (uint32_t ) RCC_CFGR_PLLSRC ;
        RCC->CR |= 0x14000000;//pll = 6/1

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /* If HSE fails to start-up, the application will have wrong clock
          configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_FREQ_XXMHz
/**
* @brief  Sets System clock frequency to XXMHz and configure HCLK, PCLK2
*          and PCLK1 prescalers.
* @note   This function should be used only after reset.
* @param  None
* @retval None
*/
static void SetSysClockToXX(void)
{
    __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
    int i;
    uint32_t temp;
    uint8_t plln, pllm;
    /* SYSCLK, HCLK, PCLK2 and PCLK1 configuration ---------------------------*/
    /* Enable HSE */
    RCC->CR |= ((uint32_t)RCC_CR_HSEON);

    /* Wait till HSE is ready and if Time out is reached exit */
    do
    {
        HSEStatus = RCC->CR & RCC_CR_HSERDY;
        StartUpCounter++;
    }
    while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

    if ((RCC->CR & RCC_CR_HSERDY) != RESET)
    {
        HSEStatus = (uint32_t)0x01;
        i = 1000; while(i--);
    }
    else
    {
        HSEStatus = (uint32_t)0x00;
    }

    if (HSEStatus == (uint32_t)0x01)
    {
        SystemCoreClock         = SYSCLK_FREQ_XXMHz;
        /* Enable Prefetch Buffer */
        FLASH->ACR |= FLASH_ACR_PRFTBE;
        /* Flash 0 wait state ,bit0~2*/
        FLASH->ACR &= ~0x07;
        temp = (SystemCoreClock - 1) / 24000000;
        FLASH->ACR |= temp;

#if 1
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;
#else
        /* HCLK = SYSCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV2;
#endif
        /* PCLK2 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

        if(SystemCoreClock > 36000000)
        {
            /* PCLK1 = HCLK/2 */
            RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
        }
        else
        {
            /* PCLK1 = HCLK */
            RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
        }

        RCC->CFGR &= (uint32_t)0xFFFCFFFF;
        RCC->CR &= (uint32_t)0x000FFFFF;

        RCC->CFGR |= (uint32_t ) RCC_CFGR_PLLSRC ;

        AutoCalPllFactor(8000000, SystemCoreClock, &plln, &pllm);
        RCC->CR &= (uint32_t)0x000FFFFF;
        RCC->CR |= (plln << 26) | (pllm << 20);

        /* Enable PLL */
        RCC->CR |= RCC_CR_PLLON;

        /* Wait till PLL is ready */
        while((RCC->CR & RCC_CR_PLLRDY) == 0)
        {
        }

        /* Select PLL as system clock source */
        RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
        RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

        /* Wait till PLL is used as system clock source */
        while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
        {
        }
    }
    else
    {
        /* If HSE fails to start-up, the application will have wrong clock
          configuration. User can add here some code to deal with this error */
    }
}
#elif defined SYSCLK_HSI_24MHz
void SetSysClockTo24_HSI(void)
{
    unsigned char temp = 0;

    RCC->CR |= RCC_CR_HSION;

    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = RCC_CFGR_PPRE1_2;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;

    RCC->CR &= ~(RCC_CR_PLLON);

    RCC->CR &= ~(0x1f << 26);

    RCC->CR |= (2 - 1) << 26;

    FLASH->ACR = FLASH_ACR_PRFTBE;

    RCC->CR |= RCC_CR_PLLON;

    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;

    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while(temp != 0x02)
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}

#elif defined SYSCLK_HSI_36MHz
void SetSysClockTo36_HSI(void)
{
    unsigned char temp = 0;

    RCC->CR |= RCC_CR_HSION;

    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = RCC_CFGR_PPRE1_2;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;

    RCC->CR &= ~(RCC_CR_PLLON);

    RCC->CR &= ~(0x1f << 26);

    RCC->CR |= (3 - 1) << 26;

    FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE;

    RCC->CR |= RCC_CR_PLLON;

    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~ RCC_CFGR_SW;

    RCC->CFGR |= RCC_CFGR_SW_PLL; /

    while(temp != 0x02)
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}

#elif defined SYSCLK_HSI_48MHz
void SetSysClockTo48_HSI(void)
{
    unsigned char temp = 0;

    RCC->CR |= RCC_CR_HSION;

    while(!(RCC->CR & RCC_CR_HSIRDY));

    RCC->CFGR = RCC_CFGR_PPRE1_2;

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;

    RCC->CR &= ~(RCC_CR_PLLON);

    RCC->CR &= ~(0x1f << 26);

    RCC->CR |= (4 - 1) << 26;

    FLASH->ACR = FLASH_ACR_LATENCY_1 | FLASH_ACR_PRFTBE;

    RCC->CR |= RCC_CR_PLLON;

    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~RCC_CFGR_SW;

    RCC->CFGR |= RCC_CFGR_SW_PLL;

    while(temp != 0x02)
    {
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
}
#elif defined SYSCLK_HSI_XXMHz
void SetSysClockToXX_HSI(void)
{
    uint32_t temp;
    uint8_t plln, pllm;

    RCC->CR |= RCC_CR_HSION;

    while(!(RCC->CR & RCC_CR_HSIRDY));

    SystemCoreClock         = SYSCLK_HSI_XXMHz;
    /* Enable Prefetch Buffer */
    FLASH->ACR |= FLASH_ACR_PRFTBE;
    /* Flash 0 wait state ,bit0~2*/
    FLASH->ACR &= ~0x07;

    temp = (SystemCoreClock - 1) / 24000000;

    FLASH->ACR |= temp;

    /* HCLK = SYSCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_HPRE_DIV1;

    /* PCLK2 = HCLK */
    RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE2_DIV1;

    if(SystemCoreClock > 36000000)
    {
        /* PCLK1 = HCLK/2 */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    }
    else
    {
        /* PCLK1 = HCLK */
        RCC->CFGR |= (uint32_t)RCC_CFGR_PPRE1_DIV2;
    }

    RCC->CFGR &= (uint32_t)0xFFFCFFFF;

    RCC->CR &= (uint32_t)0x000FFFFF;

    AutoCalPllFactor(12000000, SystemCoreClock, &plln, &pllm);

    RCC->CR &= (uint32_t)0x000FFFFF;

    RCC->CR |= (plln << 26) | (pllm << 20);

    /* Enable PLL */
    RCC->CR |= RCC_CR_PLLON;

    /* Wait till PLL is ready */
    while((RCC->CR & RCC_CR_PLLRDY) == 0)
    {
    }
    /* Select PLL as system clock source */
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));

    RCC->CFGR |= (uint32_t)RCC_CFGR_SW_PLL;

    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != (uint32_t)0x08)
    {
    }
}

#endif

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

