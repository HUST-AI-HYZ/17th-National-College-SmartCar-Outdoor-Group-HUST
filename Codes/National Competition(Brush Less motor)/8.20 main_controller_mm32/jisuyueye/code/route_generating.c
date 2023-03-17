#include "parameter.h"
#include "gps.h"
#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include "route_generating.h"
#include "button.h"
//用于保存产生的路径
typedef struct RPoints
{
    //纬度
    double rps_latitude;
    double rps_longitude;
    //点类型
    int rps_type;
} RPoints;
//暂时用double型保存，别的类型可以后续转换
int out1 = 84;
int out2 = 172;

FourPoints kps_plygrnd[4];
RPoints rps_plygrnd[150];

int generate_round_road(float step, FourPoints strt, FourPoints end, int* delt_dots);
int generate_direct_road(float step, FourPoints strt, FourPoints end, int* delt_dots);
int write_in(int delt_dots, double*dis, double*direction);




void generating_route_init()
{
		//南信工数据集（中间）0
	out1 = 78, out2 = 172;
	kps_plygrnd[3].kps_latitude = 32.20905;
	kps_plygrnd[3].kps_longitude = 118.72094;
	kps_plygrnd[3].kps_type = 3;

	kps_plygrnd[2].kps_latitude = 32.20828;
	kps_plygrnd[2].kps_longitude = 118.72105;
	kps_plygrnd[2].kps_type = 2;

	kps_plygrnd[1].kps_latitude = 32.20819;
	kps_plygrnd[1].kps_longitude = 118.72019;
	kps_plygrnd[1].kps_type = 1;

	kps_plygrnd[0].kps_latitude = 32.20894;
	kps_plygrnd[0].kps_longitude = 118.72008;
	kps_plygrnd[0].kps_type = 0;

	//南信工数据集（靠内）1
	out1 = 78, out2 = 172;
	kps_plygrnd[3].kps_latitude = 32.20905;
	kps_plygrnd[3].kps_longitude = 118.720925;
	kps_plygrnd[3].kps_type = 3;

	kps_plygrnd[2].kps_latitude = 32.20828;
	kps_plygrnd[2].kps_longitude = 118.721035;
	kps_plygrnd[2].kps_type = 2;

	kps_plygrnd[1].kps_latitude = 32.20819;
	kps_plygrnd[1].kps_longitude = 118.720205;
	kps_plygrnd[1].kps_type = 1;

	kps_plygrnd[0].kps_latitude = 32.20894;
	kps_plygrnd[0].kps_longitude = 118.720095;
	kps_plygrnd[0].kps_type = 0;
	
	 //每个点的间隔(直道路)
   distance_each_point_direct = 5.0;
   //每个点的间隔（弯道路）
   distance_each_point_round = 3.0;
}







void generating_route()
{
    //初始化数据
    //起点0
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
    //临时数据（中点）
    double temp_mid_point_lati = 0.0;
    double temp_mid_point_longi = 0.0;

    //处理的点数
    int delt_dots = 0;
    //校验用数组
    double exam_distance[200] = { 0.0 };
    double exam_direction[200] = { 0.0 };

    //处理第一段直道
    rps_plygrnd[0].rps_latitude = kps_plygrnd[0].kps_latitude;
    rps_plygrnd[0].rps_longitude = kps_plygrnd[0].kps_longitude;
    rps_plygrnd[0].rps_type = 0;
    generate_direct_road(distance_each_point_direct, kps_plygrnd[0], kps_plygrnd[1],&delt_dots);
    generate_round_road(distance_each_point_round, kps_plygrnd[1], kps_plygrnd[2], &delt_dots);
    generate_direct_road(distance_each_point_direct, kps_plygrnd[2], kps_plygrnd[3], &delt_dots);
    generate_round_road(distance_each_point_round, kps_plygrnd[3], kps_plygrnd[0], &delt_dots);

    //校验组
    for (size_t i = 0; i < delt_dots + 1; i++)
    {
        exam_distance[i] = GPSDistance(rps_plygrnd[i].rps_latitude, rps_plygrnd[i].rps_longitude, rps_plygrnd[i + 1].rps_latitude, rps_plygrnd[i + 1].rps_longitude);
        exam_direction[i] = GPSBearingAngle(rps_plygrnd[i].rps_latitude, rps_plygrnd[i].rps_longitude, rps_plygrnd[i + 1].rps_latitude, rps_plygrnd[i + 1].rps_longitude);
    }

    write_in(delt_dots, exam_distance, exam_direction);
    //分配区域
    round_in_out();
}

