
#include "HAL_conf.h"
#include "HAL_device.h"
#include "uart.h"


#define DEBUG_UART      UART1           //定义所使用的串口
#define UART_PORT       GPIOD           //定义串口端口
#define UART_TX_PIN     GPIO_Pin_0      //定义串口发送引脚
#define UART_RX_PIN     GPIO_Pin_1      //定义串口接收引脚
#define UART_TX_AFPIN   GPIO_PinSource0 //定义串口发送引脚
#define UART_RX_AFPIN   GPIO_PinSource1 //定义串口接收引脚

void uart_init(uint32 baudrate)
{
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);

    GPIO_PinAFConfig(UART_PORT, UART_TX_AFPIN, GPIO_AF_3);
    GPIO_PinAFConfig(UART_PORT, UART_RX_AFPIN, GPIO_AF_3);
    
    //引脚初始化设置
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = UART_TX_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(UART_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = UART_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(UART_PORT, &GPIO_InitStructure);

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_UART1, ENABLE);
    
    UART_InitTypeDef UART_InitStructure;
    UART_StructInit(&UART_InitStructure);
    UART_InitStructure.UART_BaudRate = baudrate;
    UART_InitStructure.UART_WordLength = UART_WordLength_8b;
    UART_InitStructure.UART_StopBits = UART_StopBits_1;
    UART_InitStructure.UART_Parity = UART_Parity_No;
    UART_InitStructure.UART_HardwareFlowControl = UART_HardwareFlowControl_None;
    UART_InitStructure.UART_Mode = UART_Mode_Rx | UART_Mode_Tx;

    UART_Init(DEBUG_UART, &UART_InitStructure);
    UART_Cmd(DEBUG_UART, ENABLE);
}

void write_byte(uint8 data)
{
    UART_SendData(DEBUG_UART, data);
    while(!UART_GetFlagStatus(DEBUG_UART, UART_FLAG_TXEPT));
}

void write_str(int8 *str)
{
    while(*str)
    {
        write_byte(*str++);
    }
}

void write_buffer(uint8 *buffer, uint32 length)
{
    while(length--)
    {
        write_byte(*buffer++);
    }
}

