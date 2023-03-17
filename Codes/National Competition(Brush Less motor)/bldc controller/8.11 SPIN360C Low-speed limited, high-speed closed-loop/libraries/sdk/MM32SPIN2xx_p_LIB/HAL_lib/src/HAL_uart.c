/**
******************************************************************************
* @file     HAL_uart.c
* @author   AE team
* @version  V1.1.0
* @date     09/09/2019
* @brief    This file provides all the UART firmware functions.
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
#include "HAL_uart.h"
#include "HAL_rcc.h"

/** @addtogroup StdPeriph_Driver
* @{
*/

/** @defgroup UART
* @brief UART driver modules
* @{
*/

/** @defgroup UART_Private_TypesDefinitions
* @{
*/

/**
* @}
*/

/** @defgroup UART_Private_Defines
* @{
*/



/* UART UE Mask */
#define GCR_UE_Set                  ((uint16_t)0x0001)  /* UART Enable Mask */
#define GCR_UE_Reset                ((uint16_t)0xFFFE)  /* UART Disable Mask */

#define CCR_CLEAR_Mask       		((uint32_t)0xFFFFFE30)  /* UART CCR Mask */
#define GCR_CLEAR_Mask       		((uint32_t)0xFFFFFFE0)  /* UART GCR Mask */

#define CCR_WAKE_Mask               ((uint16_t)0xDFFF)	/* UART CCR Mask */

#define CCR_RWU_Set                 ((uint16_t)0x1000)  /* UART mute mode Enable Mask */
#define CCR_RWU_Reset               ((uint16_t)0xEFFF)  /* UART mute mode Enable Mask */

#define SCR_HDSEL_Set               ((uint16_t)0x1000)  /* UART Half-Duplex Enable Mask */
#define SCR_HDSEL_Reset             ((uint16_t)0xEFFF)  /* UART Half-Duplex Disable Mask */

#define RX_Address_Mask			    ((uint16_t)0xFF00)	/* UART RXADDR Mask */

#define CCR_9bit_Enable             ((uint16_t)0x0800)  /* UART CCR 9bit Enable */
#define CCR_9bit_Disable            ((uint16_t)0xF7FF)  /* UART CCR 9bit Enable */

#define CCR_9bit_Set			    ((uint16_t)0x0100)  /* UART CCR 9bit set */
#define CCR_9bit_Reset			    ((uint16_t)0xFEFF)

#define CCR_B8TOG_Set       	    ((uint16_t)0x0400)	/* UART 9bit Automatic_Flip */
#define CCR_B8TOG_Reset 		    ((uint16_t)0xFBFF)

#define SCR_SCEN_Set                ((uint16_t)0x0001)  /* UART SC Enable Mask */
#define SCR_SCEN_Reset              ((uint16_t)0xFFFE)  /* UART SC Disable Mask */

#define SCR_NACK_Set                ((uint16_t)0x0002)  /* UART SC Enable Mask */
#define SCR_NACK_Reset              ((uint16_t)0xFFFD)  /* UART SC Disable Mask */

#define SCR_SCFCNT_Mask             ((uint16_t)0xF00F)  /* UART SCR Register  SCFCNT Mask */

#define CCR_BRK_Set                 ((uint16_t)0x0008)  /* UART Break frame send Mask */
/**
* @}
*/

/** @defgroup UART_Private_Macros
* @{
*/

/**
* @}
*/

/** @defgroup UART_Private_Variables
* @{
*/

/**
* @}
*/

/** @defgroup UART_Private_FunctionPrototypes
* @{
*/

/**
* @}
*/

/** @defgroup UART_Private_Functions
* @{
*/

/**
* @brief  Deinitializes the UARTx peripheral registers to their
*   default reset values.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @retval : None
*/
void UART_DeInit(UART_TypeDef *UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
#ifdef UART2
    if(UART2 == UARTx)
    {
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART2, ENABLE);
        RCC_APB1PeriphResetCmd(RCC_APB1Periph_UART2, DISABLE);
    }
#endif
#ifdef UART1
    if(UART1 == UARTx)
    {
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_UART1, ENABLE);
        RCC_APB2PeriphResetCmd(RCC_APB2Periph_UART1, DISABLE);
    }
#endif
}