//角度解算要注意360这个条件

int generate_round_road(float step, FourPoints strt, FourPoints end,int *delt_dots)
{
    double mid_lati = (strt.kps_latitude + end.kps_latitude) / 2;
    double mid_longi = (strt.kps_longitude + end.kps_longitude) / 2;
    double radius = GPSDistance(mid_lati, mid_longi, strt.kps_latitude, strt.kps_longitude);
    //计算角度改变量,sina≈a,注意弧度值和角度值转换
    double delta_sita = (step / radius) * 180 / PI;
    //计算次数
    int times = (int)(180 / delta_sita);
    //重新算角度
    delta_sita = 180 * 1.0 / times;
    double vector_lati = 0.0;
    double vector_longi = 0.0;
    //计算中点与右侧点的向量
    //从右到左

    vector_lati = strt.kps_latitude - mid_lati;
    vector_longi = strt.kps_longitude - mid_longi;

    //有没有过半
    int passbyhalf = 0;
    //与之垂直的向量
    double verti_vector_lati = vector_longi;
    double verti_vector_longi = -vector_lati;
    //方向向量
    //角度比率，这次使用搜索法
    double a_ratio = 0.001;
    double dis_ratio = 0.001;
    double error_ratio = 0;
    double direction_vector_lati = vector_lati + verti_vector_lati * a_ratio;
    double direction_vector_longi = vector_longi + verti_vector_longi * a_ratio;
    //圆直径基准角
    double basic_angle = 0.0;
    //从右到左
    basic_angle = GPSBearingAngle(mid_lati, mid_longi, strt.kps_latitude, strt.kps_longitude);

    //退出方式
    bool out_way = true;

    int i = 1;

    //划分后的距离(调试用)
    double angle_sum = 0;
    double gene_dis1[80] = {0.0};
    double gene_dis2[80] = { 0.0 };
    double gene_dis3[80] = { 0.0 };
    double di_angle[80] = { 0.0 };

    //水平向量方向

    while (i < times + 1)
    {
        //第i个点与圆心的方位角
        double dir_angle = GPSBearingAngle(mid_lati, mid_longi, mid_lati + direction_vector_lati, mid_longi + direction_vector_longi);
        //改变量(过360那个坎)
        double change_angle = delta_heading_angle(basic_angle, dir_angle);

        while (change_angle < angle_sum + delta_sita)
        {
            if (passbyhalf == 0)
            {
                direction_vector_lati = vector_lati + verti_vector_lati * a_ratio;
                direction_vector_longi = vector_longi + verti_vector_longi * a_ratio;
                a_ratio = a_ratio + 0.001;
            }
            else
            {
                direction_vector_lati = - vector_lati + verti_vector_lati * a_ratio;
                direction_vector_longi = - vector_longi + verti_vector_longi * a_ratio;
                a_ratio = a_ratio - 0.001;
            }
            //第i个点与圆心的方位角
            dir_angle = GPSBearingAngle(mid_lati, mid_longi, mid_lati + direction_vector_lati, mid_longi + direction_vector_longi);
            //改变量
            change_angle = delta_heading_angle(basic_angle, dir_angle);
        }

        //到中心点的距离
        double dis_tomidpnt = GPSDistance(mid_lati, mid_longi, mid_lati + direction_vector_lati* dis_ratio, mid_longi + direction_vector_longi* dis_ratio);

        while (dis_tomidpnt < radius)
        {
            dis_ratio = dis_ratio + 0.001;
            dis_tomidpnt = GPSDistance(mid_lati, mid_longi, mid_lati + direction_vector_lati * dis_ratio, mid_longi + direction_vector_longi * dis_ratio);
        }
        //第一次分析
        rps_plygrnd[*delt_dots + i].rps_latitude = mid_lati + direction_vector_lati * dis_ratio;
        rps_plygrnd[*delt_dots + i].rps_longitude = mid_longi + direction_vector_longi * dis_ratio;
        rps_plygrnd[*delt_dots + i].rps_type = 1;


        gene_dis1[i] = GPSDistance(rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude, rps_plygrnd[*delt_dots + i - 1].rps_latitude, rps_plygrnd[*delt_dots + i - 1].rps_longitude);

        //如果距离出现问题
        if (gene_dis1[i] < step * 0.90 || gene_dis1[i] > step * 1.10)
        {
            error_ratio = step / gene_dis1[i];

            //再来一次
            rps_plygrnd[*delt_dots + i].rps_latitude = rps_plygrnd[*delt_dots + i - 1].rps_latitude + (rps_plygrnd[*delt_dots + i].rps_latitude - rps_plygrnd[*delt_dots + i - 1].rps_latitude) * error_ratio;
            rps_plygrnd[*delt_dots + i].rps_longitude = rps_plygrnd[*delt_dots + i - 1].rps_longitude + (rps_plygrnd[*delt_dots + i].rps_longitude - rps_plygrnd[*delt_dots + i - 1].rps_longitude) * error_ratio;
            rps_plygrnd[*delt_dots + i].rps_type = 1;
        }
        else
        {

        }
        di_angle[i] = GPSBearingAngle(mid_lati, mid_longi, rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude);
        angle_sum = delta_heading_angle(basic_angle, di_angle[i]);

        gene_dis2[i] = GPSDistance(rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude, rps_plygrnd[*delt_dots + i - 1].rps_latitude, rps_plygrnd[*delt_dots + i - 1].rps_longitude);
        gene_dis3[i] = GPSDistance(rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude, mid_lati, mid_longi);
        dis_ratio = 0.001;

        //平时75 170，南信工 84 172
        if (delta_heading_angle(basic_angle, di_angle[i]) > out1)
        {
            passbyhalf = 1;
        }

        if (delta_heading_angle(basic_angle, di_angle[i]) > out2)
        {
            out_way = false;
            break;
        }

        i++;

    }

    if (out_way == true)
    {
        i--;
    }

    *delt_dots = *delt_dots + i;
    double dis_l = GPSDistance(end.kps_latitude, end.kps_longitude, rps_plygrnd[*delt_dots].rps_latitude, rps_plygrnd[*delt_dots].rps_longitude);

    if (dis_l < distance_each_point_round)
    {
        rps_plygrnd[*delt_dots].rps_latitude = end.kps_latitude;
        rps_plygrnd[*delt_dots].rps_longitude = end.kps_longitude;
        rps_plygrnd[*delt_dots].rps_type = 1;
    }
    else
    {
        rps_plygrnd[*delt_dots].rps_latitude = (end.kps_latitude + rps_plygrnd[*delt_dots - 1].rps_latitude)*0.5;
        rps_plygrnd[*delt_dots].rps_longitude = (end.kps_longitude + rps_plygrnd[*delt_dots - 1].rps_longitude)*0.5;
        rps_plygrnd[*delt_dots].rps_type = 1;
        *delt_dots = *delt_dots + 1;

        rps_plygrnd[*delt_dots].rps_latitude = end.kps_latitude ;
        rps_plygrnd[*delt_dots].rps_longitude = end.kps_longitude;
        rps_plygrnd[*delt_dots].rps_type = 1;
    }

    return 1;
}









