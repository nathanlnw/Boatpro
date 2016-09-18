/****************************************************************************
*  Copyright (C), 2008, 712.Co.,Ltd.                                        *
*  FileName: GMSKMod.C                                            			*
*  Description :															*
*  Function List:   					                                    *
*	1-- S16  GMSKModMain(U16 *pData,U16 Length)						*	
*	1-- void BitFilter(U16 Sample)										*
*****************************************************************************
*  History:               				                                    *
*    <author>  <time>    <version >   <desc>                                *
*      Lee    	 08/07/30     1.0       build this moudle                   *
****************************************************************************/
#include "def.h"
#include "FIRstruc.h"
#include "gmskmod.h" 
#include "common.h"
#include "Tim.h"
#include "math_rand.h"
#if WIN32
#include "stdio.h"
#endif

#include "testing.h"


/********************滤波器参数设置********************/
/* coef */
const S16 Coef_IF_GaussianFilter[G_MOD_FILTER_SIZE] = 
{       
0x0000,0x0000,0x0000,0x0001,0x000B,0x0053,0x01AB,0x05FB,
0x0EDF,0x19B2,0x1ED5,0x19B2,0x0EDF,0x05FB,0x01AB,0x0053,
0x000B,0x0001,0x0000,0x0000,0x0000,
};

FSKDataXmtStruct FSKDataXmtStru;                                                //lq 用于消息队列向调制队列转移消息的中间过渡    
//FSKDataXmtStruct FSKDataXmtStru2;                                               //lq 用于从调制队列中取调制消息发送的中间过渡
FSKDataXmtStruct *pFSKDataXmtStru2;
FSKDataXmtFIFOStruct gstruFSKDataXmtFIFO;

U8  gucDataXmt_Fifo[FSKDATAXMT_FIFO_DEPTH + 1][FSKDATAXMT_FIFODATA_LEN];	//与组帧模块FIFO接口，第一个Byte存放待发送数据信道号，第二个Byte存放待发送数据长度，后为数据
U16 gucDataXmt_FifoIn;		//FIFO输入指针
U16 gucDataXmt_FifoOut;

/* declare */
FIRStruct FirBuf;
S16 DlineBuf[G_MOD_FILTER_SIZE * 2];

const U32 FrameSequ = 0x33333301;

/*lqdb
U8 ucTemp = 0;
U8 ucTemp1 = 0;
char arrTemp[30];
*/
/************************************************************
*  Function:       ResetFSKDataTran							*
*  Description:    初始化FSK调制Filter参数缓冲				*
*  Input:          pFSKDataXmtPara	数据发送结构体			*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void ResetFSKDataTran(FSKDataXmtStruct *pFSKDataXmtPara)
{
	U16 i;

	FirInit(&FirBuf, (S16 *)Coef_IF_GaussianFilter, DlineBuf, G_MOD_FILTER_SIZE);

	pFSKDataXmtPara->siRxTx_Slot = 0;
	pFSKDataXmtPara->siTxRx_Slot = 0;
	pFSKDataXmtPara->siDataStart_Slot = 0;
	pFSKDataXmtPara->siPowerUp_Slot = 0;
	pFSKDataXmtPara->siPowerDown_Slot = 0;

	for (i = 0; i < FSKDATAXMT_BUF_LEN; i++)
	{
		pFSKDataXmtPara->siDataXmtBuf[i] = 0;
	}
	pFSKDataXmtPara->uiDataXmtIndex = 0;
	pFSKDataXmtPara->uiDataLen = 0;
	pFSKDataXmtPara->uiModIndex	= 0;
}

/************************************************************
*  Function:       InitialFSKTranFIFO						*
*  Description:    初始化数据发送FIFO						*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitialFSKTranFIFO(void)
{
	U16 i, j;

	for (i = 0; i < FSKDATAXMT_FIFO_DEPTH + 1; i++)
	{
		for (j = 0; j < FSKDATAXMT_FIFODATA_LEN; j++)
		{
			gucDataXmt_Fifo[i][j] = 0;
		}
	}

	gucDataXmt_FifoIn = 0;
	gucDataXmt_FifoOut = 0;
}

/************************************************************
*  Function:       InitFSKDataTranPara								*
*  Description:    初始化FSK调制参数缓冲					*
*  Input:          无			
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitFSKDataTranPara(void)
{
	/*初始化升余弦滤波器*/
	ResetFSKDataTran(&FSKDataXmtStru);

	FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_IDLE;
	FSKDataXmtStru.uiFirt_En = FALSE;
	
	InitialFSKTranFIFO();
}

