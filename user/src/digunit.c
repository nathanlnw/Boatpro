/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: initial.c
  Author: vozhj      Version : V1.00        Date: 2012.03.21
  Description:    初始化参数，将全部模块的参数在此处进行初始化
  Version:         V1.00
  Function List:   // 主要函数及其功能
  History:         // 历史修改记录
      <author>  <time>   	<version >   <desc>

*******************************************************************************/
#include "def.h"
#include "debug.h"
#include "initial.h"
#include "LinkPtlPort.h"
#include "LinkPtlReport.h"
#include "gmskmod.h"
#include "digunit.h"
#include "ledctrl.h"
#if !WIN32
#include "ioctrl.h"
#endif
#include "gmskmod.h"
#include "tim.h"
#include "wdg.h"
#if 1 //lqdb
#include "led.h"
#endif
#include "LinkManagementEntity.h"
/*lqdb
U8 ucBefore = 0;//lqdb
U8 ucBefore2 = 0;
char arrBefore[50];//lqdb
U8 ucAfter = 0;//lqdb
U8 ucAfter2 = 0;//lqdb

char arrAfter[50];//lqdb
*/

DigUnitStru	DigUnitPara;
/*******************************************************************************
* 名称  : DigDealMain
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void InitaDigDeal(void)
{
	DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
	DigUnitPara.TranEnd = FALSE;
}

#if 0
/*******************************************************************************
* 名称  : DigDealMain
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DigDealUnit(void)
{

	U8 ucChk;

	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
	{
		//查询接口缓冲中是否有数据
		ucChk = ChkPtlFifoSta();

		if (ucChk == TRUE)
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_STEP1;
		}

	}
	else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP1)
	{

		//1846控制，GMSK调制
		ucChk = GMSKTran();                                                     //lq 以1条消息为单位进行调制

		if (ucChk == TRUE)                                                      //lq 调制成功
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_STEP2;                             
#if !WIN32
			LEDInit(LED_TYPE_RF_XMT);                                           //lq 消息发射LED指示
#endif
		}
		else
		{
			DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}

      
	}
	else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP2)
	{
		//等待调制数据发送完成
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
		{
			//DigUnitPara.TranEnd = FALSE;

			//关闭1846
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq);          //lq 1条消息(调制后的)发送完成后，便关闭RDA1846
			DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}

	}
	else
	{
		DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
	}
                               
  
}
#endif

#if 0
/*******************************************************************************
* 名称  : DigDealUnit_ITDMA
* 描述  : ITDMA方式下消息队列中消息的处理
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DigDealUnit_ITDMA(void)
{
    U8 ucCheck;

	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
    {
        while (!IsFullOfFSKDataXmt() && ChkPtlFifoSta())							//lq 调制队列不满且消息队列非空，可以继续调制
    	{                
            ucCheck = GMSKTran_ITDMA();                                                       //lq 消息队列中还有未调制的消息
    
            if (ucCheck == TRUE)
            {
                if (!GetFetchFirstFskDataEn())
                {
                    SetFetchFirstFskDataEn();
                }        
            }
        } 
        
        while (ChkPtlFifoSta())                                                     //lq 消息队列中还有未调制的消息,丢掉不发射
        {
            //lq... 将该无效消息排出队列       
    		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq 标识协议栈中的该消息已发送
    
            LinkPtlFifoPara.FifoOut++;
    		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;        
        }                                                                          //lq 等消息队列中的所有消息均调制完后，才发送调制队列中调制后的消息
    
        if (GetFetchFirstFskDataEn())
        {
            ResetFetchFirstFskDataEn();
            if (FIFOOutOfFSKDataXmt(&pFSKDataXmtStru2))                                  //lq+ 获取调制队列中第一条要发送的调制消息
            {
                SetFSKDataXmtEn();
                
                /*lq 打开1846*/
#if 0
                RDA1846Ctrl(DRIVER_1846_XMT, pFSKDataXmtStru2->uiXmtFreq + eqEquipPara.slXmtFreqComp, RDA1846_CS_2);//lq 在向1846发送调制数据前，开启RDA1846