int generate_direct_road(float step, FourPoints strt, FourPoints end, int* delt_dots)
{
    //按照逆时针行进方向
    if (! ((strt.kps_type == 0 && end.kps_type == 1)  || (strt.kps_type == 2 && end.kps_type == 3)) )
    {
        //表示运行错误
        return 0;
    }
    else
    {

    }

    //计算两点间距离，进行划分
    double dis = 0.0;
    dis = GPSDistance(strt.kps_latitude, strt.kps_longitude, end.kps_latitude, end.kps_longitude);
    //计算区域数量，计算划分情况
    double minstep = dis / (step * 1.05);
    double maxstep = dis / (step * 0.95);
    int stepnumber = 0;
    //求差值
    //如果比较小
    if (maxstep - minstep >= 1.0)
    {
        stepnumber = (int)((maxstep + minstep)/2);
    }
    else
    {
        stepnumber = (int)((maxstep)/2);
    }
    //等分划分
    int i = 1;
    //划分后的距离(调试用)
    double gene_dis1[50] = {0.0};
    double gene_dis2[50] = { 0.0 };

    //每一小段
    double lati_step = (end.kps_latitude - strt.kps_latitude) / stepnumber;
    double longi_step = (end.kps_longitude - strt.kps_longitude) / stepnumber;
    while (i < stepnumber)
    {
        rps_plygrnd[*delt_dots + i].rps_latitude = rps_plygrnd[*delt_dots + i - 1].rps_latitude + lati_step;
        rps_plygrnd[*delt_dots + i].rps_longitude = rps_plygrnd[*delt_dots + i - 1].rps_longitude + longi_step;
        rps_plygrnd[*delt_dots + i].rps_type = 0;

        gene_dis1[i] = GPSDistance(rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude, end.kps_latitude, end.kps_longitude);
        gene_dis2[i] = GPSDistance(rps_plygrnd[*delt_dots + i].rps_latitude, rps_plygrnd[*delt_dots + i].rps_longitude, rps_plygrnd[*delt_dots + i - 1].rps_latitude, rps_plygrnd[*delt_dots + i - 1].rps_longitude);
        i++;
    }
    //最后一个直接连接上
    rps_plygrnd[*delt_dots + i].rps_latitude = end.kps_latitude;
    rps_plygrnd[*delt_dots + i].rps_longitude = end.kps_longitude;
    rps_plygrnd[*delt_dots + i].rps_type = 0;

    *delt_dots = *delt_dots + i;
    return 1;
}

