#ifndef _ROUTE_GENERATING_H
#define _ROUTE_GENERATING_H
#include "gps.h"
#include "stdlib.h"
#include <math.h>
#include "isr.h"
#include <stdlib.h>
#include <string.h>
#include "common.h"

void round_in_out();

//利用四个直弯切换点产生路径
typedef struct FourPoints
{
	//关键点纬度
	double kps_latitude;
	double kps_longitude;
	//点类型
	int kps_type;
	//0 表示起点，也就是第一个直弯切换点
}FourPoints;

extern FourPoints kps_hust_est_plygrnd[4];

#endif
