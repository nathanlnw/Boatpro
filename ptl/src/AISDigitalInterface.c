/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: AISDigitalInterface.c
  Author: lq       Version : V1.00        Date: 2014.04.25
  Description:     AIS数字通信接口
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include "Include.h"

AisDiSenFifoStruct AisDiSenQue;
ABMStruct ABMPara;
BBMStruct BBMPara;

/*******************************************************************************
* 名称  : AISDI_InitAisDiSenFifoStruct
* 描述  : 结构体初始化
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static void AISDI_InitAisDiSenFifoStruct(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	pAisDiSenFifoStruct->FifoInput = 0;
	pAisDiSenFifoStruct->FifoOutput = 0;
}

/*******************************************************************************
* 名称  : AISDI_InitABMStruct
* 描述  : 结构体初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_InitABMStruct(ABMStruct *pABMStruct)
{
    pABMStruct->SentenceTotal = 0;
    pABMStruct->SentenceNumber = 0;
    pABMStruct->MessageNumber = 0;
    pABMStruct->MmsiDest = 0;
    pABMStruct->Channel = ENUM_AIS_CHANNEL_BOTH;
    pABMStruct->MsgID = 0;
    pABMStruct->EncapDataLen = 0;
    pABMStruct->FillBitNumber = 0; 	    
    pABMStruct->ValidFlg = FALSE;
}

/*******************************************************************************
* 名称  : AISDI_InitBBMStruct
* 描述  : 结构体初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_InitBBMStruct(BBMStruct *pBBMStruct)
{
    pBBMStruct->SentenceTotal = 0;
    pBBMStruct->SentenceNumber = 0;
    pBBMStruct->MessageNumber = 0;
    pBBMStruct->Channel = ENUM_AIS_CHANNEL_BOTH;
    pBBMStruct->MsgID = 0;
    pBBMStruct->EncapDataLen = 0;
    pBBMStruct->FillBitNumber = 0; 	    
    pBBMStruct->ValidFlg = FALSE;
}

/*******************************************************************************
* 名称  : AISDI_InitAisDiSenFifoInstance
* 描述  : 结构体初始化
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_InitAisDiSenFifoInstance(void)
{
	AISDI_InitAisDiSenFifoStruct(&AisDiSenQue);
}

/*******************************************************************************
* 名称  : AsciiToSixBinCode
* 描述  : ASCC码转为AIS的6位二进制码
* 输入  : ASCII码		  
* 输出  : 无
* 返回  : 6位二进制码
*******************************************************************************/
U8 AsciiToSixBinCode(U8 code)
{
	U16 sum;
	
	sum = code + 0x28;
	if (sum > 0x80)
	{
	    return (sum + 0x20) & 0x3F;
	}
	else
	{
	    return (sum + 0x28) & 0x3F;            
	}
}

/*******************************************************************************
* 名称  : SixBinCodeToAscii
* 描述  : AIS的6位二进制码转为ASCII码
* 输入  : 6位二进制码	  
* 输出  : 无
* 返回  : ASCII码
*******************************************************************************/
U8 SixBinCodeToAscii(U8 code)
{
    if (code < 0x28)
    {
        return code + 0x30;
    }
    else
    {
        return code + 0x38;
    }        
}