int write_in(int delt_dots, double* dis, double* direction)
{
    finished_gpspoints = delt_dots;

    for (int i = 0; i < delt_dots; i++)
    {
        int16 hund = 0, ten = 0;
        //double部分
        Points_Stored[i].lon_keypoint = rps_plygrnd[i].rps_longitude;
        Points_Stored[i].lat_keypoint = rps_plygrnd[i].rps_latitude;

        //整数小数分离
        ten = (int)(rps_plygrnd[i].rps_latitude /10);
        Points_Stored[i].lat_kypnt_z =  ten * 10 + (int)(rps_plygrnd[i].rps_latitude /1 - (ten * 10 ) ) ;
        Points_Stored[i].lat_kypnt_f = rps_plygrnd[i].rps_latitude - Points_Stored[i].lat_kypnt_z;
        //整数小数分离
        hund = (int)(rps_plygrnd[i].rps_longitude /100);
        ten = (int)(rps_plygrnd[i].rps_longitude /10) - hund * 10;
        Points_Stored[i].lon_kypnt_z = hund*100 + ten*10 + (int)(rps_plygrnd[i].rps_longitude - (hund*100 + ten*10));
        Points_Stored[i].lon_kypnt_f = rps_plygrnd[i].rps_longitude - Points_Stored[i].lon_kypnt_z;
        //点类型
        Points_Stored[i].type_point = rps_plygrnd[i].rps_type;
    }


}

