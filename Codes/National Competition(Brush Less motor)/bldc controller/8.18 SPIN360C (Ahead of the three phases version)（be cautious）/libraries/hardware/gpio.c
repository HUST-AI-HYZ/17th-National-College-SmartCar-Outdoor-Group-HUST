
#include "HAL_conf.h"
#include "gpio.h"


#define LED_RUN_PORT        GPIOC
#define LED_RUN_PIN         GPIO_Pin_15

#define LED_FAULT_PORT      GPIOC
#define LED_FAULT_PIN       GPIO_Pin_14

#define LED_EN_PORT         GPIOB
#define LED_EN_PIN          GPIO_Pin_13



void led_run_control(LED_STATUS_enum status)
{
    GPIO_WriteBit(LED_RUN_PORT, LED_RUN_PIN, (BitAction)status);
}


void led_fault_control(LED_STATUS_enum status)
{
    GPIO_WriteBit(LED_FAULT_PORT, LED_FAULT_PIN, (BitAction)status);
}

void led_en_control(LED_STATUS_enum status)
{
    GPIO_WriteBit(LED_EN_PORT, LED_EN_PIN, (BitAction)status);
}

void led_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOC|RCC_AHBPeriph_GPIOD, ENABLE); 

	GPIO_InitStructure.GPIO_Pin  =  LED_RUN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_RUN_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =  LED_FAULT_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_FAULT_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  =  LED_EN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_EN_PORT, &GPIO_InitStructure);

    
    GPIO_SetBits(LED_RUN_PORT, LED_RUN_PIN);
    GPIO_SetBits(LED_FAULT_PORT, LED_FAULT_PIN);
    GPIO_SetBits(LED_EN_PORT, LED_EN_PIN);

    //C12引脚必须设置为输出高电平
	GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	GPIO_SetBits(GPIOC, GPIO_Pin_12);                        
}