/*****************************************************************************
*    Function:	BitFilter													 *
*    Description : This is a GMSK mod  function.			    			 *
*    Input parameter:                                                        *
*       Sample	: 待调制BIT位												 *
*    Output parameter:  NONE                                                 *
*    Return value:    NONE                                                   *
******************************************************************************/
void BitFilter(FSKDataXmtStruct *pFSKDataXmtPara, U16 Sample, U16 uiModAmp, U16 uiOffset)
{
#if 1
	U16 k;
	S16 siTemp, siFirTemp;
	S16 siTempOut;

	if(Sample > 0)//这个是头？                                                  //lq '1'调制
	{
		siTemp = 400;                                                           //lq?
	}
	else                                                                        //lq '0'调制
	{
		siTemp = -400;
	}
	
	//修改代码风格
	for(k = 0; k < 5; k++) 
	{
		siFirTemp = FirFilter(&FirBuf, siTemp);//这个地方并不是补零，而是复制   //lq FIR滤波器

		/* uiModAmp可调发送频偏 */
		siTempOut = (S16)(((S32)siFirTemp * uiModAmp + 16) >> 5);
		siTempOut *= 5;
		pFSKDataXmtPara->siDataXmtBuf[pFSKDataXmtPara->uiModIndex++] = siTempOut + uiOffset; //lq? 存放调制后的数据？
	}                                                                                        //lq 1位（待调制）变为16*5位（调制后）
#endif
}

/*****************************************************************************
*    Function:	BitCurrentFilter													 *
*    Description : This is a GMSK mod  function.			    			 
					此滤波器仅产生直流分量
*    Input parameter:                                                        *
*       Sample	: 待调制BIT位												 *
*    Output parameter:  NONE                                                 *
*    Return value:    NONE                                                   *
******************************************************************************/
void BitCurrentFilter(FSKDataXmtStruct *pFSKDataXmtPara, U16 Sample, U16 uiModAmp, U16 uiOffset)
{
	U16 k;

	for(k = 0; k < 5; k++)
	{
		pFSKDataXmtPara->siDataXmtBuf[pFSKDataXmtPara->uiModIndex++] = uiOffset;
	}
}

/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:调制数据长度										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       调制数据后波形长度 used by GMSK/NRZI                                 *
******************************************************************************/
#define FSKDATAXMT_POWERUPSLOT			(10)//(10)
#define FSKDATAXMT_DATASTARTSLOT		(40)//(20)
#define FSKDATAXMT_POWERDOWNSLOTOFFSET	(40)//(5)

U16 uiTempBuf[100];

struct math_rand send_seed;
void set_send_seed(U32 seed)
{
	SetRandSeed(&send_seed, seed);
}
U16 select_slot_to_send(void)
{
#if 0	
	S16 slot;
	
	slot = GetSlotNum();
	
	return (slot + 2) % 2250;
#else
	U16 slot;
	slot = GetSlotNum() + RandData(&send_seed, 2, 9);
	if (slot >= 2250)
	{
		slot = slot - 2250;
	}
	return slot;
#endif
}

