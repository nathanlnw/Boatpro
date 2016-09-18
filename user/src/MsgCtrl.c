/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: MsgCtrl.c
  Author: lq       Version : V1.00        Date: 2014.05.29
  Description:     消息处理
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "def.h"
#include "MsgCtrl.h"
#include "LinkManagementEntity.h"
#include "AisDef.h"
#include "AISDigitalInterface.h"

/*******************************************************************************
* 名称  : MSG_MsgProcess
* 描述  : 处理接收的消息
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void MSG_MsgProcess(void)
{
    BinMsgRxCellStruct binMsgRxCell;        
    U8 channel;

    /*lq 接收机1 */
    if (LME_Inst1BinMsgRxFifoDeque(&binMsgRxCell) == TRUE)
    {
        if (binMsgRxCell.RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;        
        }
        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, binMsgRxCell.Data,
            binMsgRxCell.DataLen, channel);
    }

    /*lq 接收机2 */    
    if (LME_Inst2BinMsgRxFifoDeque(&binMsgRxCell) == TRUE)
    {
        if (binMsgRxCell.RxChannel == ENUM_AIS_CHANNEL_A)
        {
            channel = AIS_CHANNEL_CHAR_A;
        }
        else
        {
            channel = AIS_CHANNEL_CHAR_B;        
        }
        AISDI_BuildVDM(AIS_DI_TALKER_ID_DEFAULT, binMsgRxCell.Data,
            binMsgRxCell.DataLen, channel);
    }
}