/*******************************************************************************
* 名称  : AISDI_IsSenQueFull
* 描述  : 语句队列是否已满
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针		  
* 输出  : 无
* 返回  : TRUE - 已满，FALSE - 未满
*******************************************************************************/
U8 AISDI_IsSenQueFull(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	if ((pAisDiSenFifoStruct->FifoInput + 1) % AIS_DI_SEN_FIFO_DEPTH
		== pAisDiSenFifoStruct->FifoOutput)
	{
        debug_printf("$[%6d:%4d] %s : Sentence queue is full!\r\n",
            GetFrameNum(), GetSlotNum(), __func__);		 
			
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : AISDI_IsSenQueEmpty
* 描述  : 语句队列是否为空
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针		  
* 输出  : 无
* 返回  : TRUE - 为空，FALSE - 非空
*******************************************************************************/
U8 AISDI_IsSenQueEmpty(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
	if (pAisDiSenFifoStruct->FifoInput == pAisDiSenFifoStruct->FifoOutput)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : AISDI_SenEnque
* 描述  : 语句队列入队
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针
		  pData - 指向语句的指针		  
* 输出  : 无
* 返回  : True - 入队成功，FALSE - 入队失败
*******************************************************************************/
U8 AISDI_SenEnque(AisDiSenFifoStruct* pAisDiSenFifoStruct, U8* pData)
{
	if (AISDI_IsSenQueFull(pAisDiSenFifoStruct) == FALSE)
	{
		strcpy(pAisDiSenFifoStruct->Fifo[pAisDiSenFifoStruct->FifoInput], pData);
		pAisDiSenFifoStruct->FifoInput = (pAisDiSenFifoStruct->FifoInput + 1) 
										  % AIS_DI_SEN_FIFO_DEPTH;		
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : AISDI_SenDeque
* 描述  : 语句队列出队
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针
		  pData - 指向存放出队语句的指针		  
* 输出  : 无
* 返回  : True - 出队成功，FALSE - 出队失败
*******************************************************************************/
U8 AISDI_SenDeque(AisDiSenFifoStruct* pAisDiSenFifoStruct, U8* pData)
{
	if (AISDI_IsSenQueEmpty(pAisDiSenFifoStruct) == FALSE)
	{
		strcpy(pData, pAisDiSenFifoStruct->Fifo[pAisDiSenFifoStruct->FifoOutput]);
		pAisDiSenFifoStruct->FifoOutput = (pAisDiSenFifoStruct->FifoOutput + 1) 
										  % AIS_DI_SEN_FIFO_DEPTH;		
	
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : AISDI_BuildVDM
* 描述  : 组建VDM语句
* 输入  : listener - 侦听者ID
          data - 消息
          len - 消息字节长度
          channel - 接收消息的信道符号  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_BuildVDM(U8 listener[], U8 data[], U16 len, U8 channel)
{
	static U8 msgSeq = 0;
	U8 	senNum;
	U8	senTotal;
	U8	fillBitNum;
	U16 sixBitCodeNum;
    U16 temp16;
	AISDISenBuffArray sentence;
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 temp8;
	U8 tempBuff[10];

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = data;
	msg_bitmap.array_size = len;
	
	if ((len * 8) % 6 == 0)
	{
		sixBitCodeNum = (len * 8) / 6;
		fillBitNum = 0;	
	}
	else
	{
		sixBitCodeNum = (len * 8) / 6 + 1;
		fillBitNum = 6 - (len * 8) % 6;
	}

	if (channel != 'A' && channel != 'B')
	{
		/*lq 信道号字段为空 */
		if (sixBitCodeNum <= 62)
		{
            /*lq 单语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");			
			strcat(sentence, ",1");		//lq 语句总数
			strcat(sentence, ",1");		//lq 语句序号
			strcat(sentence, ",");		//lq 消息序号，单语句消息的消息序号字段为空
			strcat(sentence, ",");		//lq 信道为空
			strcat(sentence, ",");		//lq 数据
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq 位填充数

			/*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq 多语句 */
            if (sixBitCodeNum % 61 == 0)
            {
                senTotal = sixBitCodeNum / 61;
                temp16 = 61;
            }
            else
            {
                senTotal = sixBitCodeNum / 61 + 1;
                temp16 = sixBitCodeNum % 61;            
            }            

            /*lq 最大长度语句 */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDM");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq 语句总数
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq 语句序号
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq 消息序号
    			strcat(sentence, ",");		    //lq 信道为空
    			strcat(sentence, ",");		    //lq 数据
    			for (i = 0; i < 61; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq 位填充数
    
    			/*lq 存入语句队列*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq 该消息的最后一条语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq 语句总数
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq 语句序号
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq 消息序号
			strcat(sentence, ",");		    //lq 信道为空
			strcat(sentence, ",");		    //lq 数据
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq 位填充数
			
            /*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDM语句消息序号加1 */
            msgSeq = (msgSeq + 1) % 10;                       
		}
	}
	else
	{
		/*lq 信道号字段不为空 */
		if (sixBitCodeNum <= 61)
		{
            /*lq 单语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
			strcat(sentence, ",1");		//lq 语句总数
			strcat(sentence, ",1");		//lq 语句序号
			strcat(sentence, ",");		//lq 消息序号，单语句消息的消息序号字段为空
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1);//lq 信道			
			strcat(sentence, ",");		//lq 数据
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq 位填充数

			/*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq 多语句 */
            if (sixBitCodeNum % 60 == 0)
            {
                senTotal = sixBitCodeNum / 60;
                temp16 = 60;
            }
            else
            {
                senTotal = sixBitCodeNum / 60 + 1;
                temp16 = sixBitCodeNum % 60;            
            }            

            /*lq 最大长度语句 */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDM");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq 语句总数
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq 语句序号
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq 消息序号
    			strcat(sentence, ",");			
    			strncat(sentence, &channel, 1); //lq 信道			
    			strcat(sentence, ",");		    //lq 数据
    			for (i = 0; i < 60; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq 位填充数
    
    			/*lq 存入语句队列*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq 该消息的最后一条语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDM");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq 语句总数
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq 语句序号
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq 消息序号
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1); //lq 信道
			strcat(sentence, ",");		    //lq 数据
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq 位填充数
			
            /*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDM语句消息序号加1 */
            msgSeq = (msgSeq + 1) % 10;                       
  		}	
	}
}

/*******************************************************************************
* 名称  : AISDI_BuildVDO
* 描述  : 组建VDO语句
* 输入  : listener - 侦听者ID
          data - 消息
          len - 消息字节长度
          channel - 接收消息的信道符号  		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_BuildVDO(U8 listener[], U8 data[], U16 len, U8 channel)
{
	static U8 msgSeq = 0;
	U8 	senNum;
	U8	senTotal;
	U8	fillBitNum;
	U16 sixBitCodeNum;
    U16 temp16;
	AISDISenBuffArray sentence;
	BITMAP msg_bitmap;
	U8 len_1;
	U8 len_2;
	U8 i;
    U8 temp8;
	U8 tempBuff[10];

	memset((U8*)&msg_bitmap, 0, sizeof(BITMAP));
	msg_bitmap.array = data;
	msg_bitmap.array_size = len;
	
	if ((len * 8) % 6 == 0)
	{
		sixBitCodeNum = (len * 8) / 6;
		fillBitNum = 0;	
	}
	else
	{
		sixBitCodeNum = (len * 8) / 6 + 1;
		fillBitNum = 6 - (len * 8) % 6;
	}

	if (channel != 'A' && channel != 'B')
	{
		/*lq 信道号字段为空 */
		if (sixBitCodeNum <= 62)
		{
            /*lq 单语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");			
			strcat(sentence, ",1");		//lq 语句总数
			strcat(sentence, ",1");		//lq 语句序号
			strcat(sentence, ",");		//lq 消息序号，单语句消息的消息序号字段为空
			strcat(sentence, ",");		//lq 信道为空
			strcat(sentence, ",");		//lq 数据
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq 位填充数

			/*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq 多语句 */
            if (sixBitCodeNum % 61 == 0)
            {
                senTotal = sixBitCodeNum / 61;
                temp16 = 61;
            }
            else
            {
                senTotal = sixBitCodeNum / 61 + 1;
                temp16 = sixBitCodeNum % 61;            
            }            

            /*lq 最大长度语句 */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDO");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq 语句总数
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq 语句序号
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq 消息序号
    			strcat(sentence, ",");		    //lq 信道为空
    			strcat(sentence, ",");		    //lq 数据
    			for (i = 0; i < 61; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq 位填充数
    
    			/*lq 存入语句队列*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq 该消息的最后一条语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq 语句总数
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq 语句序号
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq 消息序号
			strcat(sentence, ",");		    //lq 信道为空
			strcat(sentence, ",");		    //lq 数据
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq 位填充数
			
            /*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDO语句消息序号加1 */
            msgSeq = (msgSeq + 1) % 10;                       
		}
	}
	else
	{
		/*lq 信道号字段不为空 */
		if (sixBitCodeNum <= 61)
		{
            /*lq 单语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
			strcat(sentence, ",1");		//lq 语句总数
			strcat(sentence, ",1");		//lq 语句序号
			strcat(sentence, ",");		//lq 消息序号，单语句消息的消息序号字段为空
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1);//lq 信道			
			strcat(sentence, ",");		//lq 数据
			for (i = 0; i < sixBitCodeNum - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	//lq 位填充数

			/*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);							
		}
		else
		{
            /*lq 多语句 */
            if (sixBitCodeNum % 60 == 0)
            {
                senTotal = sixBitCodeNum / 60;
                temp16 = 60;
            }
            else
            {
                senTotal = sixBitCodeNum / 60 + 1;
                temp16 = sixBitCodeNum % 60;            
            }            

            /*lq 最大长度语句 */
            for (senNum = 1; senNum < senTotal; senNum++)
            {
            	sprintf(sentence, "!");
            	strncat(sentence, listener, 2);
            	strcat(sentence, "VDO");
			    sprintf(tempBuff, ",%d", senTotal); 
    			strcat(sentence, tempBuff);		//lq 语句总数
			    sprintf(tempBuff, ",%d", senNum); 
    			strcat(sentence, tempBuff);		//lq 语句序号
			    sprintf(tempBuff, ",%d", msgSeq); 
    			strcat(sentence, tempBuff);		//lq 消息序号
    			strcat(sentence, ",");			
    			strncat(sentence, &channel, 1); //lq 信道			
    			strcat(sentence, ",");		    //lq 数据
    			for (i = 0; i < 60; i++)
    			{
    				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
    				temp8 = SixBinCodeToAscii(temp8);
    				strncat(sentence, &temp8, 1);
    			}
    			strcat(sentence, ",0");	        //lq 位填充数
    
    			/*lq 存入语句队列*/
    			AISDI_SenEnque(&AisDiSenQue, sentence);            
            }

            /*lq 该消息的最后一条语句 */
        	sprintf(sentence, "!");
        	strncat(sentence, listener, 2);
        	strcat(sentence, "VDO");
		    sprintf(tempBuff, ",%d", senTotal); 
			strcat(sentence, tempBuff);		//lq 语句总数
		    sprintf(tempBuff, ",%d", senNum); 
			strcat(sentence, tempBuff);		//lq 语句序号
		    sprintf(tempBuff, ",%d", msgSeq); 
			strcat(sentence, tempBuff);		//lq 消息序号
			strcat(sentence, ",");			
			strncat(sentence, &channel, 1); //lq 信道
			strcat(sentence, ",");		    //lq 数据
			for (i = 0; i < temp16 - 1; i++)
			{
				bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6);
				temp8 = SixBinCodeToAscii(temp8);
				strncat(sentence, &temp8, 1);
			}
			bitmap_get_value_for_U8(&msg_bitmap, &temp8, 6 - fillBitNum);
			temp8 = SixBinCodeToAscii(temp8 << fillBitNum);
			strncat(sentence, &temp8, 1);
			sprintf(tempBuff, ",%d", fillBitNum); 
			strcat(sentence, tempBuff);	    //lq 位填充数
			
            /*lq 存入语句队列*/
			AISDI_SenEnque(&AisDiSenQue, sentence);

            /*lq VDO语句消息序号加1 */
            msgSeq = (msgSeq + 1) % 10;                       
  		}	
	}
}

/*******************************************************************************
* 名称  : AISDI_BuildABK
* 描述  : 组建ABK语句
* 输入  : listener - 侦听者ID
          addrMmsi - 寻址设备的MMSI
          rxChannel - 7号消息和13号消息接收到的信道
          msgId - ABK语句进行确认的消息的ID
          ackType - 确认类型
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_BuildABK(U8 listener[], U32 addrMmsi, AisChannelEnum rxChannel,
                    U8 msgId, U8 msgSeqNum, U8 ackType)
{
	AISDISenBuffArray sentence;
	U8 tempBuff[16];
	
    /*lq 组建语句 */
	sprintf(sentence, "$");
	strncat(sentence, listener, 2);
	strcat(sentence, "ABK");
	strcat(sentence, ",");
	sprintf(tempBuff, "%09d", addrMmsi);    			
	strcat(sentence, tempBuff);		        //lq MMSI of the addressed AIS unit
	strcat(sentence, ",");
	if (rxChannel == ENUM_AIS_CHANNEL_A)    //lq AIS channel of reception
    {
	    strcat(sentence, AIS_CHANNEL_NAME_A);
    }
    else
    {
	    strcat(sentence, AIS_CHANNEL_NAME_B);
    }
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", msgId);    			
	strcat(sentence, tempBuff);		        //lq ITU-R M.1371Message ID
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", msgSeqNum);    			
	strcat(sentence, tempBuff);		        //lq Message sequence number
	strcat(sentence, ",");
	sprintf(tempBuff, "%d", ackType);    			
	strcat(sentence, tempBuff);		        //lq Type of acknowledgement
    
    /*lq 存入语句队列*/
    AISDI_SenEnque(&AisDiSenQue, sentence);
}

/*******************************************************************************
* 名称  : AISDI_SenPrint
* 描述  : 串口输出AIS语句
* 输入  : pAisDiSenFifoStruct - AisDiSenFifoStruct指针
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_SenPrint(AisDiSenFifoStruct* pAisDiSenFifoStruct)
{
    AISDISenBuffArray sentence;
#if 0    
    while (AISDI_SenDeque(pAisDiSenFifoStruct, sentence) == TRUE)
    {
        UartResponseMsg(sentence);
    }
#else
    if (AISDI_SenDeque(pAisDiSenFifoStruct, sentence) == TRUE)
    {
        UartResponseMsg(sentence);
    }    
#endif        
}

/*******************************************************************************
* 名称  : AISDI_SentencePrint
* 描述  : 串口输出AIS语句
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_SentencePrint(void)
{
    AISDI_SenPrint(&AisDiSenQue);
}

/*******************************************************************************
* 名称  : AISDI_SentenceParse
* 描述  : AIS数字接口标准语句解析。当通过一个语句配置多个参数时，若某字段为空则
          忽略该字段，仅当某字段的取值无效时才认为配置失败（在其之前的配置成功）,
          若所有字段或者为空或者取值有效则配置成功。
* 输入  : pDat-数据缓存指针，uiLen-数据长度
* 输出  : 无
* 返回  : TRUE-消未处理，FALSE-消息已处理
*******************************************************************************/
U8 AISDI_SentenceParse(U8 *pDat, U16 uiLen)
{
	U8 chTemp[AIS_DI_SENTENCE_MAXLEN + 1];
    U8 chTemp2[AIS_DI_SENTENCE_MAXLEN + 1];
	U8 	ucCheck;
	U8 	ucLen;
	U32 ulTemp;
	U32 ulTemp1;
	U32 ulTemp2;
	U32 ulTemp3;
	U32 ulTemp4;
    U8 *pEnd;

    /*lq AIABM-AIS Broadcast binary message更新*/
	if (strncmp((char *)pDat, "AIABM", 5) == FALSE)
    {
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            /**************Total number of sentences***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_SEN_TOTAL_MIN
                        || ulTemp > AIS_DI_SEN_TOTAL_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp1 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

            /**************Sentence number***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_SEN_NUM_MIN
                        || ulTemp > AIS_DI_SEN_NUM_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp2 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

			if (ulTemp1 > 1)    //lq 语句总数为1时可省略消息序号字段
            {
                /**************Sequential message identifier***************/
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                
                /*lq 字段非空*/
                if (ucCheck != FALSE)
    			{
        			/*lq 长度有效性判断*/
        			ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);
        			}
    
                    ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                    
                    /*lq 字符有效性判断*/
        			if ((*pEnd) != '\0')
        			{
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq 取值范围有效性判断*/
                        if (ulTemp < AIS_DI_MSG_SEQ_MIN
                            || ulTemp > AIS_DI_MSG_SEQ_MAX)
                        {
            				UartResponseMsg("$DUAIR,0,ABM");
            				return (FALSE);                
                        }
    
                        ulTemp3 = ulTemp;
                    }
                }
                else
                {
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);            
                }            
            }

            /**************MMSI of the destination AIS unit***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 9)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    ABMPara.MmsiDest = ulTemp;                                
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);                            
            }            

            /**************channel for broadcast***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_ABM_CHN_MIN
                        || ulTemp > AIS_DI_ABM_CHN_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }
                    
                    switch(ulTemp)
                    {
                        case 1:
                            ABMPara.Channel = ENUM_AIS_CHANNEL_A;                                
                            break;
                        case 2:
                            ABMPara.Channel = ENUM_AIS_CHANNEL_B;                                
                            break;
                        default :
                            ABMPara.Channel = ENUM_AIS_CHANNEL_BOTH;                                
                            break;
                    }
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);                            
                }
            }            

            /**************Message ID***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 2)                         //lq ABM语句与BBM语句的该字段长度不同
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (!((ulTemp == 6) || (ulTemp == 12)))
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }
                    
                    ABMPara.MsgID = ulTemp;                                
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {				
                    UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);            
                }
            } 

            /**************Encapsulated data***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 60);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 60)))
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                strcpy(chTemp2, chTemp);
            }
            else
            {
                UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

            /**************Number of fill-bits***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 8, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,ABM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_ABM_FILL_BIT_MIN
                        || ulTemp > AIS_DI_ABM_FILL_BIT_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,ABM");
        				return (FALSE);                
                    }

                    ulTemp4 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,ABM");
				return (FALSE);            
            }

        	if (ulTemp2 == 1)								                    //lq 第一条语句
        	{
                ABMPara.ValidFlg = FALSE;

        	    /*lq 第一条语句始终覆盖之前的内容 */
                strcpy(ABMPara.EncapData, chTemp2);
                ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        
        		if (ulTemp1 > 1)
        		{
        			/*lq 记录多语句消息的当前语句的状态 */
        			ABMPara.SentenceTotal = ulTemp1;
        			ABMPara.SentenceNumber = ulTemp2;
        			ABMPara.MessageNumber = ulTemp3;
        		}
        		else
        		{
        			/*lq 单条语句 */
        			ABMPara.FillBitNumber = ulTemp4;
                    ABMPara.ValidFlg = TRUE;
        		}
        	}
        	else if ((ulTemp3 == ABMPara.MessageNumber)
        				&& (ulTemp2 == (ABMPara.SentenceNumber + 1))
        				&& (ulTemp2 <= ABMPara.SentenceTotal))					//lq 非第一条语句 
        	{
        		if (ulTemp2 < ABMPara.SentenceTotal)							//lq 多语句消息的中间语句
        		{
        			ABMPara.SentenceNumber++;
        			strcat(ABMPara.EncapData, chTemp2);
                    ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        		}
        		else if (ulTemp2 == ABMPara.SentenceTotal)						//lq 多语句消息的最后一条语句
        		{       
        			/*lq 转存完整的Encapsulated Data */
        			strcat(ABMPara.EncapData, chTemp2);
                    ABMPara.EncapDataLen = strlen(ABMPara.EncapData);
        			ABMPara.FillBitNumber = ulTemp4;
                    ABMPara.ValidFlg = TRUE;
        		}
        	}

            UartResponseMsg("$DUAIR,1,ABM");
        }        
    }

    /*lq AIBBM-AIS Broadcast binary message更新*/
	else if (strncmp((char *)pDat, "AIBBM", 5) == FALSE)
    {
		if (DevDynamicPara.ucWorkMode == EQ_WORKMOD_NORMAL)
		{
            /**************Total number of sentences***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 1, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_SEN_TOTAL_MIN
                        || ulTemp > AIS_DI_SEN_TOTAL_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp1 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

            /**************Sentence number***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 2, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_SEN_NUM_MIN
                        || ulTemp > AIS_DI_SEN_NUM_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp2 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

			if (ulTemp1 > 1)    //lq 语句总数为1时可省略消息序号字段
            {
                /**************Sequential message identifier***************/
                ucCheck = GetSegment((char *)pDat, chTemp, 3, uiLen, 30);
                
                /*lq 字段非空*/
                if (ucCheck != FALSE)
    			{
        			/*lq 长度有效性判断*/
        			ucLen = strlen(chTemp);
        			if (ucLen != 1)
        			{
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);
        			}
    
                    ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                    
                    /*lq 字符有效性判断*/
        			if ((*pEnd) != '\0')
        			{
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);
        			}
                    else
                    {
                        /*lq 取值范围有效性判断*/
                        if (ulTemp < AIS_DI_MSG_SEQ_MIN
                            || ulTemp > AIS_DI_MSG_SEQ_MAX)
                        {
            				UartResponseMsg("$DUAIR,0,BBM");
            				return (FALSE);                
                        }
    
                        ulTemp3 = ulTemp;
                    }
                }
                else
                {
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);            
                }            
            }

            /**************channel for broadcast***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 4, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_BBM_CHN_MIN
                        || ulTemp > AIS_DI_BBM_CHN_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }
                    
                    switch(ulTemp)
                    {
                        case 1:
                            BBMPara.Channel = ENUM_AIS_CHANNEL_A;                                
                            break;
                        case 2:
                            BBMPara.Channel = ENUM_AIS_CHANNEL_B;                                
                            break;
                        default :
                            BBMPara.Channel = ENUM_AIS_CHANNEL_BOTH;                                
                            break;
                    }
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);                            
                }
            }            

            /**************Message ID***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 5, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 2)))    //lq ABM语句与BBM语句的该字段长度不同
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (!((ulTemp == 8) || (ulTemp == 14)))
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }
                    
                    BBMPara.MsgID = ulTemp;                                
                }
            }
            else
            {
                if (ulTemp2 == 1)
                {				
                    UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);            
                }
            } 

            /**************Encapsulated data***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 6, uiLen, 60);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (!((ucLen >= 1) && (ucLen <= 60)))
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                strcpy(chTemp2, chTemp);
            }
            else
            {
                UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

            /**************Number of fill-bits***************/
            ucCheck = GetSegment((char *)pDat, chTemp, 7, uiLen, 30);
            
            /*lq 字段非空*/
            if (ucCheck != FALSE)
			{
    			/*lq 长度有效性判断*/
    			ucLen = strlen(chTemp);
    			if (ucLen != 1)
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}

                ulTemp = (U32)strtoul(chTemp, &pEnd, 10);
                
                /*lq 字符有效性判断*/
    			if ((*pEnd) != '\0')
    			{
    				UartResponseMsg("$DUAIR,0,BBM");
    				return (FALSE);
    			}
                else
                {
                    /*lq 取值范围有效性判断*/
                    if (ulTemp < AIS_DI_BBM_FILL_BIT_MIN
                        || ulTemp > AIS_DI_BBM_FILL_BIT_MAX)
                    {
        				UartResponseMsg("$DUAIR,0,BBM");
        				return (FALSE);                
                    }

                    ulTemp4 = ulTemp;
                }
            }
            else
            {
				UartResponseMsg("$DUAIR,0,BBM");
				return (FALSE);            
            }

        	if (ulTemp2 == 1)								                    //lq 第一条语句
        	{
                BBMPara.ValidFlg = FALSE;

        	    /*lq 第一条语句始终覆盖之前的内容 */
                strcpy(BBMPara.EncapData, chTemp2);
                BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        
        		if (ulTemp1 > 1)
        		{
        			/*lq 记录多语句消息的当前语句的状态 */
        			BBMPara.SentenceTotal = ulTemp1;
        			BBMPara.SentenceNumber = ulTemp2;
        			BBMPara.MessageNumber = ulTemp3;
        		}
        		else
        		{
        			/*lq 单条语句 */
        			BBMPara.FillBitNumber = ulTemp4;
                    BBMPara.ValidFlg = TRUE;
        		}
        	}
        	else if ((ulTemp3 == BBMPara.MessageNumber)
        				&& (ulTemp2 == (BBMPara.SentenceNumber + 1))
        				&& (ulTemp2 <= BBMPara.SentenceTotal))					//lq 非第一条语句 
        	{
        		if (ulTemp2 < BBMPara.SentenceTotal)							//lq 多语句消息的中间语句
        		{
        			BBMPara.SentenceNumber++;
        			strcat(BBMPara.EncapData, chTemp2);
                    BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        		}
        		else if (ulTemp2 == BBMPara.SentenceTotal)						//lq 多语句消息的最后一条语句
        		{       
        			/*lq 转存完整的Encapsulated Data */
        			strcat(BBMPara.EncapData, chTemp2);
                    BBMPara.EncapDataLen = strlen(BBMPara.EncapData);
        			BBMPara.FillBitNumber = ulTemp4;
                    BBMPara.ValidFlg = TRUE;
        		}
        	}

            UartResponseMsg("$DUAIR,1,BBM");
        }        
    }

    else
    {
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
* 名称  : AISDI_ABMIsValid
* 描述  : ABM数据是否有效
* 输入  : pABMStruct - ABMStruct结构体指针
* 输出  : 无
* 返回  : ABM数据是否有效
*******************************************************************************/
U8 AISDI_ABMIsValid(ABMStruct *pABMStruct)
{
    return pABMStruct->ValidFlg;
}

/*******************************************************************************
* 名称  : AISDI_BBMIsValid
* 描述  : BBM数据是否有效
* 输入  : pBBMStruct - BBMStruct结构体指针
* 输出  : 无
* 返回  : BBM数据是否有效
*******************************************************************************/
U8 AISDI_BBMIsValid(BBMStruct *pBBMStruct)
{
    return pBBMStruct->ValidFlg;
}

/*******************************************************************************
* 名称  : AISDI_ModuleInit
* 描述  : 模块初始化
* 输入  : 无
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void AISDI_ModuleInit()
{
	AISDI_InitAisDiSenFifoStruct(&AisDiSenQue);
    AISDI_InitABMStruct(&ABMPara);
    AISDI_InitBBMStruct(&BBMPara);
}
