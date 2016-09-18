/****************************************************************************
*  Copyright (C), 2012, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.c 				                                *
*  Description:  底层与协议栈反馈接口实现									*
*  Function List:   					                                    *
*	1--  void InitaReportPara (void);							     		*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 12/01/08     1.0       build this moudle		            *
****************************************************************************/

#include "def.h"
#include "LinkPtlPort.h"
#include "Ptl_Interface.h"

LinkPtlFifoStruc LinkPtlFifoPara;
//extern MessManaStr mmMessCell;
/************************************************************
*  Function:       InitaReportPara							*
*  Description:    初始化Report参数							*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitaLinkPtlPortPara (void)
{
	U8 i, j;

	LinkPtlFifoPara.FifoIn = 0;
	LinkPtlFifoPara.FifoOut = 0;

	for (i = 0; i < (LINKPTL_FIFOLEN + 1); i++)
	{
		LinkPtlFifoPara.LinkPtlPara[i].MessState = FALSE;                       //lq 初始化所有消息的状态为已发送(不可发送)
		LinkPtlFifoPara.LinkPtlPara[i].TranSlotNum = 0;
		LinkPtlFifoPara.LinkPtlPara[i].ChannelNum = 'A';
		for (j = 0; j < LINKPTL_DATABUF_LEN; j++)                               //lq 初始化默认消息内容全为0
		{
			LinkPtlFifoPara.LinkPtlPara[i].DataBuf[j] = 0;
		}
		LinkPtlFifoPara.LinkPtlPara[i].DataLength = 0;                          //lq 初始化默认消息长度为0
	}
}

#if 0
/************************************************************
*  Function:       LinkPtlPort								*
*  Description:    协议栈发送消息出来						*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void LinkPtlPort (void)
{
	U16 i;

	if (mmMessCell.MessState == TRUE)
	{
		mmMessCell.MessState = FALSE;

		//FIFO是否有空间
		if ((LinkPtlFifoPara.FifoIn + 1) != LinkPtlFifoPara.FifoOut)
		{
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].MessState = TRUE;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].TranSlotNum = mmMessCell.TranSlotNum;
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].ChannelNum = mmMessCell.ChannelNum;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength = mmMessCell.DataLength;
			for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength; i++)
			{
				LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataBuf[i] = (U8)mmMessCell.pData[i];
			}
			LinkPtlFifoPara.FifoIn ++;
			LinkPtlFifoPara.FifoIn &= LINKPTL_FIFOLEN;
		}
	}
}
#endif

/************************************************************
*  Function:       LinkPtlPort								*
*  Description:    协议栈消息存放接口						*
*  Input:          MessManaStr *mmMessCell 协议栈消息结构体										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void LinkPtlPort (MessManaStr *mmMessCell)
{
	U16 i;

	if (mmMessCell->MessState == TRUE)
	{
		mmMessCell->MessState = FALSE;

		//FIFO是否有空间
		if ((LinkPtlFifoPara.FifoIn + 1) != LinkPtlFifoPara.FifoOut)
		{
			
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].MessState = TRUE;   //lq 标记该条消息可以发送（但还未发送）

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].TranSlotNum = mmMessCell->TranSlotNum;
			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].ChannelNum = mmMessCell->ChannelNum;

			LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength = mmMessCell->DataLength;
			for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataLength; i++)
			{
				LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoIn].DataBuf[i] = (U8)mmMessCell->pData[i];
			}

			LinkPtlFifoPara.FifoIn ++;
			LinkPtlFifoPara.FifoIn &= LINKPTL_FIFOLEN;
		}
	}
}

/************************************************************
*  Function:       LinkPtlPortDeque							
*  Description:    协议栈消息提取						
*  Input:          pLinkPtlStruc - 协议栈消息结构体指针	
*  Output:         无										
*  Return:         TRUE - 出队成功，FALSE - 出队失败		
*  Others:         无										
************************************************************/
U8 LinkPtlPortDeque(LinkPtlStruc *pLinkPtlStruc)
{
	U16 i;

	/*lq 判断是否有数据 */
	if (IsLinkPtlQueEmpty() == FALSE)
	{
		pLinkPtlStruc->MessState = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState;   //lq 标记该条消息可以发送（但还未发送）
		pLinkPtlStruc->TranSlotNum = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum;
		pLinkPtlStruc->ChannelNum = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum;
		pLinkPtlStruc->DataLength = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength;
		for (i = 0; i < LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength; i++)
		{
			pLinkPtlStruc->DataBuf[i] = LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[i];
		}

		LinkPtlFifoPara.FifoOut ++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;

        return TRUE;
	}
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsLinkPtlQueEmpty
* 描述  : 队列是否为空
* 输入  : 无
* 输出  : 无
* 返回  : True - 为空，FALSE - 非空
*******************************************************************************/
U8 IsLinkPtlQueEmpty(void)
{
    if (LinkPtlFifoPara.FifoIn == LinkPtlFifoPara.FifoOut)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : IsLinkPtlQueFull
* 描述  : 队列是否已满
* 输入  : 无
* 输出  : 无
* 返回  : True - 已满，FALSE - 未满
*******************************************************************************/
U8 IsLinkPtlQueFull(void)
{
    if ((LinkPtlFifoPara.FifoIn + 1) % (LINKPTL_FIFOLEN + 1)
         == LinkPtlFifoPara.FifoOut)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : ChkPtlFifoSta
* 描述  : 查询接口中是否存在数据
* 输入  : 无		  
* 输出  : 无
* 返回  : 接口中数据状态
*******************************************************************************/
U8 ChkPtlFifoSta(void)
{
	if (LinkPtlFifoPara.FifoOut != LinkPtlFifoPara.FifoIn)
	{
		return (TRUE);	
	}
	else
	{
		return (FALSE);
	}
}

/*******************************************************************************
* 名称  : GetPtlFifoSleepSta
* 描述  : 查询接口是否可进入睡眠
* 输入  : 无		  
* 输出  : 无
* 返回  : 接口中数据状态
*******************************************************************************/
U8 GetPtlFifoSleepSta(void)
{
	if (LinkPtlFifoPara.FifoOut == LinkPtlFifoPara.FifoIn)
	{
		return (TRUE);
	}
	else
	{
		return (FALSE);
	}
}		

/************************************************************
*  Function:       GetPtlSta								*
*  Description:    查询协议栈休眠状态						*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         协议栈状态								*
************************************************************/
U8 GetPtlSta (void)
{
	return ((U8)mmMessCell.CurrentOperationFlg);
}

/************************************************************
*  Function:       DCSimPtl									*
*  Description:    底层仿真协议栈接口						*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
U16 gucTestPtlBuf[70];
extern S16 gsiSlotCnt;	     //2250时隙计数
#define DCSIM_DATA_LEN		40
void DCSimPtl (void)
{
#if 0
	U16 i;

	mmMessCell.MessState = TRUE;
	mmMessCell.ChannelNum = 'A';
	mmMessCell.TranSlotNum = gsiSlotCnt + 2;

	for (i = 0; i < DCSIM_DATA_LEN; i++)
	{
		gucTestPtlBuf[i] = 0x55;
	}
	mmMessCell.DataLength = DCSIM_DATA_LEN;
	mmMessCell.pData = &gucTestPtlBuf[0];
#endif
}
























