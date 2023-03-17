#ifndef _bldc_config_h
#define _bldc_config_h

//0：不开启刹车功能     1：开启
#define BLDC_BRAKE_ENABLE       1       

//0：不开启速度闭环     1：开启
#define BLDC_CLOSE_LOOP_ENABLE  0       

//只有在闭环开启的时候速度曲线才能被设置
#if BLDC_CLOSE_LOOP_ENABLE==1      
    //此宏定义作用是实现电机缓慢加速
    //0：不开启速度曲线设置 1：开启
    #define BLDC_SPEED_CURVE_ENABLE 1
    
    //设置最大转速
    #define BLDC_MAX_SPEED          45000   

    //BLDC_STEP_SPEED作用是使得电机缓慢加速，避免出现电流过大的，值越小电机加速越慢
    //如果不需要可以通过将BLDC_SPEED_CURVE_ENABLE设置为0，来关闭这个功能
    #define BLDC_STEP_SPEED         100      

#endif



//设置总电流阈值，当母线电流超过设置的阈值会自动关闭pwm输出 可设置范围0-15 值越小保护越强
#define BLDC_FAULT_THRESHOLD    15
   










#endif