//自动规划函数
//明确起跑正逆时针方向、根据猜测点算出最近的数据点作为点0来处理，根据Points_Store来对目标点进行自动化排序，跑完一圈后连接到起点并添加几个尾点以免提前停车
//记得检查
void gps_auto_plannation(void)
{
#define PNTNUM 4
    double dis_test_1[PNTNUM]= {0,};
    double dis_test_2[PNTNUM]= {0,};
    int8 incre_1[PNTNUM - 1]= {0};
    int8 incre_2[PNTNUM - 1]= {0};
    int8 incre1_sum = 0;
    int8 incre2_sum = 0;

    //如果有距离小于20的，就直接暴力搜索
    double guess_dis = 10000.0;
    //最小值
    double min_dis = 10000.0;
    //对应起点编号
    int16 min_num = -1;
    //起搜点
    uint16 search_start = 0;
    //终止点
    uint16 search_end = 0;

    //防止越界
    if (guess_start < 0)
    {
        guess_start = 0;
    }
    else if (guess_start >= finished_gpspoints)
    {
        guess_start = finished_gpspoints - 1;
    }
    //初始距离
    min_dis = GPSDistance(Save_Data.f_lati, Save_Data.f_longi,Points_Stored[guess_start].lat_keypoint, Points_Stored[guess_start].lon_keypoint);
    min_num = guess_start;
    //正向暴力法
    for (int i = 1 ; i < finished_gpspoints ; )
    {
        //进制问题
        if (guess_start + i > finished_gpspoints - 1)
        {
            search_start = - (finished_gpspoints - guess_start - 0) + i;
        }
        else
        {
            search_start = guess_start + i;
        }
        double temp = GPSDistance(Save_Data.f_lati, Save_Data.f_longi,Points_Stored[search_start].lat_keypoint, Points_Stored[search_start].lon_keypoint);

        if( temp < min_dis )
        {
            min_dis = GPSDistance(Save_Data.f_lati, Save_Data.f_longi,Points_Stored[search_start].lat_keypoint, Points_Stored[search_start].lon_keypoint);
            min_num = search_start;
        }
        //步进方式
        if(i < ALL_POINTS - 8)
        {
            if (temp < 20.0)
            {
                i = i + 1;
            }
            else if(temp >= 20.0 && temp < 50.0)
            {
                i = i + 3;
            }
            else
            {
                i = i + 5;
            }
        }
        else
        {
            i = i + 1;
        }

    }
    //航向判断
    //逆时针
    double angle1 = GPSBearingAngle(Save_Data.f_lati, Save_Data.f_longi,Points_Stored[min_num].lat_keypoint, Points_Stored[min_num].lon_keypoint);
    double angle2 = 0;
    if( (min_num + 1) < finished_gpspoints)
    {
        angle2 = GPSBearingAngle(Points_Stored[min_num].lat_keypoint, Points_Stored[min_num].lon_keypoint,Points_Stored[min_num + 1].lat_keypoint, Points_Stored[min_num + 1].lon_keypoint);
    }
    else
    {
        angle2 = GPSBearingAngle(Points_Stored[min_num].lat_keypoint, Points_Stored[min_num].lon_keypoint,Points_Stored[0].lat_keypoint, Points_Stored[0].lon_keypoint);
    }

    //同一方向，那么最近点设置为0点
    int last_one = 0;
    if( fabs(delta_heading_angle(angle1,angle2) < 100) )
    {
        Points_Use[0].lat_keypoint = Points_Stored[min_num].lat_keypoint;
        Points_Use[0].lon_keypoint =  Points_Stored[min_num].lon_keypoint;
        Points_Use[0].speed_type = Points_Stored[min_num].speed_type;
        //自动填充
        //逆时针
        //finished_gpspoints指点数量
        for (int i = 1; i < finished_gpspoints ; i ++)
        {
            int temp_pt = 0;
            if (min_num + i > finished_gpspoints - 1)
            {
                temp_pt = - (finished_gpspoints - min_num - 0) + i;
            }
            else
            {
                temp_pt = min_num + i;
            }
            Points_Use[i].lat_keypoint = Points_Stored[temp_pt].lat_keypoint;
            Points_Use[i].lon_keypoint =  Points_Stored[temp_pt].lon_keypoint;
            Points_Use[i].type_point = Points_Stored[temp_pt].type_point;
						Points_Use[i].speed_type = Points_Stored[temp_pt].speed_type;
            last_one =  temp_pt;

        }
    }
    //怕第一段距离太短了，所以最小点的下一个点作为0点
    else
    {
        //第一个点的编号
        int first_num = 0;
        if( (min_num + 1) < finished_gpspoints)
        {
            Points_Use[0].lat_keypoint = Points_Stored[min_num + 1].lat_keypoint;
            Points_Use[0].lon_keypoint =  Points_Stored[min_num + 1].lon_keypoint;
            Points_Use[0].type_point =  Points_Stored[min_num + 1].type_point;
					  Points_Use[0].speed_type = Points_Stored[min_num + 1].speed_type;
            first_num = min_num + 1;
        }
        else
        {
            Points_Use[0].lat_keypoint = Points_Stored[0].lat_keypoint;
            Points_Use[0].lon_keypoint =  Points_Stored[0].lon_keypoint;
            Points_Use[0].type_point = Points_Stored[0].type_point;
					  Points_Use[0].speed_type = Points_Stored[0].speed_type;
            first_num = 0;
        }


        //自动填充
        //逆时针
        //finished_gpspoints指点数量
        for (int i = 1; i < finished_gpspoints ; i ++)
        {
            int temp_pt = 0;
            if (first_num + i > finished_gpspoints - 1)
            {
                temp_pt = - (finished_gpspoints - first_num - 0) + i;
            }
            else
            {
                temp_pt = first_num + i;
            }
            Points_Use[i].lat_keypoint = Points_Stored[temp_pt].lat_keypoint;
            Points_Use[i].lon_keypoint =  Points_Stored[temp_pt].lon_keypoint;
            Points_Use[i].type_point = Points_Stored[temp_pt].type_point;
						Points_Use[i].speed_type = Points_Stored[temp_pt].speed_type;
            last_one = temp_pt;
        }
    }
    
    //补充安全距离(4个点)
    for (int i = 1; i < 4 ; i ++)
    {
        int temp_pt = 0;
        if (last_one + i >= finished_gpspoints)
        {
            temp_pt = - (finished_gpspoints - last_one - 0) + i;
        }
        else
        {
            temp_pt = last_one + i;
        }
        Points_Use[finished_gpspoints + i - 1].lat_keypoint = Points_Stored[temp_pt].lat_keypoint;
        Points_Use[finished_gpspoints + i - 1].lon_keypoint =  Points_Stored[temp_pt].lon_keypoint;
        Points_Use[finished_gpspoints + i - 1].type_point = Points_Stored[temp_pt].type_point;
				Points_Use[finished_gpspoints + i - 1].speed_type = Points_Stored[temp_pt].speed_type;
    }

    if (finished_gpspoints + 3 > ALL_POINTS + 3)
    {
        tft180_show_string(0, 7 * 16, "overflow");
    }
    //采集到的如果是60个的话
    //run的时候我们有63个

}

