
#include "HAL_conf.h"
#include "adc.h"

adc_struct adc_information;


void adc_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_ADC2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE); 
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3 | GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		//ADC引脚需要设置为模拟输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_2 | GPIO_Pin_10 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                  //adc分辨率设置为12位
	ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_8;                  //adc时钟=PCLK2/8
	ADC_InitStructure.ADC_Mode = ADC_Mode_Single_Period;                    //单周期转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			        //数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;   //使用定时器CC4通道触发ADC转换，即每个PWM周期触发一次ADC转换
	ADC_Init(ADC1, &ADC_InitStructure);
	ADC_Cmd(ADC1, ENABLE);//使能ADC
    
    ADC_RegularChannelConfig(ADC1, 3, 0, ADC_SampleTime_7_5Cycles);     //A3引脚  检测母线经过RC滤波之后的电流
	ADC_RegularChannelConfig(ADC1, 6, 1, ADC_SampleTime_7_5Cycles);     //A6引脚  检测母线滤波 之前 的电流
    ADC_RegularChannelConfig(ADC1, 7, 2, ADC_SampleTime_7_5Cycles);     //A7引脚  检测电源电压
    ADC_RegularChannelConfig(ADC1, 10, 3, ADC_SampleTime_7_5Cycles);    //B2引脚  检测A相电流
    ADC_RegularChannelConfig(ADC1, 11, 4, ADC_SampleTime_7_5Cycles);    //B10引脚 检测B相电流
    
    //ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;                  //adc分辨率设置为12位
	ADC_InitStructure.ADC_PRESCARE = ADC_PCLK2_PRESCARE_8;                  //adc时钟=PCLK2/8
	ADC_InitStructure.ADC_Mode = ADC_Mode_Single_Period;                    //单周期转换
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;			        //数据右对齐
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC4;   //使用定时器CC4通道触发ADC转换，即每个PWM周期触发一次ADC转换
	ADC_Init(ADC2, &ADC_InitStructure);
	ADC_Cmd(ADC2, ENABLE);//使能ADC

    ADC_RegularChannelConfig(ADC2, 1, 0, ADC_SampleTime_7_5Cycles);    //B10引脚 检测B相电流
    ADC_ExternalTrigConvCmd(ADC2, ENABLE);              //ADC外部触发使能
    
	ADC_ITConfig(ADC1,ADC_IT_EOC, ENABLE);              //使能ADC中断
	ADC_ExternalTrigConvCmd(ADC1, ENABLE);              //ADC外部触发使能
	NVIC_InitStructure.NVIC_IRQChannel = ADC1_IRQn;     //中断编号
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0x01;	//优先级设置 0:最高, 3:最低
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		//中断使能
	NVIC_Init(&NVIC_InitStructure);
}

void adc_read(void)
{
    adc_information.current_bus_filter = ADC1->ADDR3;   //获取滤波之后的母线电流
    adc_information.current_bus = ADC1->ADDR6;          //获取母线电流
    adc_information.voltage_bus = ADC2->ADDR1*5*4/4096; //读取电源电压 ADC引脚上的电压 = ADC值/4096*5  电源电压 = ADC引脚上的电压 / (R27/(R27+R20))
    adc_information.current_a = ADC1->ADDR10;           //获取A相电流
    adc_information.current_b = ADC1->ADDR11;           //获取B相电流
   
}
