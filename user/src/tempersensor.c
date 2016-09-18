/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: tempersensor.c
  Author: lq       Version : V1.00        Date: 2013.01.15
  Description:     温度控制相关功能的实现
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "config.h"

#if CONFIG_TEMPERATURE_MONITOR
#include "def.h"
#include "tempersensor.h"
#include "Adc.h"

const float fsAvg_Slope = 4.3;
const float fsV25  = 1.43;

TemperSensorStruct struTemperSensorPara;

/*******************************************************************************
* Function Name  : InitTemperSensorPara
* Description    : 初始化工作状态控制参数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitTemperSensorPara(void)
{
 	U16 i;

    struTemperSensorPara.TimePeriod = TEMPER_20MS_PERIOD;
	struTemperSensorPara.TimeCnt = SetTimePeriodProcess(struTemperSensorPara.TimePeriod);	

	struTemperSensorPara.AvgBufIndex = 0;

	for (i = 0; i < (TEMPER_AVGBUF_LEN + 1); i++)
	{
		struTemperSensorPara.AvgBuf[i] = 0xFFF;
	}

	struTemperSensorPara.AvgSum = (U32)0xFFF * 16;
    
    struTemperSensorPara.fsTemper = 25.0;	

    ResetADC1_TemperConvFinishFlag();
    
    ResetTemperOnFlag();    	   
}

/*******************************************************************************
* Function Name  : GetTemper
* Description    : 获取当前温度值
* Input          : None
* Output         : None
* Return         : 当前温度值
*******************************************************************************/
float GetTemper(void)
{
    return struTemperSensorPara.fsTemper;
}

/*******************************************************************************
* Function Name  : CalcuTemper
* Description    : 计算当前温度值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CalcuTemper(void)
{
	float fsVsense;
    U16 uiTempAvg;

    if (struTemperSensorPara.TimeCnt == GetLocalClockTickProcess())
	{
		struTemperSensorPara.TimeCnt = SetTimePeriodProcess(struTemperSensorPara.TimePeriod);


        /*lq ADCchannel8、14、15、16转换完成*/
        if (GetADC1_TemperConvFinishFlag())
        {
             ResetADC1_TemperConvFinishFlag();
        }
        else
        {
            return ;
        }
		
		//平滑滤波
		struTemperSensorPara.AvgSum -= struTemperSensorPara.AvgBuf[struTemperSensorPara.AvgBufIndex];
		struTemperSensorPara.AvgBuf[struTemperSensorPara.AvgBufIndex] = arrADC1_Val[3];
		struTemperSensorPara.AvgSum += (U32)arrADC1_Val[3];

		struTemperSensorPara.AvgBufIndex++;
		struTemperSensorPara.AvgBufIndex &= TEMPER_AVGBUF_LEN;

		uiTempAvg = (U16)(struTemperSensorPara.AvgSum >> 4);                         //lq 平均值        
        
        if (!GetTemperOnFlag())
        {
            if (struTemperSensorPara.AvgBufIndex >= TEMPER_AVGBUF_LEN)
            {
                SetTemperOnFlag();
            }
            
            struTemperSensorPara.fsTemper = 25.0;
        }
        else
        {
            /*lq 计算当前温度值*/
            fsVsense = GetTemperVolt(uiTempAvg);
        
            struTemperSensorPara.fsTemper = (1000.0 * (fsV25 - fsVsense) / fsAvg_Slope) + 25;     		    
        }                	
    }    
}

/*******************************************************************************
* Function Name  : GetTemperVolt
* Description    : 根据AD采样值计算传感器输出电压值
* Input          : usSampValue - AD采样值
* Output         : None
* Return         : 传感器输出电压值
*******************************************************************************/
float GetTemperVolt(const U16 usSampValue)
{
    //lq... 暂时使用公式计算，以后改用实测值
    return (3.3 * usSampValue) / 4096.0;                                          

}

/*******************************************************************************
* Function Name  : SetADC1_TemperConvFinishFlag
* Description    : 设置温度转换完成标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetADC1_TemperConvFinishFlag(void)
{
    struTemperSensorPara.ucADC1_TemperConvFinishFlag = TRUE;    
}

/*******************************************************************************
* Function Name  : ResetADC1_TemperConvFinishFlag
* Description    : 复位温度转换完成标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetADC1_TemperConvFinishFlag(void)
{
    struTemperSensorPara.ucADC1_TemperConvFinishFlag = FALSE;    
}

/*******************************************************************************
* Function Name  : GetADC1_TemperConvFinishFlag
* Description    : 设置温度转换完成标志
* Input          : None
* Output         : None
* Return         : 当前状态
*******************************************************************************/
U8 GetADC1_TemperConvFinishFlag(void)
{
    return struTemperSensorPara.ucADC1_TemperConvFinishFlag; 
}

/*******************************************************************************
* Function Name  : GetTemperOnFlag
* Description    : 获取温度值是否有效标志
* Input          : None
* Output         : None
* Return         : 当前状态
*******************************************************************************/
U8 GetTemperOnFlag(void)
{
    return struTemperSensorPara.ucTemperOnFlag;
}

/*******************************************************************************
* Function Name  : SetTemperOnFlag
* Description    : 设置温度值有效标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SetTemperOnFlag(void)
{
    struTemperSensorPara.ucTemperOnFlag = TRUE;
}

/*******************************************************************************
* Function Name  : ResetTemperOnFlag
* Description    : 复位温度值有效标志
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetTemperOnFlag(void)
{
    struTemperSensorPara.ucTemperOnFlag = FALSE;
}
#endif