#if 0
U8 GMSKModMain(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//信号缓存		时隙		数据长度		调制幅度	频率偏差
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)                         //lq 前一条消息（调制后的）发送完成后才进行下一条消息的调制
	{
		
		//iniat Gussion filter 
		ResetFSKDataTran(&FSKDataXmtStru);                                      //lq 先初始化存放调制后数据的结构体

		FSKDataXmtStru.siTran_Slot = uiSlotNum;                                 //lq? 发送时隙号使用的是默认值0

		if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)                      //lq? FSKDATA_NOWSLOT?
		{
			if (FSKDataXmtStru.siTran_Slot >= 2250)
			{
				return FALSE;	
			}
		}

		Length = uiLen;

		//长度保护，防止长度越界引起故障
		//没有作发送失败处理
		if(Length >= 65)
		{
			Length = 64;
		}
		else if(Length <= 1)
		{
			Length = 2;
		}

		//CRC
		CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);

		for(i = 0; i < Length; i++)
		{
			uiTempBuf[i] = pInBuf[i];//数据存入缓存
		}

		uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//然后CRC值放入最后两位    //lq CRC的高位
		uiTempBuf[Length+1] = (CRCVal & 0x00ff);                                //lq CRC的低位

		//帧头调制
		for(i = 0; i < 32; i++)
		{
			uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//这个帧头是固定的//lq 对数据（字）的每一位（‘0’或‘1’）进行调制， 
			BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32位的帧头  //lq 调制后的数据放入FSKDataXmtStru的siDataXmtBuf中
		}

		uiLast = 0x0001;
		uiHighIndex = 0;

		//数据段调制
		for(i = 0; i < Length + 2; i++)//整个帧的调制                           //lq 数据段中含有2个单元的CRC码
		{// for 1
			uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节

			//读取调制数据bit值，由高位到低位
			for(j = 0; j < 8; j++)                                              //lq 字节内，由低位到高位
			{//for2
				uiTemp = (uiTemp1 >> j) & 0x0001;//获取bit值
				if(uiTemp > 0)//这个只有两个值 0 1
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					if(++uiHighIndex >= 5)//只要是1就会加                       //lq 连续5个‘1’后加1个‘0’
					{	//4//连续4个1后，第五个1就翻转
						uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个就是翻转的体现0->1 1->0
						uiHighIndex = 0;	//^异或运算符
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					}
				}
				else
				{	//这里也就是遇零翻转                                        //lq NRZI(非归零反转编码)，遇‘0’翻转，遇‘1’保持
					uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个是符号翻转的体现0->1 1->0   //lq NRZI编码
					uiHighIndex = 0;//遇到0则归零 
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);                 //lq 对NRZI编码的每一位进行(GMSK?)调制
				}
			}//end for 2
		}//end for 1

		//帧尾
		uiTemp1 = 0x007E;//这个就是帧尾了?
		for(j = 0; j < 8; j++)
		{
			uiTemp = (uiTemp1 >> j) & 0x0001;
			if(uiTemp > 0)
			{
				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
			}
			else
			{
				uiLast = ((uiLast & 0x0001) ^ 0x0001);
				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
			}			
		}

		BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
		BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);


		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;                   //lq 调制后的数据长度（单位-字，16b）

		FSKDataXmtStru.siTran_Slot = select_slot_to_send();                     //lq? 选择发送时隙?
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//使能输出               //lq 进入（调制后的）消息的发送状态的控制，将调制后的数据通过DAC送入26M晶振，然后再送入RDA1846的晶振引脚


		return (TRUE);
	}
	
	return(FALSE);
}
#endif

