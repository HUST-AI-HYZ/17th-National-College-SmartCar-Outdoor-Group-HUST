#include "gps.h"
#include "stdlib.h"
#include <math.h>
#include "isr.h"
#include <stdlib.h>
#include <string.h>
#include "route_generating.h"

#define HALF_DIRECT_LENTH 42.1
#define RADIUS 55

//static double x_pi = 3.14159265358979324 * 3000.0 / 180.0;
#define PI 3.14159265358979324

/*********GPS相关变量*********/

//上一次读取保存的数据
_SaveData Save_Data;
//存储关键点的数据  暂时保存40个
_KeyPoint Points_Stored[ALL_POINTS];
_KeyPoint Points_Use[ALL_POINTS + 3];
//_KeyPoint ERR_Points[ALL_POINTS];
//由当前点和关键点所计算出来的中间数据
_UseData Use_Data;
//行驶方向
//导航模式，是从0启动还是自搜寻启动
//默认从-1开始
int16 guiding_mode = 1;
//估计的起点(如果不估计则从0开始)
uint16 guess_start = 0;
//发车时间字符串，用于创建sdcard变量
char StartTime[12] = "f_714";
//微操判断，相当于换区域次数。
int16 area_tag = 0;
//GPS微操
//每次控制的是小数点后第六位
//在本地。
//纬度加1    距离变化0.111319
//经度加1    距离变化0.095901
//区域ZONE  5
int16 micro_op_lati[ROUTE][ZONE] = {{0} , {0} ,{0}};
int16 micro_op_longi[ROUTE][ZONE] = {{0} , {0} ,{0}};
//微操段
int16 micro_area = 0;
//上一个微操段
int16 last_micro_area = 0;
//GPS复位
//int16 gps_reset = 0;
//当前点与起点的误差
float error_lenth_s_p = 0.0;
//误差方向（当前点指向第0个点）
float error_direction = 0.0;
//跑点数
int16 run_sections = 10;
//已经通过的段数量（也表示上一个通过的点）
int16 passed_sections = 0;
//GPS的模式
int16 gps_mode = 0;
//选中的已采集的数据点的序号值
uint16 point_selected_gps = 0;
//完成采集的数据点
uint16 finished_gpspoints = 0;
//选中点的纬度
uint32 point_latitude_selected = 0.0;
//选中点的经度
uint32 point_longitude_selected = 0.0;
//从上一个点以来，走过的距离。(多少米一个点...)
float distance_from_lastpoint = 0.00001;
//每个点的间隔(直道路)
float distance_each_point_direct = 6.0;
//每个点的间隔（弯道路）
float distance_each_point_round = 6.0;
//是否换点
int8 change_point = 0;
//点类型赋值

//默认为直道
int16 point_type = 0;
int16 point_type_pageread = 0;
//水平横移
float horizontal_error = 0;
//区域
//直道区为0，先默认了
int8 now_area = 0;
int8 last_area = 0;
//直道阈值
float direct_thres = 250;
//icm 调节
float error_icm_last = 0.0;

//路线选择
//因为现在采用了自动生成，所以为了避免挤掉原有的数据，所以用2道
int route_read = 1;
int route_write = 0;
//是否延迟发车
int whether_delay = 1;
//经度队列小数
float longitude_FQ[50] = {
    0.0,
};
//整数队列
int16 longitude_ZQ[50] = {
    0.0,
};

//纬度队列
float latitude_FQ[50] = {
    0,
};

float latitude_ZQ[50] = {
    0.0,
};
//GPS命令设置

//close gga
uint8 close_gga_cmd[] = {
    0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x00, 0x00, 0xFA, 0x0F};
//F1 D9 06 01 03 00 F0 00 00 FA 0F

//CLOSE ANT
uint8 close_ant_cmd[] = {
    0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x20, 0x00, 0x1A, 0x4F};
//F1 D9 06 01 03 00 F0 20 00 1A 4F

//close gsv
uint8 close_gsv_cmd[] = {
    0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x04, 0x00, 0xFE, 0x17};
//F1 D9 06 01 03 00 F0 04 00 FE 17

//close gsa
uint8 close_gsa_cmd[] = {
    0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x02, 0x00, 0xFC, 0x13};
//F1 D9 06 01 03 00 F0 02 00 FC 13