/**
* @brief  Initializes the UARTx peripheral according to the specified
*   parameters in the UART_InitStruct .
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param UART_InitStruct: pointer to a UART_InitTypeDef structure
*   that contains the configuration information for the
*   specified UART peripheral.
* @retval : None
*/
void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct)
{
    uint32_t tmpreg = 0x00, apbclock = 0x00;
    uint32_t tmpreg1 = 0x00;
    uint32_t UARTxbase = 0;
    RCC_ClocksTypeDef RCC_ClocksStatus;
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_BAUDRATE(UART_InitStruct->UART_BaudRate));
    assert_param(IS_UART_WORD_LENGTH(UART_InitStruct->UART_WordLength));
    assert_param(IS_UART_STOPBITS(UART_InitStruct->UART_StopBits));
    assert_param(IS_UART_PARITY(UART_InitStruct->UART_Parity));
    assert_param(IS_UART_MODE(UART_InitStruct->UART_Mode));
    assert_param(IS_UART_HARDWARE_FLOW_CONTROL(UART_InitStruct->UART_HardwareFlowControl));

    UARTxbase = (*(uint32_t *)&UARTx);
    /*---------------------------- UART CCR Configuration -----------------------*/
    /* get UART CCR values */
    tmpreg = UARTx->CCR;
    /* Clear spb,psel,pen bits */
    tmpreg &= CCR_CLEAR_Mask;
    /* Configure the UART Word Length,the UART Stop Bits,Parity ------------*/
    /* Set the char bits according to UART_WordLength value */
    /* Set spb bit according to UART_StopBits value */
    /* Set PEN bit according to UART_Parity value */
    tmpreg |= (uint32_t)UART_InitStruct->UART_WordLength | (uint32_t)UART_InitStruct->UART_StopBits | UART_InitStruct->UART_Parity;

    /* Write to UART CCR */
    UARTx->CCR = tmpreg;

    /*---------------------------- UART GCR Configuration -----------------------*/
    /* get UART GCR values */
    tmpreg = UARTx->GCR;
    /* Clear TXEN and RXEN ,autoflowen, mode ,uarten bits */
    tmpreg &= GCR_CLEAR_Mask;
    /* Set autorlowen bit according to UART_HardwareFlowControl value */
    /* Set rxen,txen bits according to UART_Mode value */
    tmpreg |= UART_InitStruct->UART_HardwareFlowControl | UART_InitStruct->UART_Mode;
    /* Write to UART GCR */
    UARTx->GCR = tmpreg;
    /*---------------------------- UART BRR Configuration -----------------------*/
    /* Configure the UART Baud Rate -------------------------------------------*/
    RCC_GetClocksFreq(&RCC_ClocksStatus);
    if (UARTxbase == UART2_BASE)
    {
        apbclock = RCC_ClocksStatus.PCLK1_Frequency;
    }
    else
    {
        apbclock = RCC_ClocksStatus.PCLK2_Frequency;
    }
    /* Determine the UART_baud*/
    tmpreg = (apbclock / UART_InitStruct->UART_BaudRate) / 16;
    tmpreg1 = (apbclock / UART_InitStruct->UART_BaudRate) % 16;
    UARTx->BRR = tmpreg;
    UARTx->FRA = tmpreg1;
}

/**
* @brief  Fills each UART_InitStruct member with its default value.
* @param UART_InitStruct: pointer to a UART_InitTypeDef structure
*   which will be initialized.
* @retval : None
*/
void UART_StructInit(UART_InitTypeDef *UART_InitStruct)
{
    /* UART_InitStruct members default value */
    UART_InitStruct->UART_BaudRate = 9600;
    UART_InitStruct->UART_WordLength = UART_WordLength_8b;
    UART_InitStruct->UART_StopBits = UART_StopBits_1;
    UART_InitStruct->UART_Parity = UART_Parity_No;
    UART_InitStruct->UART_Mode = UART_Mode_Rx | UART_Mode_Tx;
    UART_InitStruct->UART_HardwareFlowControl = UART_HardwareFlowControl_None;
}

/**
* @brief  Enables or disables the specified UART peripheral.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param NewState: new state of the UARTx peripheral.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void UART_Cmd(UART_TypeDef *UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected UART by setting the uarten bit in the GCR register */
        UARTx->GCR |= GCR_UE_Set;
    }
    else
    {
        /* Disable the selected UART by clearing the uarten bit in the GCR register */
        UARTx->GCR &= GCR_UE_Reset;
    }
}

