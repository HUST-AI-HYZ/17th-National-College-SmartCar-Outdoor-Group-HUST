
#include "HAL_conf.h"
#include "bldc_config.h"
#include "comp.h"

void comp_init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOD, ENABLE);
    
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  =  GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;                           //引脚需要设置为模拟输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_COMP, ENABLE);                    //开启时钟
    
    COMP_InitTypeDef COMP_InitStructure;
    COMP_InitStructure.COMP_InvertingInput = COMP_InvertingInput_CRV;       //比较器反向端连接到CRV
    COMP_InitStructure.COMP_NonInvertingInput = COMP_NonInvertingInput_IO0; //比较器同向端连接到IO0
    COMP_InitStructure.COMP_Output = COMP_Output_TIM1BKIN;                  //比较器输出到定时器1的刹车输入端口
    COMP_InitStructure.COMP_OutputPol = COMP_OutputPol_NonInverted;         //输出不反向
    COMP_InitStructure.COMP_Hysteresis = COMP_Hysteresis_No;                //比较器输出不延时
    COMP_InitStructure.COMP_Mode = COMP_Mode_MediumSpeed;                   //中等速率
    COMP_InitStructure.COMP_Filter = COMP_Filter_32_Period;                 //比较器输出滤波时长为64个周期
    
    SET_COMP_CRV(COMP_InvertingInput_CRV, BLDC_FAULT_THRESHOLD);            //设置CRV电压为2/20*VCC
    COMP_Init(COMP_Selection_COMP4, &COMP_InitStructure);
    COMP_Cmd(COMP_Selection_COMP4, ENABLE);                                 //比较器使能
}