//close zda
uint8 close_zda_cmd[] = {
    0xF1, 0xD9, 0x06, 0x01, 0x03, 0x00, 0xF0, 0x07, 0x00, 0x01, 0x1D};
//F1 D9 06 01 03 00 F0 07 00 01 1D

//set10 hz
uint8 set_rate_10hz[] = {
    0xF1, 0xD9, 0x06, 0x42, 0x14, 0x00, 0x00, 0x0A, 0x05, 0x00, 0x64, 0x00, 0x00, 0x00,
    0x60, 0xEA, 0x00, 0x00, 0xD0, 0x07, 0x00, 0x00, 0xC8, 0x00, 0x00, 0x00, 0xB8, 0xED};
//F1 D9 06 42 14 00 00 0A 05 00 64 00 00 00 60 EA 00 00 D0 07 00 00 C8 00 00 00 B8 ED

void my_gps_init(void)
{
  // 初始化流控引脚  GPS模块
  gpio_init(A1, GPI, GPIO_HIGH, GPI_PULL_UP);											
  uart_init(UART_2, 115200 , UART2_TX_A2 , UART2_RX_A3);

  uart_putbuff(UART_2, (uint8 *)close_gga_cmd, sizeof(close_gga_cmd));
  uart_putbuff(UART_2, (uint8 *)close_gsa_cmd, sizeof(close_gsa_cmd));
  uart_putbuff(UART_2, (uint8 *)close_gsv_cmd, sizeof(close_gsv_cmd));

  uart_putbuff(UART_2, (uint8 *)close_zda_cmd, sizeof(close_zda_cmd));
  uart_putbuff(UART_2, (uint8 *)set_rate_10hz, sizeof(set_rate_10hz));
  uart_putbuff(UART_2, (uint8 *)close_ant_cmd, sizeof(close_ant_cmd));

  uart_rx_irq(UART_2, ENABLE);
}