U8 GMSKModMain_ITDMA(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, 	U32 uiXmtFreq)
{	   			//信号缓存		时隙		数据长度		调制幅度	频率偏差         发射频率
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;
	
    /*lqdb
    ucTemp++;
    */	

	if (!GetFSKDataXmtEn())                         //lq 前一条消息（调制后的）发送完成后才进行下一条消息的调制
	{
    	//iniat Gussion filter 
    	ResetFSKDataTran(&FSKDataXmtStru);                                      //lq 先初始化存放调制后数据的结构体
    
    	FSKDataXmtStru.siTran_Slot = uiSlotNum;                                 //lq? 发送时隙号使用的是默认值0
        FSKDataXmtStru.uiXmtFreq = uiXmtFreq;                                   //lq 保存发射频率
    /*
    	if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)                      //lq? FSKDATA_NOWSLOT?
    	{
    		if (FSKDataXmtStru.siTran_Slot >= 2250)
    		{
    			return FALSE;	
    		}
    	}
    */
    	Length = uiLen;
    
    	//长度保护，防止长度越界引起故障
    	//没有作发送失败处理
    	if(Length >= 65)                                                            //lq 允许的消息最大长度为LINKPTL_DATABUF_LEN = 65
    	{
    		Length = 64;
    	}
    	else if(Length <= 1)
    	{
    		Length = 2;
    	}
    
    	//CRC
    	CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
    
    	for(i = 0; i < Length; i++)
    	{
    		uiTempBuf[i] = pInBuf[i];//数据存入缓存
    	}
    
    	uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//然后CRC值放入最后两位    //lq CRC的高位
    	uiTempBuf[Length+1] = (CRCVal & 0x00ff);                                //lq CRC的低位
    
    	//帧头调制
    	for(i = 0; i < 32; i++)
    	{
    		uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//这个帧头是固定的//lq 对数据（字）的每一位（‘0’或‘1’）进行调制， 
    		BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32位的帧头  //lq 调制后的数据放入FSKDataXmtStru的siDataXmtBuf中
    	}
    
    	uiLast = 0x0001;
    	uiHighIndex = 0;
    
    	//数据段调制
    	for(i = 0; i < Length + 2; i++)//整个帧的调制                           //lq 数据段中含有2个单元的CRC码
    	{// for 1
    		uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
    
    		//读取调制数据bit值，由高位到低位
    		for(j = 0; j < 8; j++)                                              //lq 字节内，由低位到高位
    		{//for2
    			uiTemp = (uiTemp1 >> j) & 0x0001;//获取bit值
    			if(uiTemp > 0)//这个只有两个值 0 1
    			{
    				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    				if(++uiHighIndex >= 5)//只要是1就会加                       //lq 连续5个‘1’后加1个‘0’
    				{	//4//连续4个1后，第五个1就翻转
    					uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个就是翻转的体现0->1 1->0
    					uiHighIndex = 0;	//^异或运算符
    					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    				}
    			}
    			else
    			{	//这里也就是遇零翻转                                        //lq NRZI(非归零反转编码)，遇‘0’翻转，遇‘1’保持
    				uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个是符号翻转的体现0->1 1->0   //lq NRZI编码
    				uiHighIndex = 0;//遇到0则归零 
    				BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);                 //lq 对NRZI编码的每一位进行(GMSK?)调制
    			}
    		}//end for 2
    	/*lqdb
        if (ucTemp > 1)
        {
            sprintf(arrTemp,"arrLen=%d",++ucTemp1); 
            UartResponseMsg(arrTemp);
        }
        */
        }//end for 1
    
    
    	//帧尾
    	uiTemp1 = 0x007E;//这个就是帧尾了?
    	for(j = 0; j < 8; j++)
    	{
    		uiTemp = (uiTemp1 >> j) & 0x0001;
    		if(uiTemp > 0)
    		{
    			BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    		}
    		else
    		{
    			uiLast = ((uiLast & 0x0001) ^ 0x0001);
    			BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
    		}			
    	}
    
    	BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
    	BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
    
    
    	FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
    	FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
    	FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
    
    	FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;                   //lq 调制后的数据长度（单位-字，16b）
    
    //lq-	FSKDataXmtStru.siTran_Slot = select_slot_to_send();                     //lq? 选择发送时隙?
    	FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//使能输出               //lq 进入（调制后的）消息的发送状态的控制，将调制后的数据通过DAC送入26M晶振，然后再送入RDA1846的晶振引脚
    
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:调制数据长度										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       调制数据后波形长度 used by GMSK/NRZI                                 *
******************************************************************************/
U8 GMSKModTest(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset, U8 ucTestMod)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast;
	
//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
    {		
		if (TestCtrlPara.FirstGenFlag == TRUE)//应重新产生信号					//lq 测试项改变，才需重新产生待发射的调制信号，	
		{																		//lq 仅当该测试项的首次发射时调制信号，该测试项的后续发射无需再进行调制
			TestCtrlPara.FirstGenFlag = FALSE;	
			
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;
	
			Length = uiLen;
	
			//长度保护，防止长度越界引起故障
			//没有作发送失败处理
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];
			}
	
			//数据段调制
			for(i=0; i<Length; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
	
				//读取调制数据bit值，由高位到低位
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> (7 - j))&0x0001;
					if(uiTemp > 0)
					{
						uiLast = 0x0001;
	
						if (ucTestMod == 0)
						{
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						else
						{
							BitCurrentFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						
					}
					else
					{
						uiLast = 0x0000;
						if (ucTestMod == 0)
						{
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
						else
						{
							BitCurrentFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
					}
				}//end for 2
			}//end for 1
	
			if (ucTestMod == 0)
			{
				BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
				BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			}
			else
			{
				BitCurrentFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
				BitCurrentFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			}

		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;

		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

		return (TRUE);
	}
	
	return(FALSE);
}

/*******************************************************************************
* 名称  : GMSKModTest2
* 描述  : 包含帧头帧尾的测试态信号调制函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GMSKModTest2(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//信号缓存		时隙		数据长度		调制幅度	频率偏差
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
	{
			
		if (TestCtrlPara.FirstGenFlag == TRUE)//应重新产生信号
		{
			TestCtrlPara.FirstGenFlag = FALSE;	
		
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = uiSlotNum;
	
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					return FALSE;	
				}
			}
	
			Length = uiLen;
	
			//长度保护，防止长度越界引起故障
			//没有作发送失败处理
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			//CRC
			CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];//数据存入缓存
			}
	
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//然后CRC值放入最后两位
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
	
			//帧头调制
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//这个帧头是固定的
				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32位的帧头
			}
	
			uiLast = 0x0001;
			uiHighIndex = 0;
	
			//数据段调制
			for(i = 0; i < Length + 2; i++)//整个帧的调制
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
	
				//读取调制数据bit值，由高位到低位
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;//获取bit值
					if(uiTemp > 0)//这个只有两个值 0 1
					{
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						if(++uiHighIndex >= 5)//只要是1就会加
						{	//4//连续4个1后，第五个1就翻转
							uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个就是翻转的体现0->1 1->0
							uiHighIndex = 0;	//^异或运算符
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						}
					}
					else
					{	//这里也就是遇零翻转
						uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个是符号翻转的体现0->1 1->0
						uiHighIndex = 0;//遇到0则归零 
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
					}
				}//end for 2
			}//end for 1
	
			//帧尾
			uiTemp1 = 0x007E;//这个就是帧尾了?
			for(j = 0; j < 8; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}			
			}
	
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);

		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据

		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
		
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//使能输出


		return (TRUE);
	}
	
	return(FALSE);
}

/*******************************************************************************
* 名称  : GMSKModTest2
* 描述  : 包含帧头帧尾，但不采用NRZI编码帧的数据和校验码的测试态信号调制函数
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U8 GMSKModTest3(U8 *pInBuf, S16 uiSlotNum, U16 uiLen, U16 uiModAmp, U16 uiOffset)
{	   			//信号缓存		时隙		数据长度		调制幅度	频率偏差
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

//lq-	if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)
	if (!GetFSKDataXmtEn())
	{
		if (TestCtrlPara.FirstGenFlag == TRUE)//应重新产生信号
		{
			TestCtrlPara.FirstGenFlag = FALSE;	
		
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);
	
			FSKDataXmtStru.siTran_Slot = uiSlotNum;
	
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					return FALSE;	
				}
			}
	
			Length = uiLen;
	
			//长度保护，防止长度越界引起故障
			//没有作发送失败处理
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
	
			//CRC
			CRCVal = AIS_Cal_CRC16(&pInBuf[0], Length);
	
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = pInBuf[i];//数据存入缓存
			}
	
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);//然后CRC值放入最后两位
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
	
			//帧头调制
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);//这个帧头是固定的

				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp, uiOffset);//32位的帧头
			}
	
			uiLast = 0x0001;
			uiHighIndex = 0;
	
			//数据段调制
			for(i = 0; i < Length + 2; i++)//整个帧的调制
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
	
				//读取调制数据bit值，由高位到低位
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;//获取bit值
					if(uiTemp > 0)//这个只有两个值 0 1
					{
//lq						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						BitFilter(&FSKDataXmtStru, 1, uiModAmp, uiOffset);
                       
                        uiLast = 1;
					
                    	if(++uiHighIndex >= 5)//只要是1就会加
						{	//4//连续4个1后，第五个1就翻转
//lq							uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个就是翻转的体现0->1 1->0
							uiHighIndex = 0;	//^异或运算符
//lq							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
							BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
                            
                            uiLast = 0;						
                        }
					}
					else
					{	//这里也就是遇零翻转
//lq						uiLast = ((uiLast & 0x0001) ^ 0x0001);//这个是符号翻转的体现0->1 1->0
						uiHighIndex = 0;//遇到0则归零 
//lq						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
						BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
					
                        uiLast = 0;
                    }
				}//end for 2
			}//end for 1
	
			//帧尾
			uiTemp1 = 0x007E;//这个就是帧尾了?
			for(j = 0; j < 8; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp, uiOffset);
				}			
			}
	
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
			BitFilter(&FSKDataXmtStru, 0, uiModAmp, uiOffset);
		}

		FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
		FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
		FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据

		FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;

		FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
		
		FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;//使能输出


		return (TRUE);
	}
	
	return(FALSE);
}

#if 0
/*****************************************************************************
*    Function:	GMSKModMain                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:调制数据长度										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       调制数据后波形长度 used by GMSK/NRZI                                 *
******************************************************************************/
#define FSKDATAXMT_POWERUPSLOT		(30)
#define FSKDATAXMT_DATASTARTSLOT	(40)

U16 uiTempBuf[100];

U16 GMSKModMain(U16 uiModAmp)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast,uiHighIndex;
	U16 CRCVal;

	if (gucDataXmt_FifoIn != gucDataXmt_FifoOut)
	{//if 0
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)			//上一帧数据调制完成，方进行数据调制
		{//if 1
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);                                  

			FSKDataXmtStru.siTran_Slot = ((gucDataXmt_Fifo[gucDataXmt_FifoOut][0] & 0xFF) << 8) 
										| (gucDataXmt_Fifo[gucDataXmt_FifoOut][1] & 0xFF);
			
			if (FSKDataXmtStru.siTran_Slot != FSKDATA_NOWSLOT)
			{
				if (FSKDataXmtStru.siTran_Slot >= 2250)
				{
					FSKDataXmtStru.siTran_Slot -= 2250;	
				}
			}
					
			Length = gucDataXmt_Fifo[gucDataXmt_FifoOut][2];

			//长度保护，防止长度越界引起故障
			//没有作发送失败处理
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
		
			//CRC
			CRCVal = AIS_Cal_CRC16(&gucDataXmt_Fifo[gucDataXmt_FifoOut][3], Length);
		
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = gucDataXmt_Fifo[gucDataXmt_FifoOut][3 + i];
			}
			uiTempBuf[Length] = ((CRCVal >> 8) & 0x00ff);
			uiTempBuf[Length+1] = (CRCVal & 0x00ff);
		
			//帧头调制
			for(i = 0; i < 32; i++)
			{
				uiTemp = (U16)((FrameSequ >> (31-i)) & 0x00000001);
				BitFilter(&FSKDataXmtStru, uiTemp, uiModAmp);
			}
		
			uiLast = 0x0001;
			uiHighIndex = 0;
		
			//数据段调制
			for(i = 0; i < Length + 2; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
		
				//读取调制数据bit值，由高位到低位
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j) & 0x0001;
					if(uiTemp > 0)
					{
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
						if(++uiHighIndex >= 5)
						{
							uiLast = ((uiLast & 0x0001) ^ 0x0001);
							uiHighIndex = 0;
							BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
						}
					}
					else
					{
						uiLast = ((uiLast & 0x0001) ^ 0x0001);
						uiHighIndex = 0;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
				}//end for 2
			}//end for 1
			
			//帧尾
			uiTemp1 = 0x007E;
			for(j = 0; j < 11; j++)
			{
				uiTemp = (uiTemp1 >> j) & 0x0001;
				if(uiTemp > 0)
				{
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
				}
				else
				{
					uiLast = ((uiLast & 0x0001) ^ 0x0001);
					BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
				}			
			}

			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;

			FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
			FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
			FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据

			FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
			FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

			gucDataXmt_FifoOut++;
 			gucDataXmt_FifoOut &= FSKDATAXMT_FIFO_DEPTH;
		}//end if 1
	}//end if 0
	 
	return(TRUE);
}