//允许少量微操
void micro_operating()
{
    int time_tag = 0;
    int j =0;
    for (int i = 0; i < finished_gpspoints; i++)
    {
        //如果出现了nan，直接退出
        if (isnan(Points_Stored[i].lat_keypoint))
        {
            break;
        }



        //判断要不要切换
        if (time_tag == 0 && Points_Stored[i].type_point ==DIRECT_AREA )
        {
            time_tag = 1;
            j ++;
        }
        else if (time_tag == 1 && Points_Stored[i].type_point ==ROUND_IN_AREA )
        {
            time_tag = 2;
            j++;
        }
        else if (time_tag == 2 && Points_Stored[i].type_point ==ROUND_AREA )
        {
            time_tag = 3;
            j++;
        }
        else if (time_tag == 3 && Points_Stored[i].type_point ==ROUND_OUT_AREA )
        {
            time_tag = 4;
            j++;
        }
        else if (time_tag == 4 && Points_Stored[i].type_point ==DIRECT_AREA )
        {
            time_tag = 5;
            j++;
        }
        else if (time_tag == 5 && Points_Stored[i].type_point ==ROUND_IN_AREA )
        {
            time_tag = 6;
            j++;
        }
        else if (time_tag == 6 && Points_Stored[i].type_point ==ROUND_AREA )
        {
            time_tag = 7;
            j++;
        }
        else if (time_tag == 7 && Points_Stored[i].type_point ==ROUND_OUT_AREA )
        {
            Points_Stored[i].lat_keypoint += micro_op_lati[route_read][0]/ 1000000.0;
            Points_Stored[i].lon_keypoint += micro_op_longi[route_read][0]/ 1000000.0;
            Points_Stored[i].lat_kypnt_f +=  micro_op_lati[route_read][0]/ 1000000.0;
            Points_Stored[i].lon_kypnt_f +=  micro_op_longi[route_read][0]/ 1000000.0;
            Points_Stored[i].speed_type = 0;
            break;
        }



        if (time_tag == j )
        {
            Points_Stored[i].lat_keypoint += micro_op_lati[route_read][j]/ 1000000.0;
            Points_Stored[i].lon_keypoint += micro_op_longi[route_read][j]/ 1000000.0;
            Points_Stored[i].lat_kypnt_f +=  micro_op_lati[route_read][j]/ 1000000.0;
            Points_Stored[i].lon_kypnt_f +=  micro_op_longi[route_read][j]/ 1000000.0;
            Points_Stored[i].speed_type = j;
        }



    }

}

