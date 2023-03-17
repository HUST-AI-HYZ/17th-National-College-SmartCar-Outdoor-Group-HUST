#include "flash.h"
#include "zf_driver_flash.h"
#include "gps.h"
float _PLACE_HOLDER_ = 0;

static Varible FLASH_VARS[FLASH_VAR_NUM] = FLASH_VAR_LIST;

void Data2Var(uint32 *data, Varible *var)
{
   
    if (*data == 0xFFFFFFFF)
        *data = 0;

    switch (var->type)
    {
    case TYPE_FLOAT:
        *(float *)(var->buff) = *(float *)(data);
        break;
    case TYPE_INT16:
        *(int16 *)(var->buff) = *(int16 *)(data);
        break;
    case TYPE_UINT16:
        *(uint16 *)(var->buff) = *(uint16 *)(data);
    default:
        break;
    }
}

void Var2Data(uint32 *data, Varible *var)
{
    switch (var->type)
    {
    case TYPE_FLOAT:
        *(float *)data = (int32)(*(float *)(var->buff) * 100) / 100.0;
        break;
    case TYPE_INT16:
        *(int16 *)data = *(int16 *)(var->buff);
        break;
    case TYPE_UINT16:
        *(uint16 *)data = *(uint16 *)(var->buff);
    default:
        break;
    }
}

void FlashReadParam(int16 number, Varible *vars)
{
    uint32 buff[FLASH_LENGTH];
    FLASH_PAGE_READ(buff);

    int16 index = 0;
    while (index < number)
    {
        Data2Var(buff + index, vars + index);
        index = index + 1;
    }
}