/*****************************************************************************
*    Function:	GMSKModTest                                             	 *
*    Description : This is a GMSK mod Main function.			    		 *
*    Input parameter:                                                        *
*       pData	: point to a buffer used by GMSK mod					     *
*       Length		:调制数据长度										     *
*    Output parameter:  NONE                                                 *
*    Return value:                                                           *
*       调制数据后波形长度 used by GMSK/NRZI                                 *
******************************************************************************/
U16 GMSKModTest(U16 uiModAmp)
{
	S16 i,j;
	U16 Length;
	U16 uiTemp,uiTemp1,uiLast;

	if (gucDataXmt_FifoIn != gucDataXmt_FifoOut)
	{//if 0
		if (FSKDataXmtStru.uiTran_Sta == FSKDATA_TRAN_IDLE)			//上一帧数据调制完成，方进行数据调制
		{//if 1
			//iniat Gussion filter 
			ResetFSKDataTran(&FSKDataXmtStru);

			FSKDataXmtStru.siTran_Slot = FSKDATA_NOWSLOT;
					
			Length = gucDataXmt_Fifo[gucDataXmt_FifoOut][2];

			//长度保护，防止长度越界引起故障
			//没有作发送失败处理
			if(Length >= 65)
			{
				Length = 64;
			}
			else if(Length <= 1)
			{
				Length = 2;
			}
					
			for(i = 0; i < Length; i++)
			{
				uiTempBuf[i] = gucDataXmt_Fifo[gucDataXmt_FifoOut][3 + i];
			}
		
			//数据段调制
			for(i=0; i<Length; i++)
			{// for 1
				uiTemp1 = uiTempBuf[i];	  		//读取调制数据字节
		
				//读取调制数据bit值，由高位到低位
				for(j = 0; j < 8; j++)
				{//for2
					uiTemp = (uiTemp1 >> j)&0x0001;
					if(uiTemp > 0)
					{
						uiLast = 0x0001;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
					else
					{
						uiLast = 0x0000;
						BitFilter(&FSKDataXmtStru, uiLast, uiModAmp);
					}
				}//end for 2
			}//end for 1

			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;
			FSKDataXmtStru.siDataXmtBuf[FSKDataXmtStru.uiModIndex++] = 0;

			FSKDataXmtStru.siPowerUp_Slot = FSKDATAXMT_POWERUPSLOT;		//数据发送前提前1.5+1.25ms功率上升
			FSKDataXmtStru.siDataStart_Slot = FSKDATAXMT_DATASTARTSLOT;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据
			FSKDataXmtStru.siPowerDown_Slot = FSKDATAXMT_POWERDOWNSLOTOFFSET;		//时隙开始1.25+1.25msms,抵消数据传递时间处发送数据

			FSKDataXmtStru.uiDataLen = FSKDataXmtStru.uiModIndex;
			FSKDataXmtStru.uiTran_Sta = FSKDATA_TRAN_START;

			gucDataXmt_FifoOut++;
 			gucDataXmt_FifoOut &= FSKDATAXMT_FIFO_DEPTH;
		}//end if 1
	}//end if 0
	 
	return(TRUE);
}

/************************************************************
*  Function:       FillFSKTranFIFO							*
*  Description:    按时隙发送载波							*
*  Input:          pFSKDataXmtPara	数据发送结构体			*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
U8 FillGMSKTranFIFO(U8 *pInBuf, U16 uiSlotNum, U16 uiLen)
{
	U16 i, uiIndex;
	
	uiIndex = 0;

	if (((gucDataXmt_FifoIn + 1) & FSKDATAXMT_FIFO_DEPTH) != gucDataXmt_FifoOut)		//FIFO非满
	{
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)(uiSlotNum >> 8);
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)(uiSlotNum & 0xFF);
		gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] =	(U8)uiLen;

		for (i = 0; i < uiLen; i++)
		{
			gucDataXmt_Fifo[gucDataXmt_FifoIn][uiIndex++] = pInBuf[i];
		}

		gucDataXmt_FifoIn++;
		gucDataXmt_FifoIn &= FSKDATAXMT_FIFO_DEPTH;
		
		return TRUE;
	}

	return FALSE;
}
#endif

#if 0
/************************************************************
*  Function:       FIFOInOfFSKDataXmt						*
*  Description:    调制后的消息的发送队列-入队				*
*  Input:          inFSKDataXmtPara	数据发送结构体			*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
U8 FIFOInOfFSKDataXmt(FSKDataXmtStruct inFSKDataXmtPara)
{
   if (((gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH) != gstruFSKDataXmtFIFO.ucFIFOOut) //lq 消息发射队列不满
   { 
        gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOIn++] = inFSKDataXmtPara;
        
        gstruFSKDataXmtFIFO.ucFIFOIn %= FSKDATAXMTFIFO_DEPTH;

        return TRUE;
   }
   else
   {
        return FALSE;
   }
}
#endif

/************************************************************
*  Function:       FIFOInOfFSKDataXmt						*
*  Description:    调制后的消息的发送队列-入队				*
*  Input:          无                           			*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
U8 FIFOInOfFSKDataXmt(void)
{
   if (((gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH) != gstruFSKDataXmtFIFO.ucFIFOOut) //lq 消息发射队列不满
   { 
        gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOIn++] = FSKDataXmtStru;
        
        gstruFSKDataXmtFIFO.ucFIFOIn %= FSKDATAXMTFIFO_DEPTH;

        return TRUE;
   }
   else
   {
        return FALSE;
   }
}

/************************************************************
*  Function:       FIFOOutOfFSKDataXmt						*
*  Description:    调制后的消息的发送队列-出队				*
*  Input:          无			*	
*  Output:         无										*
*  Return:         数据发送结构体							*
*  Others:         无										*
************************************************************/
U8 FIFOOutOfFSKDataXmt(FSKDataXmtStruct **outFSKDataXmtPara)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut != gstruFSKDataXmtFIFO.ucFIFOIn)           //lq 消息发射队列非空 
   {
        *outFSKDataXmtPara = &gstruFSKDataXmtFIFO.arrFIFOData[gstruFSKDataXmtFIFO.ucFIFOOut++];

        gstruFSKDataXmtFIFO.ucFIFOOut %= FSKDATAXMTFIFO_DEPTH;

        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       IsEmptyOfFSKDataXmt						*
*  Description:    调制数据队列是否为空				        *
*  Input:          无			                            *	
*  Output:         无										*
*  Return:         是否为空     							*
*  Others:         无										*
************************************************************/
U8 IsEmptyOfFSKDataXmt(void)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut == gstruFSKDataXmtFIFO.ucFIFOIn)           //lq 消息发射队列空 
   {
        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       IsFullOfFSKDataXmt						*
*  Description:    调制数据队列是否已满				        *
*  Input:          无			                            *	
*  Output:         无										*
*  Return:         是否已满     							*
*  Others:         无										*
************************************************************/
U8 IsFullOfFSKDataXmt(void)
{
   if (gstruFSKDataXmtFIFO.ucFIFOOut == (gstruFSKDataXmtFIFO.ucFIFOIn + 1) % FSKDATAXMTFIFO_DEPTH)           //lq 消息发射队列满
   {
        return TRUE; 
   } 
   else
   {
        return FALSE;
   }   
}

/************************************************************
*  Function:       InitFSKDataXmtFIFO						*
*  Description:    初始化消息发射队列				        *
*  Input:          消息发射队列			                    *	
*  Output:         无										*
*  Return:         无     							        *
*  Others:         无										*
************************************************************/
void InitFSKDataXmtFIFO(FSKDataXmtFIFOStruct *pFSKDataXmtFIFO)
{
    U16 i;
    U16 j;
    pFSKDataXmtFIFO->ucFIFOIn = 0;
    pFSKDataXmtFIFO->ucFIFOOut = 0;
    pFSKDataXmtFIFO->ucFIFOLen = 0;

    for (i = 0; i < FSKDATAXMTFIFO_DEPTH; i++)
    {
        pFSKDataXmtFIFO->arrFIFOData[i].siRxTx_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siTxRx_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siDataStart_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siPowerUp_Slot = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].siPowerDown_Slot = 0;
    	for (j = 0; j < FSKDATAXMT_BUF_LEN; j++)
    	{
            pFSKDataXmtFIFO->arrFIFOData[i].siDataXmtBuf[j] = 0;
    	}
        pFSKDataXmtFIFO->arrFIFOData[i].uiDataXmtIndex = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].uiDataLen = 0;
        pFSKDataXmtFIFO->arrFIFOData[i].uiModIndex	= 0;
    }



}
