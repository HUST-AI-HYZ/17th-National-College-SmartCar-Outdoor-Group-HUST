#ifndef _GPS_H
#define _GPS_H

#include "common.h"
#include "general.h"
#include "parameter.h"
#define ALL_POINTS 190

#define DIRECT_AREA 0
#define ROUND_AREA 1
#define ROUND_IN_AREA 2
#define ROUND_OUT_AREA 3
#define COLLECTION_BY_HAND 5
double GPSBearingAngle(double lati1,double longi1,double lati2,double longi2);
double GPSDistance(double lati1,double longi1,double lati2,double longi2);
double delta_heading_angle(double base_angle , double aiming_angle);

typedef struct SaveData 
{
  char GPS_Buffer[128];         //完整数据
  char isGetData;		//是否获取到GPS数据
  char isParseData;	        //是否解析完成
  char UTCTime[11];		//UTC时间
  char latitude[11];		//纬度
  char N_S[2];		        //N/S
  char longitude[12];		//经度
  char E_W[2];		        //E/W
  char ground_speed[6];      //对地面速度
  char ground_direction[6];   //对地航向

  char isUsefull;		        //定位信息是否有效

  double lati_X ;           //纬度小数部分
  double lati_Z ;           //纬度整数部分

  double longi_X;
  double longi_Z;

  double f_longi;          //经过度分秒转换之后的经度值
  double f_lati;

  double f_ground_speed;
  double f_ground_direction;

} _SaveData;

typedef struct UseData  //实际的差值
{
  //int lat_err;          //纬度差
  //int lon_err;          //经度差
  //int lat_centre;       //弧形赛道中心点纬度
  //int lon_centre;       //弧形赛道中心点经度
  //float radius;         //圆弧的半径
  
  //float distan_centre;  //与圆心的距离
  //float lat_cent_err;   //与中心的纬度差
  //float lon_cent_err;   //与中心的经度差
  double lat_last;
  double lon_last;


  double lat_next;   //下一个点的纬度
  double lon_next;   //下一个点的经度


  double TwoDotsDis;   //前后两点的距离(固定值)
  double TwoDotsDirec;  //两点间的航向

  double angle_next_heading_now;  //当前点到下一个点的航向角
  double angle_nexttwo_heading_now;
  double distan_next_point_now;    //与下一个目标点的距离
  double gps_angle_error_now;   //实际角度与目标角度的偏差
  double change_angle_delta;
 
} _UseData;

//拆分存储
typedef struct  KeyPoint
{
  double lat_keypoint;
  double lon_keypoint;
  
  float lat_kypnt_f;   //纬度小数
  float lon_kypnt_f;   //经度
  int16 lat_kypnt_z;   //纬度整数
  int16 lon_kypnt_z;   //经度
  
  
  int16 speed_type;      //速度类型
  int16 type_point;       //点的类型
  //bool pass_by;         //是否通过 
  
}_KeyPoint;



void  Gps_Mokato(double B , double L , double * X , double * Y);
//停车距离
extern float stop_distance;
//目标圈数
extern uint16 circle_time;

extern _SaveData Save_Data;
extern _UseData Use_Data;

extern _KeyPoint Points_Stored[ALL_POINTS];
extern _KeyPoint Points_Use[ALL_POINTS + 3];
//微操判断，相当于换区域次数。
extern int16 area_tag;

//发车时间字符串，用于创建sdcard变量
extern char StartTime[12];


extern int16 passed_sections;
//gps_重启
//extern int16 gps_reset;
//GPS的模式
extern int16 gps_mode;
//选中的已采集的数据点的序号值
extern uint16 point_selected_gps;
//完成采集的数据点
extern uint16 finished_gpspoints;
//选中点的纬度
extern uint32 point_latitude_selected;
//选中点的经度
extern uint32 point_longitude_selected;
//从上一个点以来，走过的距离。(多少米一个点...)
extern float distance_from_lastpoint;
//每个点的间隔(直道路)
extern float distance_each_point_direct;
//每个点的间隔（弯道路）
extern float distance_each_point_round;

//行驶方向
//如果为正数，就是逆时针
extern int16 guiding_mode;
//估计的起点(如果不估计则从0开始)
extern uint16 guess_start;
//经度队列
extern double longitude_Q[50];
//纬度队列
extern double latitude_Q[50];
extern double angle_error_next[5];
//是否换点
extern int8 change_point;
//点类型
extern int16 point_type;
extern int16 point_type_pageread;
//区域类型
extern int8 now_area;
extern int8 last_area;
//icm调节
extern float error_icm_last;
//求当前点与真实起点的误差
extern float error_lenth_s_p;
//误差方向（当前点指向第0个点）
extern float error_direction;
//路线选择
extern int route_read;
extern int route_write;
//水平误差
extern float horizontal_error;
//GPS微操
//第一次切换处微操
#define ZONE 8
//三条赛道
#define ROUTE 4
extern int16 run_sections;
//是否延迟发车
extern int whether_delay;
extern int16 micro_op_lati[ROUTE][ZONE];
extern int16 micro_op_longi[ROUTE][ZONE];
//微操段
extern int16 micro_area;
//上一个微操段
extern int16  last_micro_area;
//直道阈值
extern uint16 direct_thres;
//直道换点角度
extern float direc_change_dis;
//弯道换点角度
extern float round_change_dis;
#endif 