//微操恢复
void micro_operating_recover()
{
    int time_tag = 0;
    int j = 0;
    for (int i = 0; i < finished_gpspoints; i++)
    {
        //如果出现了nan，直接退出
        if (isnan(Points_Stored[i].lat_keypoint))
        {
            break;
        }

        //判断要不要切换
        if (time_tag == 0 && Points_Stored[i].type_point ==DIRECT_AREA )
        {
            time_tag = 1;
            j ++;
        }
        else if (time_tag == 1 && Points_Stored[i].type_point ==ROUND_IN_AREA )
        {
            time_tag = 2;
            j++;
        }
        else if (time_tag == 2 && Points_Stored[i].type_point ==ROUND_AREA )
        {
            time_tag = 3;
            j++;
        }
        else if (time_tag == 3 && Points_Stored[i].type_point ==ROUND_OUT_AREA )
        {
            time_tag = 4;
            j++;
        }
        else if (time_tag == 4 && Points_Stored[i].type_point ==DIRECT_AREA )
        {
            time_tag = 5;
            j++;
        }
        else if (time_tag == 5 && Points_Stored[i].type_point ==ROUND_IN_AREA )
        {
            time_tag = 6;
            j++;
        }
        else if (time_tag == 6 && Points_Stored[i].type_point ==ROUND_AREA )
        {
            time_tag = 7;
            j++;
        }
        else if (time_tag == 7 && Points_Stored[i].type_point ==ROUND_OUT_AREA )
        {
            Points_Stored[i].lat_keypoint -= micro_op_lati[route_read][0]/ 1000000.0;
            Points_Stored[i].lon_keypoint -= micro_op_longi[route_read][0]/ 1000000.0;
            Points_Stored[i].lat_kypnt_f -=  micro_op_lati[route_read][0]/ 1000000.0;
            Points_Stored[i].lon_kypnt_f -=  micro_op_longi[route_read][0]/ 1000000.0;
            break;
        }


        if (time_tag == j )
        {
            Points_Stored[i].lat_keypoint -= micro_op_lati[route_read][j]/ 1000000.0;
            Points_Stored[i].lon_keypoint -= micro_op_longi[route_read][j]/ 1000000.0;
            Points_Stored[i].lat_kypnt_f -=  micro_op_lati[route_read][j]/ 1000000.0;
            Points_Stored[i].lon_kypnt_f -=  micro_op_longi[route_read][j]/ 1000000.0;
        }



    }

}