/**
* @brief  Enables or disables the specified UART interrupts.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param UART_IT: specifies the UART interrupt sources to be
*   enabled or disabled.
*   This parameter can be one of the following values:
*
* @arg UART_IT_ERR:  Error interrupt(Frame error,)
* @arg UART_IT_PE:   Parity Error interrupt
* @arg UART_OVER_ERR:  overrun Error interrupt
* @arg UART_TIMEOUT_ERR:  timeout Error interrupt
* @arg UART_IT_RXIEN: Receive Data register interrupt
* @arg UART_IT_TXIEN:  Tansmit Data Register empty interrupt
* @param NewState: new state of the specified UARTx interrupts.
*   This parameter can be: ENABLE or DISABLE.
* @retval : None
*/
void UART_ITConfig(UART_TypeDef *UARTx, uint16_t UART_IT, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_CONFIG_IT(UART_IT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the UART_IT interrupt */
        UARTx->IER |= UART_IT;
    }
    else
    {
        /* Disable the UART_IT interrupt */
        UARTx->IER &= ~UART_IT;
    }
}

/**
* @brief  Enables or disables the UART s DMA interface.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2 .
* @param UART_DMAReq: specifies the DMA request.
*   This parameter can be any combination of the following values:
* @arg UART_DMAReq_EN: UART DMA transmit request
*
* @param NewState: new state of the DMA Request sources.
*   This parameter can be: ENABLE or DISABLE.
* @note The DMA mode is not available for UART5.
* @retval : None
*/
void UART_DMACmd(UART_TypeDef *UARTx, uint16_t UART_DMAReq, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_1234_PERIPH(UARTx));
    assert_param(IS_UART_DMAREQ(UART_DMAReq));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        /* Enable the DMA transfer */
        UARTx->GCR |= UART_DMAReq;
    }
    else
    {
        /* Disable the DMA transfer */
        UARTx->GCR &= ~UART_DMAReq;
    }
}

/**
* @brief  Transmits single data through the UARTx peripheral.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param Data: the data to transmit.
* @retval : None
*/
void UART_SendData(UART_TypeDef *UARTx, uint16_t Data)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_DATA(Data));

    /* Transmit Data */
    UARTx->TDR = (Data & (uint16_t)0x00FF);
}

/**
* @brief  Returns the most recent received data by the UARTx peripheral.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @retval : The received data.
*/
uint16_t UART_ReceiveData(UART_TypeDef *UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));

    /* Receive Data */
    return (uint16_t)(UARTx->RDR & (uint16_t)0x00FF);
}

/**
* @brief  Checks whether the specified UART flag is set or not.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param UART_FLAG: specifies the flag to check.
*   This parameter can be one of the following values:
* @arg UART_FLAG_TXEMPTY:Transmit data register empty flag
* @arg UART_FLAG_TXFULL:Transmit data buffer full
* @arg UART_FLAG_RXAVL:RX Buffer has a byte flag
* @arg UART_FLAG_OVER:OverRun Error flag
* @arg UART_FLAG_TXEPT: tx and shifter are emptys flag
* @retval : The new state of UART_FLAG (SET or RESET).
*/
FlagStatus UART_GetFlagStatus(UART_TypeDef *UARTx, uint16_t UART_FLAG)
{
    FlagStatus bitstatus = RESET;
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_CLEAR_FLAG(UART_FLAG));

    if ((UARTx->CSR & UART_FLAG) != (uint16_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/**
* @brief  Clears the UARTx's pending flags.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2.
* @param UART_FLAG: specifies the flag to clear.
*   This parameter can be any combination of the following values:
* @arg UART_FLAG_TXEMPTY:Transmit data register empty flag
* @arg UART_FLAG_TXFULL:Transmit data buffer full
* @arg UART_FLAG_RXAVL:RX Buffer has a byte flag
* @arg UART_FLAG_TXEPT: tx and shifter are emptys flag
* @retval : None
*/
void UART_ClearFlag(UART_TypeDef* UARTx, uint16_t UART_FLAG)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_CLEAR_FLAG(UART_FLAG));

    UARTx->CSR &= (uint16_t)~UART_FLAG;
}