#endif                
                DigUnitPara.ModSta = DIGUNIT_MOD_STEP1;        
            }
            else
            {
        //lq-        ResetFSKDataXmtEn();
            }        
        }        
           
    }
    else if (DigUnitPara.ModSta == DIGUNIT_MOD_STEP1)
    {
		//等待调制数据发送完成
		if (GetFSKDataXmtEn() == FALSE)
		{
#if 0
			//关闭1846
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq, RDA1846_CS_2);          //lq 1条消息(调制后的)发送完成后，便关闭RDA1846
#endif	
    		DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;
		}    
    }
    else
    {
        DigUnitPara.ModSta = DIGUNIT_MOD_IDLE;                                  
    }
}
#endif

/*******************************************************************************
* 名称  : DigDealUnit
* 描述  : 从消息队列中提取消息送入BinMsg的发送队列
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DigDealUnit(void)
{
    BinMsgTxCellStruct *pBinMsgTxCell;        
    LinkPtlStruc linkPtlCell;
    S16 i;

    /*lq 从消息队列提取消息 */
    if (LinkPtlPortDeque(&linkPtlCell) == TRUE)
    {
        pBinMsgTxCell = (BinMsgTxCellStruct *)malloc(sizeof(BinMsgTxCellStruct));
        memset((BinMsgTxCellStruct *)pBinMsgTxCell, 0, sizeof(BinMsgTxCellStruct));
        
        pBinMsgTxCell->TxChannel = linkPtlCell.ChannelNum;
        pBinMsgTxCell->TxSlot = linkPtlCell.TranSlotNum;
        pBinMsgTxCell->DataLen = linkPtlCell.DataLength;
        for (i = 0; i < pBinMsgTxCell->DataLen; i++)
        {
            pBinMsgTxCell->Data[i] = linkPtlCell.DataBuf[i];
        }

        LME_InstBinMsgTxFifoEnque(pBinMsgTxCell);

        free((BinMsgTxCellStruct *)pBinMsgTxCell);
    }

}
/*******************************************************************************
* 名称  : GetDigUnitSleepSta
* 描述  : 获取数据处理模块状态
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE表示可以进入省电，FALSE禁止进入省电
*******************************************************************************/
U8 GetDigUnitSleepSta(void)
{
	if (DigUnitPara.ModSta == DIGUNIT_MOD_IDLE)
	{
		return (TRUE);
	}		
	else
	{			   
		return (FALSE);
	}
}
/*******************************************************************************
* 名称  : DigDealMain
* 描述  : 初始化必要的参数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void DigDealMain(void)
{
	Ptl_Main();                                                                 //lq 从解析后的GPS消息中提取信息构建x号消息，并将构建的消息放入协议栈

	DigDealUnit();
    
	ReportToPtl(); //此处加入反馈报告模块

}

/*******************************************************************************
* 名称  : GetDigUnitSta
* 描述  : 返回DigUnit的工作状态，供休眠模块使用
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE:发射机工作，FALSE：发射机不工作
*******************************************************************************/
U8 GetDigUnitSta(void)
{
	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	{
		return (FALSE);
	}
	else
	{
		return (TRUE);
	}
}