//区域划分
void round_in_out()
{
    int last_type_tag = Points_Stored[0].type_point;
    //确定
    for (int i = 0; i < finished_gpspoints; i++)
    {
        //如果出现了nan，直接退出
        if (isnan(Points_Stored[i].lat_keypoint))
        {
            break;
        }
        //判断要不要切换
        if (last_type_tag != Points_Stored[i].type_point)
        {
            if (Points_Stored[i].type_point == 1 && last_type_tag == 0)
            {
							  Points_Stored[i - 1].type_point = ROUND_IN_AREA;
							  for (int j = 0; j <= 8; j++)
                {							
                   Points_Stored[i + j].type_point = ROUND_IN_AREA;
                }
							  i = i + 8;
                last_type_tag = 1;
            }
            else
            {
							if(i != 0)
							{
                Points_Stored[i].type_point = ROUND_OUT_AREA;
								for (int j = 0; j <= 6; j++)
								{
									Points_Stored[i - j].type_point = ROUND_OUT_AREA;
								}
                last_type_tag = 0;
							}
            }
        }
        else
        {
            last_type_tag = Points_Stored[i].type_point;
        }
    }

    Points_Stored[0].type_point = ROUND_OUT_AREA;
		for (int j = 0; j <= 6; j++)
		{
			Points_Stored[finished_gpspoints - j].type_point = ROUND_OUT_AREA;
		}
}

//人工踩点法
void collection_by_hand()
{
    static bool collect_flag = false;

    if (button_mod % 2 == 1 && collect_flag == false)
    {
        if (gpio_get(SWITCH_4) == 0)
        {
            if(Save_Data.lati_Z == 32 && Save_Data.longi_Z == 118)
            {
                Points_Stored[finished_gpspoints].lat_keypoint =  Save_Data.f_lati;
                Points_Stored[finished_gpspoints].lon_keypoint = Save_Data.f_longi;

                Points_Stored[finished_gpspoints].lat_kypnt_f = Save_Data.lati_X;
                Points_Stored[finished_gpspoints].lon_kypnt_f = Save_Data.longi_X;
                Points_Stored[finished_gpspoints].lat_kypnt_z = Save_Data.lati_Z;
                Points_Stored[finished_gpspoints].lon_kypnt_z = Save_Data.longi_Z;

                Points_Stored[finished_gpspoints].type_point = area_tag;
                finished_gpspoints++;
                collect_flag = true;
                tft180_show_string(0, 16, "SAVEPNTS");
                FlashWhitePoints();
            }
        }
    }
    //gps_mode == 5单独保存
    else if (button_mod % 2 == 0 && collect_flag == true)
    {
        if (gpio_get(SWITCH_4) == 0)
        {
            if(Save_Data.lati_Z == 32 && Save_Data.longi_Z == 118)
            {
                Points_Stored[finished_gpspoints].lat_keypoint =  Save_Data.f_lati;
                Points_Stored[finished_gpspoints].lon_keypoint = Save_Data.f_longi;

                Points_Stored[finished_gpspoints].lat_kypnt_f = Save_Data.lati_X;
                Points_Stored[finished_gpspoints].lon_kypnt_f = Save_Data.longi_X;
                Points_Stored[finished_gpspoints].lat_kypnt_z = Save_Data.lati_Z;
                Points_Stored[finished_gpspoints].lon_kypnt_z = Save_Data.longi_Z;

                Points_Stored[finished_gpspoints].type_point = area_tag;

                finished_gpspoints++;
                collect_flag = false;
                tft180_show_string(0, 16, "SAVEPNTS");
                FlashWhitePoints();
            }
        }

    }
}
//新的数据来临时，删掉旧的微操数据
void micro_delete()
{
    //旧有的微操删除
    for (int i = 0 ; i < ZONE ; i++)
    {
        micro_op_lati[route_read][i] = 0;
        micro_op_longi[route_read][i] = 0;
    }
    FlashWriteMicro();
}