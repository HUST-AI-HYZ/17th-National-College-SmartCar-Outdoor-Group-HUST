

#include "isr.h"
#include "isr.h"
#include "button.h"
#include "route_generating.h"
#include "common.h"
#include "flash.h"
#include "gps.h"
#include "motor.h"

int16 start_delay_time = 0;
static int start_car = 0;
char CarCtrlOrder[20];
float send_order = 0.0;
bool newgpsgot = false;
bool dot_read = false;
static int set_time = 0;
bool analysis_finished = false;
//表示运行的圈数
int16 run_circle = 0;
//是否进入停止模式
bool stop_mode = false;
//进入停止模式后运行距离
float run_stop_distance = 0.0;

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM1 的定时器更新中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM1_UP_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM1_UP_IRQHandler (void)
{
    TIM1->SR &= ~TIM1->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM2 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM2_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM2_IRQHandler (void)
{
    TIM2->SR &= ~TIM2->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM3 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM3_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM3_IRQHandler (void)
{
    TIM3->SR &= ~TIM3->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM4 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM4_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM4_IRQHandler (void)
{
    TIM4->SR &= ~TIM4->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM5 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM5_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM5_IRQHandler (void)
{
    TIM5->SR &= ~TIM5->SR;                                                      // 清空中断状态
    // 此处编写用户代码
    //每次跑前，清除掉总打角，防止放置车身导致的问题
    static int32 gps_err = 0;


    total_time++;

    //拨码器在左边是0
    //角速度累计  500hz
    if (total_time % 2 == 0)
    {
        //消除静态误差
        static float error_z_sum = 0;

        //获取ICM20602的测量数值
        icm20602_get_gyro();



        //低于一定值的角速度当零漂滤去
        if (fabs(icm_gyro_z - error_z_sum) < 15)
        {
            icm_gyro_z = error_z_sum;
        }

        //度每秒为单位的陀螺仪角速度
        Data_Gyro.gyro_z = (float)(icm_gyro_z - error_z_sum) * 2000 / 32767;

        //角度累加
        //一次PD舵机间产生的误差
        Data_Gyro.angle_sum_once =  Data_Gyro.angle_sum_once  - Data_Gyro.gyro_z / 500;
        Data_Gyro.angle_sum_all = Data_Gyro.angle_sum_all - Data_Gyro.gyro_z / 500;
        Data_Gyro.angle_sum_area = Data_Gyro.angle_sum_area - Data_Gyro.gyro_z / 500;

        //开始计算角度航向
        static int i = 0;
        //根据是否发车判断
        if (start_car > 0)
        {
            static double temp = 0.0;
            //定初始值，只执行一次
            if (i == 0)
            {
                //暂时默认0 - 1方向为起点
                temp = GPSBearingAngle(Points_Use[0].lat_keypoint, Points_Use[0].lon_keypoint,
                                       Points_Use[1].lat_keypoint, Points_Use[1].lon_keypoint) + 0.1;
                i = 1;
            }

            if (fabs(temp) > 1e-6)
            {

                //陀螺仪角有点问题？今天先用gps角度调车
                Data_Gyro.gyro_direction = fmod(temp + Data_Gyro.angle_sum_all, 360.0);

                if (Data_Gyro.gyro_direction < 0.0)
                {
                    Data_Gyro.gyro_direction = 360.0 + Data_Gyro.gyro_direction;
                }
            }
        }
        else
        {
            i = 0;
        }
    }


    //舵机控制
    if (total_time % 20 == 0 && (gps_mode == 1 || gps_mode == 2) && (Save_Data.f_ground_speed > 3.15) )
    {
        //如果取4的话震荡过大
        gps_servo_control();
    }

//    //串口发送信息
//    if (total_time % 50 == 0)
//    {
//        send_order++;
//    }

    //距离累计
    if (total_time % 100 == 0)
    {   
			  distance_from_lastpoint = distance_from_lastpoint + (Save_Data.f_ground_speed / 3.6 )* 0.1;
        if (Save_Data.f_ground_speed > 1.0 && stop_mode == true)
        {
            run_stop_distance =  run_stop_distance + (Save_Data.f_ground_speed / 3.6 )* 0.1;
        }
    }
    /*****************是否到达停车要求*************/
    if (run_stop_distance > stop_distance)
    {
        //停车
        g_speed_set = 0.0;
        setspeed_to_pwm();
    }

    //如果搜索到字符串s
    if (strcmp(CarCtrlOrder, "s") == 0)
    {
        //停车
        g_speed_set = 0;
        gps_mode = 0;

        start_car = 0;

        duty_mid = 0;
        setspeed_to_pwm();
        strcpy(CarCtrlOrder, "");

    }
    else if (strcmp(CarCtrlOrder, "k") == 0)
    {
        //开机发车
        gps_mode = 1;
        strcpy(CarCtrlOrder, "");
    }
    else if (strcmp(CarCtrlOrder, "r") == 0)
    {
        //开机发车
        gps_mode = -2;
        strcpy(CarCtrlOrder, "");

    }
    /******************延迟发车*****************************/
    //注意int16范围
    if (gps_mode == 1 && start_delay_time > 0)
    {
        start_delay_time--;
    }
     
		/*****************速度处理***********************/
    //速度切换(不过速度切换的点可以细化)
    //直线区
    //速度切换(不过速度切换的点可以细化)
   //直线区
		if (total_time % 30 == 0 && dot_read == true)
		{
				if (passed_sections <= 0 && run_circle == 0)
				{
						g_speed_set = speed_max_region[3];
				}
				else if (area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA)
			 {   
					 //速度靠拢
					 if (g_speed_set < speed_max_region[0])
					 {
							g_speed_set = g_speed_set + speed_step;
					 }
					 else if(g_speed_set > speed_max_region[0])
					 {
						 g_speed_set = g_speed_set - speed_step;
					 }
						 
			 }
				else if (area_tag == ROUND_AREA)
			 {
						if (g_speed_set < speed_max_region[1])
					 {
							g_speed_set = g_speed_set + speed_step;
					 }
					 else if(g_speed_set > speed_max_region[1])
					 {
						 g_speed_set = g_speed_set - speed_step;
					 }
			 }
				else if (area_tag == ROUND_IN_AREA)
			 {
					 if (g_speed_set < speed_max_region[2])
					 {
							g_speed_set = g_speed_set + speed_step;
					 }
					 else if(g_speed_set > speed_max_region[2])
					 {
						 g_speed_set = g_speed_set - speed_step;
					 }
			 }		
        //速度设置
				setspeed_to_pwm();			 
     }
    /*******************GPS处理模块*************************/
    if (newgpsgot)
    {
        //表示已收到数据
        newgpsgot = false;

        //解析GPS数据
        parseGpsBuffer();

        //char化float
        Get_InitData(&Save_Data.longi_Z, &Save_Data.longi_X, &Save_Data.lati_Z, &Save_Data.lati_X);

        //度分秒化小数
        Trans_Data_Z_plus_X();

        //误差计算模式
        if (Save_Data.isUsefull == true && gps_mode == 1 && start_delay_time <= 0)
        {

            /***************起步区*****************/
            //防止微扰动
            if ( passed_sections == 0 &&  Save_Data.f_ground_speed < 1.8 )
            {
                pwm_set_duty(SERVO_PIN, pwm_mid_angle);
                //现在点与第一个点的误差
                error_lenth_s_p = GPSDistance(Save_Data.f_lati, Save_Data.f_longi,Points_Use[1].lat_keypoint, Points_Use[1].lon_keypoint);
                error_direction = GPSBearingAngle(Save_Data.f_lati, Save_Data.f_longi,Points_Use[1].lat_keypoint, Points_Use[1].lon_keypoint);
            }


            //创建文件
            if (analysis_finished == false)
            {
                //拷贝时间，便于文件创建
                strcpy(StartTime,Save_Data.UTCTime);
            }


            //自动搜索起点，自动产生路径
            //（通过给出提示？优化搜索最近点）
            //如果要分析
            if (analysis_finished == false && guiding_mode > 0)
            {

                //微操
                micro_operating();
                //路径规划
                gps_auto_plannation();
                //复原
                micro_operating_recover();
                //分析结束
                analysis_finished = true;
            }
            else if(analysis_finished == false && guiding_mode <= 0)
            {
                //直接拷贝模式，相当于从那里发车
                for(int i = 0; i < finished_gpspoints; i++)
                {
                    Points_Use[i] = Points_Stored[i];
                }
								for (int i = 0 ; i < 3 ; i ++)
								{
									Points_Use[finished_gpspoints + i] = Points_Stored[i];
								}
								
								analysis_finished= true;
            }



            /*******************读取点信息*************************/
            if (passed_sections <= run_sections && dot_read == false && analysis_finished == true)
            {
                /***************区域判定****************/
                area_tag = Points_Use[passed_sections + 1].type_point;
								
                //说明存在区域切换(适用于直道路起步)
                //若加入缓冲区，这里要改
                if (Points_Use[passed_sections + 1].type_point != Points_Use[passed_sections].type_point)
                {
                    Data_Gyro.angle_sum_area = 0.0;
                }
								
                //信息读取
                Use_Data.lat_last = Points_Use[passed_sections].lat_keypoint;
                Use_Data.lon_last = Points_Use[passed_sections].lon_keypoint;

                Use_Data.lat_next = Points_Use[passed_sections + 1].lat_keypoint;
                Use_Data.lon_next = Points_Use[passed_sections + 1].lon_keypoint;

                //距离差,航向差
                Use_Data.TwoDotsDis = GPSDistance(Use_Data.lat_last, Use_Data.lon_last,
                                                  Use_Data.lat_next, Use_Data.lon_next);
                //在区域切换的地方，计算出来的航向会有问题一般 （但是误差应该不大）
                Use_Data.TwoDotsDirec = GPSBearingAngle(Use_Data.lat_last, Use_Data.lon_last,
                                                        Use_Data.lat_next, Use_Data.lon_next);

                //归零
                Data_Gyro.angle_sum_once = 0;

                //是否读点
                dot_read = true;

            }


            /****************误差解算****************/
            //求出与下一个点的距离误差
            Use_Data.distan_next_point_now = GPSDistance(Save_Data.f_lati, Save_Data.f_longi,
                                             Use_Data.lat_next, Use_Data.lon_next);

            //求出与下个点的目标航向角
            Use_Data.angle_next_heading_now = GPSBearingAngle(Save_Data.f_lati, Save_Data.f_longi,Use_Data.lat_next, Use_Data.lon_next);


            /************换点，PID************************/
            //求出当前航向与目标航向的误差，并进行舵机打角控制
            //速度上不去，航向角求出来也是错的
            //测试用最低速度设置为2m/s
            if (Save_Data.f_ground_speed > 3.15)
            {


                //第一次清掉历史数据
                //可以理解为发车时清楚之前在陀螺仪残留的数据
                if (start_car == 0)
                {
                    start_car = 1;
                    Data_Gyro.angle_sum_all = 0.0;
                    Data_Gyro.angle_sum_area = 0.0;
                    error_icm_last = 0.0;
                }

                //求角度误差
                GpsAngleError();

                //换点函数
                GpsDotSelection();

                //陀螺仪控制归零
                Data_Gyro.angle_sum_once = 0.0;

                //新的调节，误差要归零
                error_icm_last = 0.0;

            }

            /*******************停车检测***********************/
            //如果通过的点数==总点数
            //则停车 ， 这边建议采集一圈多一点的信息
            //这里要改
            if (passed_sections >= finished_gpspoints && passed_sections > 0)
            {
                //表示又多跑了一圈
                run_circle++;
                //从新计数
                passed_sections = 0;
                //如果达到了圈数的要求
                if	(circle_time == run_circle)
                {
                    stop_mode = true;
                }
            }



        }
        //GPS 开车复位
        else if (gps_mode == -2)
        {
            //GPS模式复位，防止一按车就跑了
            gps_mode = 0;

            //距离复位
            distance_from_lastpoint = 0.00001;
            //区域复位
            now_area = 0;
            //误差复位
            error_icm_last = 0.0;

            error_lenth_s_p = 0.0;
            //过点数复位
            passed_sections = 0;
            //表明是否发车，以此清除陀螺仪原有数据复位
            start_car = 0;
            //读点复位
            dot_read = false;

            area_tag = 0;
            //停车复位
            start_delay_time = 2000;
            //行车复位
            analysis_finished = false;

            //表示运行的圈数
            run_circle = 0;
            //是否进入停止模式
            stop_mode = false;
            //距离清零
            run_stop_distance = 0.0;
        }
        else if (gps_mode == 6)
        {
            collection_by_hand();
            micro_delete();
        }
        else if (gps_mode == 7 && route_write == 3)
        {
            dot_read = false;
            analysis_finished = false;

            for (int i = 0; i < ALL_POINTS; i++)
            {
                Points_Stored[i].lat_keypoint = 0.0;
                Points_Stored[i].lon_keypoint = 0.0;

                Points_Stored[i].lat_kypnt_f = 0.0;
                Points_Stored[i].lon_kypnt_f = 0.0;
                Points_Stored[i].lat_kypnt_z = 0.0;
                Points_Stored[i].lon_kypnt_z = 0.0;

                Points_Stored[i].type_point = 0.0;
            }

            gps_mode = 2;
            finished_gpspoints = 0;
            distance_from_lastpoint = 0.00001;

            //因为涉及到了数据删除，所以重新处理数据
            if (dot_read == true)
            {
                dot_read = false;
            }
        }
    }
    //gps控制结束
    // 此处编写用户代码
    //debug测试用,低速时可以
    /*************遥控停车************/
		/*if (gpio_get(A0))
		{
			//停车
        g_speed_set = 0.0;
        setspeed_to_pwm();
		}*/
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM6 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM6_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM6_IRQHandler (void)
{
    TIM6->SR &= ~TIM6->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM7 的定时器中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM7_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM7_IRQHandler (void)
{
    TIM7->SR &= ~TIM7->SR;                                                      // 清空中断状态
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       TIM8 的定时器更新中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(TIM8_UP_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void TIM8_UP_IRQHandler (void)
{
    TIM8->SR &= ~TIM8->SR;                                                      // 清空中断状态
    tsl1401_collect_pit_handler();
    // 此处编写用户代码

    // 此处编写用户代码
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART1 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART1_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART1_IRQHandler(void)
{
    if(UART1->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART1->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART1->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART1->ICR |= 0x00000002;                                               // 清除中断标志位
#if DEBUG_UART_USE_INTERRUPT                                                    // 如果开启 debug 串口中断
        debug_interrupr_handler();                                              // 调用 debug 串口接收处理函数 数据会被 debug 环形缓冲区读取
#endif                                                                          // 如果修改了 DEBUG_UART_INDEX 那这段代码需要放到对应的串口中断去
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}
char ReadBuff[256];

uint8 buf = 0;

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART2 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART2_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART2_IRQHandler(void)
{
    static int num = 0;
    if(UART2->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART2->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART2->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART2->ICR |= 0x00000002;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断
        uart_getchar(UART_2, &buf);

        //获取读到的字符串，而且上一个是换行
        //如果溢出了怎么办呢？
        if (buf == '\r')
        {
            ReadBuff[num] = '\0';
            strcpy(Save_Data.GPS_Buffer, ReadBuff);
            Save_Data.isGetData = 1;
            num = 0;
            newgpsgot = true;
        }
        else
        {
            ReadBuff[num++] = buf;

            //防止数组爆掉
            if (num > 254)
            {
                num = 0;
            }
        }
        // 此处编写用户代码
    }
}
uint8 CarCtrlTemp[20];
uint8 buf2 = 0;
//-------------------------------------------------------------------------------------------------------------------
// @brief       UART3 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART3_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART3_IRQHandler(void)
{
    static int i = 0;
    if(UART3->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART3->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART3->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART3->ICR |= 0x00000002;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断
        /* 用户代码 */
        //为啥收发出现问题？？？
        uart_getchar(UART_3, &buf2);

        //获取读到的字符串，而且上一个是换行
        //如果溢出了怎么办呢？
        if (buf2 == 's' || buf2 == 't' || buf2 == 'k' || buf2 == 'r')
        {
            CarCtrlTemp[i] = buf2;
            strcpy(CarCtrlOrder, CarCtrlTemp);
            i = 0;
            CarCtrlTemp[i] = '\0';
        }
        else
        {
            //CarCtrlTemp[i++] = buf2;
        }
        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART4 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART4_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART4_IRQHandler(void)
{
    if(UART4->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART4->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART4->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART4->ICR |= 0x00000002;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART5 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART5_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART5_IRQHandler(void)
{
    if(UART5->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART5->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART5->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART5->ICR |= 0x00000002;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART6 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART6_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART6_IRQHandler(void)
{
    if(UART6->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART6->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART6->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART6->ICR |= 0x00000002;                                               // 清除中断标志位
        wireless_module_uart_handler();
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART7 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART7_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART7_IRQHandler(void)
{
    if(UART7->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART7->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART7->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART7->ICR |= 0x00000002;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       UART8 的串口中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(UART8_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void UART8_IRQHandler(void)
{
    if(UART8->ISR & 0x00000001)                                                 // 串口发送缓冲空中断
    {
        UART8->ICR |= 0x00000001;                                               // 清除中断标志位
        // 此处编写用户代码
        // 务必填写数据或者关闭中断 否则会一直触发串口发送中断

        // 此处编写用户代码
    }
    if(UART8->ISR & 0x00000002)                                                 // 串口接收缓冲中断
    {
        UART8->ICR |= 0x00000002;                                               // 清除中断标志位
        camera_uart_handler();
        // 此处编写用户代码
        // 务必读取数据或者关闭中断 否则会一直触发串口接收中断

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI0 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI0_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI0_IRQHandler(void)
{
    EXTI->PR |= (0x00000001 << 0);                                              // 清除 line0 触发标志
    // 此处编写用户代码 (A0/B0..H0) 引脚触发

    // 此处编写用户代码 (A0/B0..H0) 引脚触发
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI1 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI1_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI1_IRQHandler(void)
{
    EXTI->PR |= (0x00000001 << 1);                                              // 清除 line1 触发标志
    // 此处编写用户代码 (A1/B1..H1) 引脚触发

    // 此处编写用户代码 (A1/B1..H1) 引脚触发
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI2 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI2_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI2_IRQHandler(void)
{
    EXTI->PR |= (0x00000001 << 2);                                              // 清除 line2 触发标志
    // 此处编写用户代码 (A2/B2..H2) 引脚触发

    // 此处编写用户代码 (A2/B2..H2) 引脚触发
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI3 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI3_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI3_IRQHandler(void)
{
    EXTI->PR |= (0x00000001 << 3);                                              // 清除 line3 触发标志
    // 此处编写用户代码 (A3/B3..H3) 引脚触发

    // 此处编写用户代码 (A3/B3..H3) 引脚触发
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI4 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI4_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI4_IRQHandler(void)
{
    EXTI->PR |= (0x00000001 << 4);                                              // 清除 line4 触发标志
    // 此处编写用户代码 (A4/B4..G4) 引脚触发

    // 此处编写用户代码 (A4/B4..G4) 引脚触发
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI9-5 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI9_5_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI9_5_IRQHandler(void)
{
    if(EXTI->PR&(0x00000001 << 5))                                              // 检测 line5 是否触发
    {
        EXTI->PR |= (0x00000001 << 5);                                          // 清除 line5 触发标志
        // 此处编写用户代码 (A5/B5..G5) 引脚触发

        // 此处编写用户代码 (A5/B5..G5) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 6))                                              // 检测 line6 是否触发
    {
        EXTI->PR |= (0x00000001 << 6);                                          // 清除 line6 触发标志
        // 此处编写用户代码 (A6/B6..G6) 引脚触发

        // 此处编写用户代码 (A6/B6..G6) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 7))                                              // 检测 line7 是否触发
    {
        EXTI->PR |= (0x00000001 << 7);                                          // 清除 line8 触发标志
        // 此处编写用户代码 (A7/B7..G7) 引脚触发

        // 此处编写用户代码 (A7/B7..G7) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 8))                                              // 检测 line8 是否触发
    {
        EXTI->PR |= (0x00000001 << 8);                                          // 清除 line8 触发标志
        camera_vsync_handler();
        // 此处编写用户代码 (A8/B8..G8) 引脚触发

        // 此处编写用户代码 (A8/B8..G8) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 9))                                              // 检测 line9 是否触发
    {
        EXTI->PR |= (0x00000001 << 9);                                          // 清除 line9 触发标志
        // 此处编写用户代码 (A9/B9..G9) 引脚触发

        // 此处编写用户代码 (A9/B9..G9) 引脚触发
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       外部中断 EXTI15-10(A15-10/B15-10..G15-10) 线 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(EXTI15_10_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void EXTI15_10_IRQHandler (void)
{
    if(EXTI->PR&(0x00000001 << 10))                                             // 检测 line10 是否触发
    {
        EXTI->PR |= (0x00000001 << 10);                                         // 清除 line10 触发标志
        // 此处编写用户代码 (A10/B10..G10) 引脚触发

        // 此处编写用户代码 (A10/B10..G10) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 11))                                             // 检测 line11 是否触发
    {
        EXTI->PR |= (0x00000001 << 11);                                         // 清除 line11 触发标志
        // 此处编写用户代码 (A11/B11..G11) 引脚触发

        // 此处编写用户代码 (A11/B11..G11) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 12))                                             // 检测 line12 是否触发
    {
        EXTI->PR |= (0x00000001 << 12);                                         // 清除 line12 触发标志
        // 此处编写用户代码 (A12/B12..G12) 引脚触发

        // 此处编写用户代码 (A12/B12..G12) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 13))                                             // 检测 line13 是否触发
    {
        EXTI->PR |= (0x00000001 << 13);                                         // 清除 line13 触发标志
        // 此处编写用户代码 (A13/B13..G13) 引脚触发

        // 此处编写用户代码 (A13/B13..G13) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 14))                                             // 检测 line14 是否触发
    {
        EXTI->PR |= (0x00000001 << 14);                                         // 清除 line14 触发标志
        // 此处编写用户代码 (A14/B14..G14) 引脚触发

        // 此处编写用户代码 (A14/B14..G14) 引脚触发
    }
    if(EXTI->PR&(0x00000001 << 15))                                             // 检测 line15 是否触发
    {
        EXTI->PR |= (0x00000001 << 15);                                         // 清除 line15 触发标志
        // 此处编写用户代码 (A15/B15..G15) 引脚触发

        // 此处编写用户代码 (A15/B15..G15) 引脚触发
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 1 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH1_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (0 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (0 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 2 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH2_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel2_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (1 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (1 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 3 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH3_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel3_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (2 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (2 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 4 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH4_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel4_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (3 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (3 * 4));                                  // 清空该通道中断标志
        camera_dma_handler();
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 5 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH5_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel5_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (4 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (4 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 6 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH6_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel6_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (5 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (5 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA1 通道 7 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA1_CH7_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA1_Channel7_IRQHandler(void)
{
    if(DMA1->ISR & (0x00000001 << (6 * 4)))                                     // 判断触发通道
    {
        DMA1->IFCR |= (0x00000001 << (6 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA2 通道 1 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA2_CH1_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel1_IRQHandler(void)
{
    if(DMA2->ISR & (0x00000001 << (0 * 4)))                                     // 判断触发通道
    {
        DMA2->IFCR |= (0x00000001 << (0 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA2 通道 2 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA2_CH2_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel2_IRQHandler(void)
{
    if(DMA2->ISR & (0x00000001 << (1 * 4)))                                     // 判断触发通道
    {
        DMA2->IFCR |= (0x00000001 << (1 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA2 通道 3 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA2_CH3_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel3_IRQHandler(void)
{
    if(DMA2->ISR & (0x00000001 << (2 * 4)))                                     // 判断触发通道
    {
        DMA2->IFCR |= (0x00000001 << (2 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA2 通道 4 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA2_CH4_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel4_IRQHandler(void)
{
    if(DMA2->ISR & (0x00000001 << (3 * 4)))                                     // 判断触发通道
    {
        DMA2->IFCR |= (0x00000001 << (3 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}

//-------------------------------------------------------------------------------------------------------------------
// @brief       DMA2 通道 5 的中断服务函数 启动 .s 文件定义 不允许修改函数名称
//              默认优先级 修改优先级使用 interrupt_set_priority(DMA2_CH5_IRQn, 1);
//-------------------------------------------------------------------------------------------------------------------
void DMA2_Channel5_IRQHandler(void)
{
    if(DMA2->ISR & (0x00000001 << (4 * 4)))                                     // 判断触发通道
    {
        DMA2->IFCR |= (0x00000001 << (4 * 4));                                  // 清空该通道中断标志
        // 此处编写用户代码

        // 此处编写用户代码
    }
}










/********************紧急停车模块********************/
//撞击后停止运转,gps速度与编码器速度存在重大误差
//刚起步时要防止
/*  //感觉err1没大用？
if (Save_Data.f_ground_speed < 0.2 && g_car_speed > 300.0 && Save_Data.isUsefull == true && 0)
{
	//速度归零
	g_speed_set = 0.0;
	//GPS停止使用
	gps_mode = 0;
	lcd_showstr(0, 7, "ERROR1");
}
//如果过了三个点的间隔但依旧没换点没停下
else if (distance_from_lastpoint > (distance_each_point * pulse_onemeter * 3.0) && Save_Data.isUsefull == true)
{
	//速度归零
	g_speed_set = 0.0;
	gps_mode = 0;
               duty_mid = 1000;
	lcd_showstr(0, 7, "ERROR2");
}
//忘插编码器时（但是注意滞后性）
else if (gps_mode != 0 && Save_Data.f_ground_speed > 9.0 && g_car_speed < 50.0 && 0)
{
	//速度归零
	g_speed_set = 0.0;
	duty_mid = 1000;
	lcd_showstr(0, 7, "ERROR3");
}
       //GPS寄了
       else if (gps_mode != 0 && Save_Data.isUsefull == false)
{
               if (gps_err++ > 10000)
               {
                 //速度归零
                 g_speed_set = 0.0;
                 duty_mid = 1000;
                 gps_mode = 0;
                 lcd_showstr(0, 7, "ERROR4");
               }

}
       else
       {
          gps_err = 0;
       }
//passed points_Use = x时
//当前位于x - x+1区间，第0个点便是起点.
//直线区，弯道区的划定是根据下一个目标点决定*/