void FlashWriteParam(int16 number, Varible *vars)
{
    uint32 buff[FLASH_LENGTH];
    uint32 *d = buff;

    int16 index = 0;
    while (index < number)
    {
        Var2Data(d + index, vars + index);
        index = index + 1;
    }

    FLASH_PAGE_WRITE(buff);
}
void FlashWhitePoints()
{
    uint32  
            //纬度小数
            buff11[ALL_POINTS],
            buff12[ALL_POINTS],
            //纬度整数
            buff21[ALL_POINTS],
            buff22[ALL_POINTS],
            //经度小数
            buff31[ALL_POINTS],
            buff32[ALL_POINTS],
            //经度整数
            buff41[ALL_POINTS],
            buff42[ALL_POINTS ],
            //点的类型
            buff_type_pnts1[ALL_POINTS],
            buff_type_pnts2[ALL_POINTS];
            
 
    
    uint32 *d1 = buff11 , *d2 = buff12 , *d_type1 = buff_type_pnts1 ,*d_type2 = buff_type_pnts2;
    //纬度小数
    int16 index = 0;
    int j = 0;
    
      
    while (index < ALL_POINTS)
    {
      if(index < ALL_POINTS/2)
      {
       *(d1 + j) =  *(uint32 *)&(Points_Stored[index].lat_kypnt_f ) ;
      }
      else
      {
        *(d2 + j) =  *(uint32 *)&(Points_Stored[index].lat_kypnt_f ) ;
      }
         index = index + 1; 
         j++;
         
         if ( index == ALL_POINTS/2)
         {
           j = 0;
         }
    }
    
    
    
    //纬度整数
    index = 0,j = 0;
    d1 = buff21 , d2 = buff22; 
    while (index < ALL_POINTS)
    {
      if(index < ALL_POINTS/2)
      {
       *(d1 + j) =  (int32 )(Points_Stored[index].lat_kypnt_z) ;
      }
      else
      {
        *(d2 + j) =  (int32 )(Points_Stored[index].lat_kypnt_z) ;
      }
         index = index + 1; 
          j++;
          
          if ( index == ALL_POINTS/2)
         {
           j = 0;
         }
    }
    
    //经度小数
    index = 0,j = 0;
    d1 = buff31 , d2 = buff32;
    while (index < ALL_POINTS)
    {
      if(index < ALL_POINTS/2)
      {
       *(d1 + j) =  *(uint32 *)&(Points_Stored[index].lon_kypnt_f );
      }
      else
      {
        *(d2 + j) =  *(uint32 *)&(Points_Stored[index].lon_kypnt_f );
      }
         index = index + 1; 
          j++;
          
           if ( index == ALL_POINTS/2)
         {
           j = 0;
         }
          
    }
    
     //经度整数
    index = 0,j = 0;
    d1 = buff41 , d2 = buff42;
    while (index < ALL_POINTS)
    {
      if(index < ALL_POINTS/2)
      {
       *(d1 + j) =  *(uint32 *)&(Points_Stored[index].lon_kypnt_z );
      }
      else
      {
        *(d2 + j) =  *(uint32 *)&(Points_Stored[index].lon_kypnt_z );
      }
         index = index + 1; 
         j++;
          
           if ( index == ALL_POINTS/2)
         {
           j = 0;
         }
    }
    
      
      
    //数据点的类型
    index = 0,j = 0;
    while (index < ALL_POINTS)
    {
      
       if(index < ALL_POINTS/2)
      {
       *(d_type1 + j) =  (int32) (Points_Stored[index].type_point );
      }
      else
      {
        *(d_type2 + j) =  (int32) (Points_Stored[index].type_point );
      }
         index = index + 1; 
         j++;
          
           if ( index == ALL_POINTS/2)
         {
           j = 0;
         }
    }
    


 //114,112,110      91 92
 //109,108,107      90
    if (route_read == 0)
    {
      //旧纬小
      flash_write_page (FLASH_SECTION_114, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_109, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_write_page (FLASH_SECTION_114, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_write_page (FLASH_SECTION_109, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_write_page (FLASH_SECTION_114, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_write_page (FLASH_SECTION_109, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_write_page (FLASH_SECTION_114, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_write_page (FLASH_SECTION_109, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_write_page (FLASH_SECTION_115, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_115, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
    }
    else if (route_read == 1)
    {
        //旧纬小
      flash_write_page (FLASH_SECTION_112, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_108, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_write_page (FLASH_SECTION_112, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_write_page (FLASH_SECTION_108, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_write_page (FLASH_SECTION_112, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_write_page (FLASH_SECTION_108, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_write_page (FLASH_SECTION_112, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_write_page (FLASH_SECTION_108, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_write_page (FLASH_SECTION_113, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_113, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
    }
    else if (route_read == 2)
    {
       //旧纬小
      flash_write_page (FLASH_SECTION_110, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_107, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_write_page (FLASH_SECTION_110, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_write_page (FLASH_SECTION_107, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_write_page (FLASH_SECTION_110, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_write_page (FLASH_SECTION_107, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_write_page (FLASH_SECTION_110, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_write_page (FLASH_SECTION_107, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_write_page (FLASH_SECTION_111, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_111, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
    }
     else if (route_read == 3)
    {
       //旧纬小
      flash_write_page (FLASH_SECTION_91, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_90, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_write_page (FLASH_SECTION_91, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_write_page (FLASH_SECTION_90, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_write_page (FLASH_SECTION_91, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_write_page (FLASH_SECTION_90, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_write_page (FLASH_SECTION_91, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_write_page (FLASH_SECTION_90, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_write_page (FLASH_SECTION_92, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_write_page (FLASH_SECTION_92, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
    }

   

}  
void FlashWriteMicro()
{
     uint32  
            buff_micr_lati[ROUTE * ZONE],
            buff_micr_longi[ROUTE * ZONE];
    
    uint32  *d_micr_lati =  buff_micr_lati, *d_micr_longi =  buff_micr_longi;
    //纬度小数
    
     for (int i = 0;i < ROUTE;i++)
      for (int j = 0; j < ZONE;j++)
      {
        *(d_micr_lati + i * ZONE + j) =  (int32)(micro_op_lati [i][j]);
        *(d_micr_longi + i * ZONE + j) = (int32)(micro_op_longi [i][j]);
      }
  
      //写入116
    flash_write_page (FLASH_SECTION_116, FLASH_PAGE_0, buff_micr_lati, ROUTE * ZONE);
    flash_write_page (FLASH_SECTION_116, FLASH_PAGE_1, buff_micr_longi, ROUTE * ZONE);
}


void FlashReadMicro()
{
  uint32  buff_micr_lati[ROUTE * ZONE],buff_micr_longi[ROUTE * ZONE];
    //读取微操信息
  flash_read_page (FLASH_SECTION_116, FLASH_PAGE_0, buff_micr_lati, ROUTE * ZONE);
  flash_read_page (FLASH_SECTION_116, FLASH_PAGE_1, buff_micr_longi, ROUTE * ZONE);
  
   //C语言行优先
  for (int i = 0;i < ROUTE;i++)
    for (int j = 0; j < ZONE;j++)
    {
      micro_op_lati [i][j] =  *(int16 *)(buff_micr_lati + i*ZONE + j);
      micro_op_longi [i][j] =  *(int16 *)(buff_micr_longi + i*ZONE + j);
    }
  
}
 //114,112,110
 //109,108,107
void FlashReadPoints()
{
  uint32  
            //纬度小数
            buff11[ALL_POINTS],
            buff12[ALL_POINTS],
            //纬度整数
            buff21[ALL_POINTS],
            buff22[ALL_POINTS],
            //经度小数
            buff31[ALL_POINTS],
            buff32[ALL_POINTS],
            //经度整数
            buff41[ALL_POINTS],
            buff42[ALL_POINTS],
            //点的类型
            buff_type_pnts1[ALL_POINTS],
            buff_type_pnts2[ALL_POINTS];
            
  
  if (route_read == 0)
  {
    
     //旧纬小
      flash_read_page (FLASH_SECTION_114, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_109, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_read_page (FLASH_SECTION_114, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_read_page (FLASH_SECTION_109, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_read_page (FLASH_SECTION_114, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_read_page (FLASH_SECTION_109, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_read_page (FLASH_SECTION_114, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_read_page (FLASH_SECTION_109, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_read_page (FLASH_SECTION_115, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_115, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
  }
  else if (route_read == 1)
  {
    
      //旧纬小
      flash_read_page (FLASH_SECTION_112, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_108, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_read_page (FLASH_SECTION_112, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_read_page (FLASH_SECTION_108, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_read_page (FLASH_SECTION_112, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_read_page (FLASH_SECTION_108, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_read_page (FLASH_SECTION_112, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_read_page (FLASH_SECTION_108, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_read_page (FLASH_SECTION_113, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_113, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
     
  }
  else if (route_read == 2)
  {
    
      //旧纬小
      flash_read_page (FLASH_SECTION_110, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_107, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_read_page (FLASH_SECTION_110, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_read_page (FLASH_SECTION_107, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_read_page (FLASH_SECTION_110, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_read_page (FLASH_SECTION_107, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_read_page (FLASH_SECTION_110, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_read_page (FLASH_SECTION_107, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_read_page (FLASH_SECTION_111, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_111, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
      
  }
       else if (route_read == 3)
    {
       //旧纬小
      flash_read_page (FLASH_SECTION_91, FLASH_PAGE_0, buff11, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_90, FLASH_PAGE_0, buff12, ALL_POINTS);
      
      
      //旧纬整
      flash_read_page (FLASH_SECTION_91, FLASH_PAGE_1, buff21, ALL_POINTS);
      flash_read_page (FLASH_SECTION_90, FLASH_PAGE_1, buff22, ALL_POINTS);
      
      //旧经小
      flash_read_page (FLASH_SECTION_91, FLASH_PAGE_2, buff31, ALL_POINTS);
      flash_read_page (FLASH_SECTION_90, FLASH_PAGE_2, buff32, ALL_POINTS);
      
      //旧经整
      flash_read_page (FLASH_SECTION_91, FLASH_PAGE_3, buff41, ALL_POINTS);
      flash_read_page (FLASH_SECTION_90, FLASH_PAGE_3, buff42, ALL_POINTS);
      
      //旧类型
      flash_read_page (FLASH_SECTION_92, FLASH_PAGE_0, buff_type_pnts1, ALL_POINTS);
      //new
      flash_read_page (FLASH_SECTION_92, FLASH_PAGE_1, buff_type_pnts2, ALL_POINTS);
    }

  int16 j = 0,index = 0 ;
  
  while(j < ALL_POINTS)
  {     
      //如果说过半了
      if(j < ALL_POINTS / 2)
      {
         Points_Stored[j].lat_kypnt_f = *(float *)(buff11 + index);
         Points_Stored[j].lat_kypnt_z = *(int16 *)(buff21 + index);
         Points_Stored[j].lon_kypnt_f = *(float *)(buff31 + index);
         Points_Stored[j].lon_kypnt_z = *(int16 *)(buff41 + index);
         Points_Stored[j].type_point  = *(int16 *)(buff_type_pnts1 + index);
      }
      else
      {
         Points_Stored[j].lat_kypnt_f = *(float *)(buff12 + index);
         Points_Stored[j].lat_kypnt_z = *(int16 *)(buff22 + index);
         Points_Stored[j].lon_kypnt_f = *(float *)(buff32 + index);
         Points_Stored[j].lon_kypnt_z = *(int16 *)(buff42 + index);
         Points_Stored[j].type_point  = *(int16 *)(buff_type_pnts2 + index);
        
      }
        
      Points_Stored[j].lat_keypoint = Points_Stored[j].lat_kypnt_f + Points_Stored[j].lat_kypnt_z;
      Points_Stored[j].lon_keypoint = Points_Stored[j].lon_kypnt_f + Points_Stored[j].lon_kypnt_z;
        
      index = index + 1; 
       j = j + 1;  
       
       if (j == ALL_POINTS / 2)
      {
        index = 0;
      }
      
      
      
     
  }
  
 
  
}
void FlashReadParams()
{
    FlashReadParam(FLASH_VAR_NUM, FLASH_VARS);
    FlashReadMicro();
    FlashReadPoints();
    
}

void FlashWriteParams()
{
    FlashWriteParam(FLASH_VAR_NUM, FLASH_VARS);
    FlashWriteMicro();
    //允许点写回
    if (write_flash_points == 5)
    {
      FlashWhitePoints();
    }
}