/**
* @brief  Checks whether the specified UART interrupt has occurred or not.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1, UART2, UART3.
* @param UART_IT: specifies the UART interrupt source to check.
*   This parameter can be one of the following values:
* @arg UART_IT_ERR:  Error interrupt(Frame error,)
* @arg UART_IT_PE:   Parity Error interrupt
* @arg UART_OVER_ERR:  overrun Error interrupt
* @arg UART_TIMEOUT_ERR:  timeout Error interrupt
* @arg UART_IT_RXIEN: Receive Data register interrupt
* @arg UART_IT_TXIEN:  Tansmit Data Register empty interrupt
* @retval : The new state of UART_IT (SET or RESET).
*/
ITStatus UART_GetITStatus(UART_TypeDef* UARTx, uint16_t UART_IT)
{
    FlagStatus bitstatus = RESET;
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_GET_IT(UART_FLAG));
    assert_param(IS_UART_PERIPH_FLAG(UARTx, UART_FLAG)); /* The CTS flag is not available for UART4 and UART5 */
    if ((UARTx->ISR & UART_IT) != (uint16_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return bitstatus;
}

/**
* @brief  Clears the UARTx interrupt pending bits.
* @param UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param UART_IT: specifies the interrupt pending bit to clear.
*   This parameter can be one of the following values:
* @arg UART_IT_ERR:  Error interrupt(Frame error,)
* @arg UART_IT_PE:   Parity Error interrupt
* @arg UART_OVER_ERR:  overrun Error interrupt
* @arg UART_TIMEOUT_ERR:  timeout Error interrupt
* @arg UART_IT_RXIEN: Receive Data register interrupt
* @arg UART_IT_TXIEN:  Tansmit Data Register empty interrupt

* @retval : None
*/
void UART_ClearITPendingBit(UART_TypeDef* UARTx, uint16_t UART_IT)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_CLEAR_IT(UART_IT));
    assert_param(IS_UART_PERIPH_IT(UARTx, UART_IT)); /* The CTS interrupt is not available for UART4 and UART5 */
    /*clear UART_IT pendings bit*/
    UARTx->ICR = UART_IT;
}

/**
* @brief  Selects the UART WakeUp method.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  UART_WakeUp: specifies the UART wakeup method.
*   This parameter can be one of the following values:
*     @arg UART_WakeUp_AddressMark: WakeUp by an address mark
*     @arg UART_WakeUp_IdleLine: WakeUp by an idle line detection
* @retval None
*/
void UART_WakeUpConfig(UART_TypeDef* UARTx, uint16_t UART_WakeUp)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_WAKEUP(UART_WakeUp));

    UARTx->CCR &= CCR_WAKE_Mask;
    UARTx->CCR |= UART_WakeUp;
}

/**
* @brief  Determines if the UART is in mute mode or not.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  NewState: new state of the UART mute mode.
*   This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void UART_ReceiverWakeUpCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the UART mute mode  by setting the RWU bit in the CR1 register */
        UARTx->CCR |= CCR_RWU_Set;
    }
    else
    {
        /* Disable the UART mute mode by clearing the RWU bit in the CR1 register */
        UARTx->CCR &= CCR_RWU_Reset;
    }
}

/**
* @brief  Sets the address of the UART Rx Address.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  UART_Address: Indicates the address of the UART Rx Address.
* @retval None
*/
void UART_SetRXAddress(UART_TypeDef* UARTx, uint8_t UART_Address)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_ADDRESS(UART_Address));

    /* Clear the UART address */
    UARTx->RXADD &= RX_Address_Mask;
    /* Set the UART address node */
    UARTx->RXADD |= UART_Address;
}

/**
* @brief  Sets the address of the UART Rx MASK.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  UART_Address: Indicates the address of the UART Rx MASK.
* @retval None
*/
void UART_SetRXMASK(UART_TypeDef* UARTx, uint8_t UART_Address)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_ADDRESS(UART_Address));

    /* Clear the UART address */
    UARTx->RXMASK &= RX_Address_Mask;
    /* Set the UART address node */
    UARTx->RXMASK |= UART_Address;
}

/**
* @brief  ENBALE or DISABLE the UART's 9bit.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  NewState: new state of the UART 9 bit.
*   This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void UART_Enable9bit(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the 9 bit in the CCR register */
        UARTx->CCR |= CCR_9bit_Enable;
    }
    else
    {
        /* Disable the 9 bit in the CCR register */
        UARTx->CCR &= CCR_9bit_Disable;
    }
}

/**
* @brief  Set the UART's 9bit Level.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  NewState: new state of the UART 9 bit.
*   This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void UART_Set9bitLevel(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the one bit method by setting the ONEBITE bit in the CCR register */
        UARTx->CCR |= CCR_9bit_Set;
    }
    else
    {
        /* Disable tthe one bit method by clearing the ONEBITE bit in the CCR register */
        UARTx->CCR &= CCR_9bit_Reset;
    }
}