//判断是否发生解析错误
int8 analysis_error = 0;
//报文发送
//数据保存
void parseGpsBuffer(void)
{
  char *subString;
  char *subStringNext;
  char i = 0;
  if (Save_Data.isGetData) //如果字符串不为空
  {
    for (i = 0; i <= 8; i++) //循环7次
    {
      if (i == 0)
      {
        if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL) //没有检测到逗号
        {
          analysis_error = 1;
        } //解析错误
        else
        {
          analysis_error = 0;
        }
      }
      else if (analysis_error == 0) //检测到逗号，返回逗号的位置
      {
        subString++;                                          //位置加1（定位到逗号的后一位）
        if ((subStringNext = strstr(subString, ",")) != NULL) //定位下一个逗号的位置
        {
          char usefullBuffer[2];
          switch (i)
          {
          case 1:
            memcpy(Save_Data.UTCTime, subString, subStringNext - subString); //两个逗号之间为 时间信息 并转换成北京时间
            Save_Data.UTCTime[1] = Save_Data.UTCTime[1] + 8;
            if (Save_Data.UTCTime[1] > '9')
            {
              Save_Data.UTCTime[0]++;
              if (Save_Data.UTCTime[0] == '3')
                Save_Data.UTCTime[0] = '0';
              Save_Data.UTCTime[1] = (Save_Data.UTCTime[1] % '9') + '0' - 1;
            }
            break; //结束switch
          case 2:
            memcpy(usefullBuffer, subString, subStringNext - subString);
            break; //数据是否有效标志
          case 3:
            memcpy(Save_Data.latitude, subString, subStringNext - subString);
            break; //获取纬度信息
          case 4:
            memcpy(Save_Data.N_S, subString, subStringNext - subString);
            break; //获取N/S
          case 5:
            memcpy(Save_Data.longitude, subString, subStringNext - subString);
            break; //获取经度信息
          case 6:
            memcpy(Save_Data.E_W, subString, subStringNext - subString);
            break; //获取E/W
          case 7:
            memcpy(Save_Data.ground_speed, subString, subStringNext - subString);
            break; //获取速度
          case 8:
            memcpy(Save_Data.ground_direction, subString, subStringNext - subString);
            break; //获取航向角
          default:
            break;
          }
          subString = subStringNext;    //下一个逗号位置给第一个指针，
          Save_Data.isParseData = true; //手动给真值，（数据是否解析完成）

          /*检测信息是否有效*/
          if (usefullBuffer[0] == 'A')
            Save_Data.isUsefull = true;
          else if (usefullBuffer[0] == 'V')
            Save_Data.isUsefull = false;
        }
        else
        {
          analysis_error = 1; //解析错误
        }
      }
    }
  }
}
//煮沸的方向获取
void zhufei_nmmsl()
{

  if (Save_Data.ground_direction[2] == '.')
  {
    Save_Data.f_ground_direction = (Save_Data.ground_direction[0] - '0') * 10 + (Save_Data.ground_direction[1] - '0') * 1 + (Save_Data.ground_direction[3] - '0') * 0.1 + (Save_Data.ground_direction[4] - '0') * 0.01 + (Save_Data.ground_direction[5] - '0') * 0.001;
  }
  else if (Save_Data.ground_direction[1] == '.')
  {
    Save_Data.f_ground_direction = (Save_Data.ground_direction[0] - '0') * 1 + (Save_Data.ground_direction[2] - '0') * 0.1 + (Save_Data.ground_direction[3] - '0') * 0.01 + (Save_Data.ground_direction[4] - '0') * 0.001;
  }
  else if (Save_Data.ground_direction[3] == '.')
  {
    Save_Data.f_ground_direction = (Save_Data.ground_direction[0] - '0') * 100 + (Save_Data.ground_direction[1] - '0') * 10 + (Save_Data.ground_direction[2] - '0') * 1 + (Save_Data.ground_direction[4] - '0') * 0.1 + (Save_Data.ground_direction[5] - '0') * 0.01;
  }
  else
  {
    Save_Data.f_ground_direction = 0;
  }

  //速度获取
  //煮沸科技

  if (Save_Data.ground_speed[2] == '.')
  {
    Save_Data.f_ground_speed = (Save_Data.ground_speed[0] - '0') * 10 + (Save_Data.ground_speed[1] - '0') * 1 + (Save_Data.ground_speed[3] - '0') * 0.1 + (Save_Data.ground_speed[4] - '0') * 0.01 + (Save_Data.ground_speed[5] - '0') * 0.001;
  }
  else if (Save_Data.ground_speed[1] == '.')
  {
    Save_Data.f_ground_speed = (Save_Data.ground_speed[0] - '0') * 1 + (Save_Data.ground_speed[2] - '0') * 0.1 + (Save_Data.ground_speed[3] - '0') * 0.01 + (Save_Data.ground_speed[4] - '0') * 0.001;
  }
  else if (Save_Data.ground_speed[3] == '.')
  {
    Save_Data.f_ground_speed = (Save_Data.ground_speed[0] - '0') * 100 + (Save_Data.ground_speed[1] - '0') * 10 + (Save_Data.ground_speed[2] - '0') * 1 + (Save_Data.ground_speed[4] - '0') * 0.1 + (Save_Data.ground_speed[5] - '0') * 0.01;
  }
  else
  {
    Save_Data.f_ground_speed = 0;
  }

  Save_Data.f_ground_speed = Save_Data.f_ground_speed * 1.85;
}

char Get_InitData(double *Lon_Z, double *Lon_X, double *Lat_Z, double *Lat_X)
{
  int32 num = 0;
  //无效数据直接略过
  if (Save_Data.isUsefull == false)
  {
    return 0;
  }
  //判断纬度值中是否有‘.'有:说明有数据 例如：3946.99715  变化范围在后六位中变化，最后一位不稳定可舍去。
  if ((strstr(Save_Data.latitude, ".")) != NULL)
  {
    *Lat_Z = (Save_Data.latitude[0] - '0') * 10 + (Save_Data.latitude[1] - '0') * 1;
    num = (Save_Data.latitude[2] - '0') * 1000000 + (Save_Data.latitude[3] - '0') * 100000 + (Save_Data.latitude[5] - '0') * 10000 + (Save_Data.latitude[6] - '0') * 1000 + (Save_Data.latitude[7] - '0') * 100 + (Save_Data.latitude[8] - '0') * 10 + (Save_Data.latitude[9] - '0');

    *Lat_X = num * 1.0 / 100000; //取值为39 和 46.99715
  }
  else
  {
    Save_Data.isUsefull = false;
  }

  //经度原理同上 例如：11628.32198
  if ((strstr(Save_Data.longitude, ".")) != NULL)
  {
    *Lon_Z = (Save_Data.longitude[0] - '0') * 100 + (Save_Data.longitude[1] - '0') * 10 + (Save_Data.longitude[2] - '0') * 1;

    num = (Save_Data.longitude[3] - '0') * 1000000 + (Save_Data.longitude[4] - '0') * 100000 + (Save_Data.longitude[6] - '0') * 10000 + (Save_Data.longitude[7] - '0') * 1000 + (Save_Data.longitude[8] - '0') * 100 + (Save_Data.longitude[9] - '0') * 10 + (Save_Data.longitude[9] - '0');

    *Lon_X = num * 1.0 / 100000; //取值为116 和 28.32198
  }
  else
  {
    Save_Data.isUsefull = false;
  }
  //处理ground_speed与ground_direction数据
  //问题，发送字符串格式会怎样？？
  //Save_Data.f_ground_direction = (Save_Data.ground_direction[0] - '0') * 100 + (Save_Data.ground_direction[1] - '0') * 10 + (Save_Data.ground_direction[2] - '0') * 1 + (Save_Data.ground_direction[4] - '0') * 0.1 + (Save_Data.ground_direction[5] - '0') * 0.01;

  //正点原子
  //Save_Data.f_ground_speed = (Save_Data.ground_speed[0] - '0') * 100 + (Save_Data.ground_speed[1] - '0') * 10 + (Save_Data.ground_speed[2] - '0') * 1 + (Save_Data.ground_speed[4] - '0') * 0.1 + (Save_Data.ground_speed[5] - '0') * 0.01;

  zhufei_nmmsl();

  return 1;
}

