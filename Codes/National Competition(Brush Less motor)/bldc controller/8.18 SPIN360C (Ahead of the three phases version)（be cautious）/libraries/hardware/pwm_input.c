
#include "hal_conf.h"
#include "motor.h"
#include "pwm_input.h"



float duty_cycle;
uint8 duty_flag;//当前占空比是否有效 1：有效  0；无效
int16 period, duty_low, duty_high;



void pwm_input_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
    GPIO_PinAFConfig(MOTOR_PWM_PORT, GPIO_PinSource10, GPIO_AF_6);
    GPIO_InitStructure.GPIO_Pin = MOTOR_PWM_PIN;      //PWM输入捕获引脚
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(MOTOR_PWM_PORT, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = MOTOR_DIR_PIN;       //方向引脚 引脚为低电平时电机正转，为高电平是电机反转
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(MOTOR_DIR_PORT, &GPIO_InitStructure);
    
    
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = TIM16_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);                 //根据指定的参数初始化NVIC寄存器
    
    
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_ICInitTypeDef  TIM16_ICInitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM16, ENABLE);

    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = 20000;
    TIM_TimeBaseStructure.TIM_Prescaler = SystemCoreClock / 1000000 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM16, &TIM_TimeBaseStructure);

    TIM_ICStructInit(&TIM16_ICInitStructure);
    TIM16_ICInitStructure.TIM_Channel = TIM_Channel_1;
    TIM16_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_BothEdge;
    TIM16_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;
    TIM16_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    TIM16_ICInitStructure.TIM_ICFilter = 2;
    
    TIM_PWMIConfig(TIM16, &TIM16_ICInitStructure);

    TIM_ITConfig(TIM16, TIM_IT_CC1, ENABLE);        //使能通道一中断
    TIM_ITConfig(TIM16, TIM_IT_Update, ENABLE);     //使能更新中断
    TIM_ClearITPendingBit(TIM16, TIM_IT_CC1);
    TIM_ClearITPendingBit(TIM16, TIM_IT_Update);

    TIM_Cmd(TIM16, ENABLE);
}

uint8 pwm_dir_get(void)
{
    //获取输入速度的正负极性
    //0：期望电机正转  1：期望电机反转
    return GPIO_ReadInputDataBit(MOTOR_DIR_PORT, MOTOR_DIR_PIN);
}