/**
* @brief  Set the UART's 9bit Polarity.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  polarity: new state of the UART 9 bit Polarity.
*   This parameter can be:
UART_9bit_Polarity_High, UART_9bit_Polarity_Low.
* @retval None
*/
void UART_Set9bitPolarity(UART_TypeDef* UARTx, uint16_t polarity)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_UART_9bit_polarity(polarity));

    if(polarity == UART_9bit_Polarity_High)
    {
        UARTx->CCR |= UART_9bit_Polarity_High;
    }
    else
    {
        UARTx->CCR &= UART_9bit_Polarity_Low;
    }
}

/**
* @brief  Set the UART's 9bit Automatic Toggle.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  NewState: new state of the UART 9 bit Automatic Toggle.
*   This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void UART_Set9bitAutomaticToggle(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        UARTx->CCR |= CCR_B8TOG_Set;
    }
    else
    {
        UARTx->CCR &= CCR_B8TOG_Reset;
    }
}

/**
* @brief  Enables or disables the UART Half Duplex communication.
* @param  UARTx: Select the UART or the UART peripheral.
*   This parameter can be one of the following values:
*   UART1,UART2.
* @param  NewState: new state of the UART Communication.
*   This parameter can be: ENABLE or DISABLE.
* @retval None
*/
void UART_HalfDuplexCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the Half-Duplex mode by setting the HDSEL bit in the CR3 register */
        UARTx->SCR |= SCR_HDSEL_Set;
    }
    else
    {
        /* Disable the Half-Duplex mode by clearing the HDSEL bit in the CR3 register */
        UARTx->SCR &= SCR_HDSEL_Reset;
    }
}

/**
  * @brief  Sets the specified UART guard time.
  * @param  UARTx: where x can be 1, 2 or 3 to select the UART peripheral.
  * @param  UART_GuardTime: specifies the guard time.
  * @note The guard time bits are not available for UART4 and UART5.
  * @retval None
  */
void UART_SetGuardTime(UART_TypeDef* UARTx, uint8_t UART_GuardTime)
{
    /* Check the parameters */
    assert_param(IS_UART_123_PERIPH(UARTx));

    /* Clear the UART Guard time */
    UARTx->SCR &= SCR_SCFCNT_Mask;
    /* Set the UART guard time */
    UARTx->SCR |= (uint16_t)((uint16_t)UART_GuardTime << 0x04);
}

/**
  * @brief  Enables or disables the UART's Smart Card mode.
  * @param  UARTx: where x can be 1, 2 or 3 to select the UART peripheral.
  * @param  NewState: new state of the Smart Card mode.
  *   This parameter can be: ENABLE or DISABLE.
  * @note The Smart Card mode is not available for UART4 and UART5.
  * @retval None
  */
void UART_SmartCardCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_123_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        /* Enable the SC mode by setting the SCEN bit in the SCR register */
        UARTx->SCR |= SCR_SCEN_Set;
    }
    else
    {
        /* Disable the SC mode by clearing the SCEN bit in the SCR register */
        UARTx->SCR &= SCR_SCEN_Reset;
    }
}

/**
  * @brief  Enables or disables NACK transmission.
  * @param  UARTx: where x can be 1, 2 or 3 to select the UART peripheral.
  * @param  NewState: new state of the NACK transmission.
  *   This parameter can be: ENABLE or DISABLE.
  * @note The Smart Card mode is not available for UART4 and UART5.
  * @retval None
  */
void UART_SmartCardNACKCmd(UART_TypeDef* UARTx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_UART_123_PERIPH(UARTx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    if (NewState != DISABLE)
    {
        /* Enable the NACK transmission by setting the NACK bit in the CR3 register */
        UARTx->SCR |= SCR_NACK_Set;
    }
    else
    {
        /* Disable the NACK transmission by clearing the NACK bit in the CR3 register */
        UARTx->SCR &= SCR_NACK_Reset;
    }
}

/**
  * @brief  Transmits break characters.
  * @param  UARTx: Select the UART or the UART peripheral.
  *   This parameter can be one of the following values:
  *   UART1,UART2.
  * @retval None
  */
void UART_SendBreak(UART_TypeDef* UARTx)
{
    /* Check the parameters */
    assert_param(IS_UART_ALL_PERIPH(UARTx));

    /* Send break characters */
    UARTx->CCR |= CCR_BRK_Set;
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