//把整数和小数部分化单位,之前是时分秒
void Trans_Data_Z_plus_X()
{
  //整数保留，小数除60再看。
  // Save_Data.lati_Z = Lat_Z;
  Save_Data.lati_X = Save_Data.lati_X / 60;

  Save_Data.longi_X = Save_Data.longi_X / 60;
  //Save_Data.longi_Z = Lon_Z ;

  Save_Data.f_lati = Save_Data.lati_X + Save_Data.lati_Z;
  Save_Data.f_longi = Save_Data.longi_X + Save_Data.longi_Z;
}

//求两点距离误差

double radians(double angle)
{
  return (angle * (PI / 180));
}

double sq(double a)
{
  return (a * a);
}
//误差较大 置信度不高
double GPSDistance(double lati1, double longi1, double lati2, double longi2)
{
  double lat1 = lati1;

  double long1 = longi1;

  double lat2 = lati2;

  double long2 = longi2;

  double delta = radians(long1 - long2);
  double sdlong = sin(delta);
  double cdlong = cos(delta);

  double rlat1 = radians(lat1);
  double rlat2 = radians(lat2);

  double slat1 = sin(rlat1);
  double clat1 = cos(rlat1);

  double slat2 = sin(rlat2);
  double clat2 = cos(rlat2);

  delta = (clat1 * slat2) - (slat1 * clat2 * cdlong);
  delta = sq(delta);
  delta += sq(clat2 * sdlong);
  delta = sqrt(delta);

  double denom = (slat1 * slat2) + (clat1 * clat2 * cdlong);
  delta = atan2(delta, denom);
  delta = delta * 6378137;

  return delta;
}

//求两点方位角
double GPSBearingAngle(double lati1, double longi1, double lati2, double longi2)
{ // The function that calculates the angle to destination
  // given our updated current GPS coordinates

  double lat1 = lati1;

  double long1 = longi1;

  double lat2 = lati2;

  double long2 = longi2;

  double y_angle = cos(lat2) * sin(long2 - long1);
  double x_angle = (cos(lat1) * sin(lat2)) - (sin(lat1) * cos(lat2) * cos(long2 - long1));

  double gpsAngleRads = atan2(y_angle, x_angle);                       // Angle is in radians
  double gpsAngleDegreesFloat = (360.00 + ((180 / PI) * gpsAngleRads)); // convert radians to degrees

  double angle = fmod(gpsAngleDegreesFloat, 360.00);

  return angle;
}

//求实际方位角与目标方位角之间的误差
void GpsAngleError()
{
  double delta = 0.0;
  //gps航向与陀螺仪航向
  //在操场，先用用gps角
  //第一个点用陀螺仪做
  if (passed_sections <= 0)
  {
      //利用GPS角度与陀螺仪角度
      delta = Use_Data.angle_next_heading_now - Data_Gyro.gyro_direction;
  }
  else
  {
     delta = Use_Data.angle_next_heading_now - Save_Data.f_ground_direction;
  }
 
  
  //针对mod360的换算
  if (fabs(delta) > 180)
  {
    if (delta < -180)
    {
      delta = 360 + (delta);
    }
    else
    {
      delta = -360 + (delta);
    }
  }
  else
  {
    delta = delta;
  }
 

      Use_Data.gps_angle_error_now = delta;
    
    
   
}



