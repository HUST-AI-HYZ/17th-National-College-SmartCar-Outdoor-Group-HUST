
#include "HAL_conf.h"
#include "opamp.h"

void opamp_init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE); 
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;           //运放所使用的引脚需要设置为模拟输入
	GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_OPAMP, ENABLE);   //开启时钟

    OPAMP_Configure(OPAMP1, ENABLE);                        //使能运放
    OPAMP_Configure(OPAMP2, ENABLE);
    OPAMP_Configure(OPAMP3, ENABLE);
}

