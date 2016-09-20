/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: RxTx.c
  Author: lq       Version : V1.00        Date: 2014.05.06
  Description:     收发控制
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "Include.h"

/*******************************************************************************
* 名称  : RxTxSwitch
* 描述  : 收发转换
* 输入  : SwitchType-转换类型		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RxTxSwitch(U8 SwitchType)
{
    if (SwitchType == RXTX_SWITCH_TYPE_TX_TO_RX)
    {
        RX_EN_ON();
        ANT_SW_RX();    
    }
    else
    {
        RX_EN_OFF();
        ANT_SW_TX();            
    }        
}

/*******************************************************************************
* 名称  : InitRxTx
* 描述  : 收发初始化
* 输入  : 无        	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitRxTx(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_InitStructure.GPIO_Pin = RX_EN_PIN;
	GPIO_Init(RX_EN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = ANT_SW_PIN;
	GPIO_Init(ANT_SW_PORT, &GPIO_InitStructure);
     
    RxTxSwitch(RXTX_SWITCH_TYPE_TX_TO_RX);        
}