/********************************
换点处理
**********************************/
void GpsDotSelection()
{
      static int change_time = 0;
      static  int last_change = 0;
      
      //记录上次换点
      if (passed_sections == 0)
      {
          last_change = 0;
      }
      
      
      
      //求目标航向和两点航向的差值绝对值(区分三个航向)
      Use_Data.change_angle_delta = delta_heading_angle(Use_Data.angle_next_heading_now, Use_Data.TwoDotsDirec);
      
      //如果超过了角度范围
      if(area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA)
      {
        if ( Use_Data.change_angle_delta > 30.0   || Use_Data.distan_next_point_now < (distance_each_point_direct * 0.3))
        {
              change_point = 1;
        }
      }
      else
      {
        if ( Use_Data.change_angle_delta > 50.0   || Use_Data.distan_next_point_now < 1.50 )
        {
              change_point = 1;
        }
      }
      
    

    if (change_point > 0) 
    {
      if (0 < passed_sections < finished_gpspoints)
      {
        passed_sections++;
      }
      
   
      last_change = change_point;
      //换点完成
      change_point = 0;
      //要求重新读取
      dot_read = false;
      //归零
      change_time = 0;
      //编码器重新计数
      distance_from_lastpoint = 0;
    }
}
//求出两个航向之间所差角度的绝对值,用于换点操作
double delta_heading_angle(double base_angle, double aiming_angle)
{
  double delta = base_angle - aiming_angle;
  double solution = 0.0;

  if (fabs(delta) > 180)
  {
    if (delta < -180)
    {
      solution = 360 + (delta);
    }
    else
    {
      solution = 360 - (delta);
    }
  }
  else
  {
    solution = fabs(delta);
  }
  return solution;
}






  //初始时这边存在很大的逻辑漏洞
  //试一下关掉限幅
  /********************直道起步限幅****************/
  /*if ( (area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA) &&  ( passed_sections <= 0))
  {
    debug_flag = 1;
    //总共限幅
    //如果向右边太多
    if (Data_Gyro.angle_sum_area > 4.0)
    {
        if (delta > 0)
        {
          delta = 0.0;
        }
    }
    else if (Data_Gyro.angle_sum_area < - 4.0)
    {
        if (delta < 0)
        {
          delta = 0.0;
        }
    }
    


    //单次限幅+自纠偏
    if ( (delta + Data_Gyro.angle_sum_area)  > 4.0  )
    {
      Use_Data.gps_angle_error_now = 4.0 - Data_Gyro.angle_sum_area;
    }
    else if ( (delta + Data_Gyro.angle_sum_area) < -4.0 )
    {
      Use_Data.gps_angle_error_now = -4.0 - Data_Gyro.angle_sum_area;
    }
    else
    {
      Use_Data.gps_angle_error_now = delta;
    }
    
  }

    /****************直道限幅——后续****************/
  //如果在直线区域，单次打角改变压在20以内
 /* else if ( (area_tag == DIRECT_AREA || area_tag == ROUND_OUT_AREA) && (passed_sections >= 1 ) )
  {
    //总共限幅
    //如果向右边太多
    debug_flag = 2;
    if (Data_Gyro.angle_sum_area > direct_thres)
    {
        if (delta > 0)
        {
          delta = 0.0;
        }
    }
    else if (Data_Gyro.angle_sum_area < -direct_thres)
    {
        if (delta < 0)
        {
          delta = 0.0;
        }
    }
    


    //单次限幅+自纠偏
    if ((delta + Data_Gyro.angle_sum_area) > direct_thres)
    {
      Use_Data.gps_angle_error_now = direct_thres - Data_Gyro.angle_sum_area;
    }
    else if ((delta + Data_Gyro.angle_sum_area) < -direct_thres)
    {
      Use_Data.gps_angle_error_now = -direct_thres - Data_Gyro.angle_sum_area;
    }
    else
    {
      Use_Data.gps_angle_error_now = delta;
    }
    
  }
  else
  {
    Use_Data.gps_angle_error_now = delta;
  }*/





  