#if 0
/*******************************************************************************
* 名称  : GetDigUnitSta
* 描述  : 返回DigUnit的工作状态，供休眠模块使用
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GMSKTran(void)
{
	U8 ucChk;

	if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState == TRUE)	//lq? 为何对此判断
	{
		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq 标识协议栈中的该消息已发送

		//此处加入1846控制函数，根据信道置发射频率
		if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum == 'A')
		{
			//置161.975频率
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
		}
		else
		{
			//置162.025频率
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHB;
		}
		
		//lq RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq);
		RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq + eqEquipPara.slXmtFreqComp);//lq 在数据调制前，开启RDA1846

		//GMSK调制
		ucChk = GMSKModMain(&LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[0], 
						LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum, 
						LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength, 
						eqEquipPara.uiModAmp, 
						eqEquipPara.uiFreqOffset);
		
		LinkPtlFifoPara.FifoOut++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;

		if (ucChk == TRUE)
		{
			return (TRUE);
		}
		else
		{
			RDA1846Ctrl(DRIVER_1846_POWERDOWN, eqEquipPara.uiXmtFreq);
			return (FALSE);
		}
		
	}

	return (FALSE);
}
#endif

/*******************************************************************************
* 名称  : GMSKTran_ITDMA
* 描述  : ITDMA方式下消息队列中消息的调制,消息队列中的消息均调制完成后再发射
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GMSKTran_ITDMA(void)
{
	U8 ucChk;

	if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState == TRUE)	//lq? 为何对此判断
	{
//lq		LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq 标识协议栈中的该消息已发送

		//此处加入1846控制函数，根据信道置发射频率
		if (LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].ChannelNum == 'A')
		{
			//置161.975频率
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHA;
		}
		else
		{
			//置162.025频率
			eqEquipPara.uiXmtFreq = EQ_XMTFREQ_CHB;
		}
		
		//lq RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq);
//lq-			RDA1846Ctrl(DRIVER_1846_XMT, eqEquipPara.uiXmtFreq + eqEquipPara.slXmtFreqComp);//lq 在数据调制前，开启RDA1846

		//GMSK调制
        #if 1
/*lqdb
        sprintf(arrBefore,"arrBefore2=%d",++ucBefore2);
		UartResponseMsg(arrBefore);
*/        
 
    	ucChk = GMSKModMain_ITDMA(&LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataBuf[0], 
        							LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].TranSlotNum, 
        							LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].DataLength, 
        							eqEquipPara.uiModAmp, 
        							eqEquipPara.uiFreqOffset,
                                    eqEquipPara.uiXmtFreq);
             
/*lqdb        
        sprintf(arrBefore,"arrAfter2=%d",++ucAfter2);
		UartResponseMsg(arrBefore);
*/
		#endif
//lqdb        FIFOInOfFSKDataXmt(FSKDataXmtStru);										//lq 将当前调制数据加入调制队列
                                                                                //lq 此函数调用有个大bug，会影响Uart1RxFIFO的成员变量的值 why?
        if (ucChk == TRUE)
        {
            FIFOInOfFSKDataXmt();
		    
            LinkPtlFifoPara.LinkPtlPara[LinkPtlFifoPara.FifoOut].MessState = FALSE; //lq 标识协议栈中的该消息已发送
            LinkPtlFifoPara.FifoOut++;                                              
    		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;
            
    		UartResponseMsg("$MessState_TRUE!");//lqdb 
            
            return TRUE;
        }
        else
        {
        }
	}
    else
    {
        //lq... 将该无效消息排出队列       
		LinkPtlFifoPara.FifoOut++;
		LinkPtlFifoPara.FifoOut &= LINKPTL_FIFOLEN;
 
        UartResponseMsg("$MessState_FALSE!");//lqdb
    }

	return FALSE;
}

#if 0
/*******************************************************************************
* 名称  : RDA1846Ctrl
* 描述  : RDA1846控制
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void RDA1846Ctrl(U8 Sta, U32 ulFreq, U8 cs)
{
	if (Sta == DRIVER_1846_XMT)
	{
		//设置1846为发送状态
#if !WIN32
        if (cs == RDA1846_CS_1)
        {
    		//此处加入唤醒1846
    		IO_Ctrl(IO_RDA1846_1_TYPE, IO_VAL_HIGH);                                  //lq 重启RDA1846
    	
    		//此处加入唤醒1846
    		IO_Write(IO_RDA1846_1_TYPE, ulFreq);                                      //lq 配置RDA1846的发射频率      
        }
        else
        {
    		//此处加入唤醒1846
    		IO_Ctrl(IO_RDA1846_2_TYPE, IO_VAL_HIGH);                                  //lq 重启RDA1846
    	
    		//此处加入唤醒1846
    		IO_Write(IO_RDA1846_2_TYPE, ulFreq);                                      //lq 配置RDA1846的发射频率      
        
        }
#endif
	}
	else
	{
		//设置1846为省电状态
#if !WIN32
        if (cs == RDA1846_CS_1)
        {
            //此处加入设置1846进入省电状态
    		IO_Ctrl(IO_RDA1846_1_TYPE, IO_VAL_LOW);        
        }
        else
        {
    		IO_Ctrl(IO_RDA1846_2_TYPE, IO_VAL_LOW);                
        }		
#endif
	}
}
#endif
