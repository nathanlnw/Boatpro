/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042.c
  Author: lq       Version : V1.00        Date: 2014.05.12
  Description:     CMX7042驱动
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "stm32f10x_lib.h"
#include "def.h"
#include "CMX7042.h"
#include "SPI.h"
#include "wdg.h"

#if defined(CMX7042_FI_VER_1_2_4_0)
#include "7032-1.2.4.0.h"

#elif defined(CMX7042_FI_VER_1_0)
#include "EF0402_M25P10A_10.h"

#endif

CMX7042Struct CMX7042Instance;

/*******************************************************************************
* 名称  : CMX7042_InitStruct
* 描述  : 初始化结构体CMX7042Struct变量
* 输入  : pCMX7042Struct-结构体指针		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
static CMX7042_InitStruct(CMX7042Struct* pCMX7042Struct)
{
    pCMX7042Struct->ProductIdCode = 0;
    pCMX7042Struct->FIVersionCode = 0;
    pCMX7042Struct->ActivedFlg = FALSE;
    pCMX7042Struct->StatusReg = 0x0;
    pCMX7042Struct->Status2Reg = 0x0;
    pCMX7042Struct->Status2RegOld = 0x0;
    pCMX7042Struct->IntMaskReg = 0x0;
    pCMX7042Struct->Rx1ErrFlg = FALSE;
    pCMX7042Struct->Rx2ErrFlg = FALSE;
    pCMX7042Struct->IntFlg = FALSE;
	pCMX7042Struct->SlotCnt = 0;
	pCMX7042Struct->PhaseCnt = 0;
	pCMX7042Struct->SlotCalibFlg = FALSE;
}

/*******************************************************************************
* 名称  : CMX7042_InitInstance
* 描述  : 初始化结构体CMX7042Struct实例
* 输入  : 无	  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_InitInstance(void)
{
    CMX7042_InitStruct(&CMX7042Instance);
    CMX7042_SetStatusBitFlg(CMX7042_STATUS_BITMASK_ComRegFree
                            | CMX7042_STATUS_BITMASK_DataTaskComp
                            | CMX7042_STATUS_BITMASK_CnfgTaskComp
                            | CMX7042_STATUS_BITMASK_TBFree);

}

/*******************************************************************************
* 名称  : CMX7042_InitPins
* 描述  : 初始化CMX7042引脚
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_InitPins(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

    /*lq ------------------CMX7042 VCC 控制引脚配置---------------------------*/
    /* Configure CMX7042 VCC ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_VCC_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_VCC_PORT, &GPIO_InitStructure);
    CMX7042_VCC_HIGH();        

    /*lq ------------------CMX7042 SLOTCLK 引脚配置---------------------------*/
    /* Configure CMX7042 SLOTCLK ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_SLOTCLK_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_SLOTCLK_PORT, &GPIO_InitStructure);
	CMX7042_SLOTCLK_LOW();

    /*lq ------------------CMX7042 RSSI1/RSSI2 引脚配置-----------------------*/
    /* Configure CMX7042 RSSI1/RSSI2 ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_RSSI1_PIN | CMX7042_RSSI2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(CMX7042_RSSI_PORT, &GPIO_InitStructure);
    CMX7042_RSSI1_LOW();        
    CMX7042_RSSI2_LOW();        

    /*lq ------------------CMX7042 IRQN 引脚配置-----------------------*/
    /* Configure CMX7042 IRQN ---------------------------------*/
    GPIO_InitStructure.GPIO_Pin = CMX7042_IRQN_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    
    GPIO_Init(CMX7042_IRQN_PORT, &GPIO_InitStructure); 
    
	/* Connect EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(CMX7042_IRQN_SOURCE_PORT, CMX7042_IRQN_SOURCE_PIN);
	
	/* Configure EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = CMX7042_IRQN_LINE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 

//lqdb
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
//lq~    
    /*lq ------------------CMX7042 C-BUS interface 引脚配置-----------------------*/
    SPI2_Init();
}

/*******************************************************************************
* 名称  : CMX7042_Init
* 描述  : 初始化CMX7042
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_Init(void)
{
    CMX7042_InitInstance();
    CMX7042_InitPins();
}

#if 1
/*******************************************************************************
* 名称  : CMX7042_IRQS
* 描述  : CMX7042中断服务程序
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_IRQS(void)
{
    U16 status = 0;
    U16 status2 = 0;

	if(EXTI_GetITStatus(CMX7042_IRQN_LINE) != RESET)
	{
#if 1
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
#endif
        CMX7042Instance.IntFlg = TRUE;
#if 0
		do
		{
	        /*lq 读状态寄存器 */
	        status = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
	        if ((status & CMX7042_STATUS_BITMASK_IRQ) != 0)         //lq 确有中断触发
	        {
	            CMX7042Instance.StatusReg |= status;
	
	            /*lq 判断接收状态是否改变 */
                if ((status & CMX7042_STATUS_BITMASK_RxState) != 0)
	            {
                    CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
	                CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

                    /*lq 判断RX1是否出现接收错误 */
                    if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                        || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                    {
                        CMX7042_SetRxError(CMX7042_RX_1);                        
                    }

                    /*lq 判断RX2是否出现接收错误 */
                    if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                        || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                    {
                        CMX7042_SetRxError(CMX7042_RX_2);                        
                    }
	            }            
	        }
            
            if ((status & CMX7042_STATUS_BITMASK_IRQ) == 0)
            {
                usart_printf("$CMX7042 IRQ pin low without IRQ: %04X!\r\n", status);        
            }                       
		} while (GPIO_ReadInputDataBit(CMX7042_IRQN_PORT, CMX7042_IRQN_PIN) == 0);	//lq 防止连续触发的后续中断检测不到
#endif
		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(CMX7042_IRQN_LINE);             
#if 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
#endif
    }
}
#else
/*******************************************************************************
* 名称  : CMX7042_IRQS
* 描述  : CMX7042中断服务程序
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_IRQS(void)
{
    U16 status = 0;
    U16 status2 = 0;

	if(EXTI_GetITStatus(CMX7042_IRQN_LINE) != RESET)
	{
#if 1
        GPIO_SetBits(GPIOC, GPIO_Pin_0);
#endif

		/* Clear the EXTI line pending bit */  
		EXTI_ClearITPendingBit(CMX7042_IRQN_LINE);

        /*lq 读状态寄存器 */
        status = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
        if ((status & CMX7042_STATUS_BITMASK_IRQ) != 0)         //lq 确有中断触发
        {
            CMX7042Instance.StatusReg |= status;

            /*lq 判断接收状态是否改变 */
            if ((status & CMX7042_STATUS_BITMASK_RxState) != 0)
            {
                CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
                CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

                /*lq 判断RX1是否出现接收错误 */
                if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                    || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                {
                    CMX7042_SetRxError(CMX7042_RX_1);                        
                }

                /*lq 判断RX2是否出现接收错误 */
                if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
                    || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
                {
                    CMX7042_SetRxError(CMX7042_RX_2);                        
                }
            }            
        }
        
        if ((status & CMX7042_STATUS_BITMASK_IRQ) == 0)
        {
            usart_printf("$CMX7042 IRQ pin low without IRQ: %04X!\r\n", status);        
        }                       
             
#if 1
        GPIO_ResetBits(GPIOC, GPIO_Pin_0);
#endif
    }
}
#endif

/*******************************************************************************
* 名称  : CMX7042_GetStatusReg
* 描述  : 查询CMX7042状态寄存器
* 输入  : 无		  
* 输出  : 无
* 返回  : 当前状态
*******************************************************************************/
U16 CMX7042_GetStatusReg(void)
{
    return CMX7042Instance.StatusReg;
}

/*******************************************************************************
* 名称  : CMX7042_GetStatus2Reg
* 描述  : 查询CMX7042状态2寄存器
* 输入  : 无		  
* 输出  : 无
* 返回  : 当前状态
*******************************************************************************/
U16 CMX7042_GetStatus2Reg(void)
{
    return CMX7042Instance.Status2Reg;
}

/*******************************************************************************
* 名称  : CMX7042_SetStatusBitFlg
* 描述  : 置位CMX7042状态位标志
* 输入  : statusBit - 状态位标识		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_SetStatusBitFlg(U16 statusBit)
{       
    CMX7042Instance.StatusReg |= statusBit;
}

/*******************************************************************************
* 名称  : CMX7042_ResetStatusBitFlg
* 描述  : 复位CMX7042状态位标志
* 输入  : statusBit - 状态位标识		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_ResetStatusBitFlg(U16 statusBit)
{       
    CMX7042Instance.StatusReg &= ~statusBit;
}

/*******************************************************************************
* 名称  : CMX7042_GetStatusBitFlg
* 描述  : 查询CMX7042状态位标志，状态寄存器读一次后所有的状态位都会清零
* 输入  : statusBit - 状态位标识		  
* 输出  : 无
* 返回  : 状态位标志的当前状态
*******************************************************************************/
U8 CMX7042_GetStatusBitFlg(U16 statusBit)
{           
    if ((CMX7042Instance.StatusReg & statusBit) != 0)
    {
        return TRUE;    
    }
    else
    {
        /*lq 判断是否为中断标志位 */        
        if ((CMX7042Instance.IntMaskReg & statusBit) != 0)
        {
            /*lq 中断标志位查询，有中断时才读状态寄存器 */
            if (CMX7042_GetIntFlgRst() == TRUE)                    
            {
        	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
                if ((CMX7042Instance.StatusReg & statusBit) != 0)
                {
                    return TRUE;    
                }
                else
                {
                    return FALSE;
                }             
            }
        }
        else
        {
            /*lq 非中断标志位查询，直接读状态寄存器 */
    	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
            if ((CMX7042Instance.StatusReg & statusBit) != 0)
            {
                return TRUE;    
            }
            else
            {
                return FALSE;
            }        
        }
    }
}

/*******************************************************************************
* 名称  : CMX7042_GetStatusBitFlg
* 描述  : 查询CMX7042状态位标志，若已经置位则将其复位
* 输入  : statusBit - 状态位标识		  
* 输出  : 无
* 返回  : 状态位标志的当前状态
*******************************************************************************/
U8 CMX7042_GetStatusBitFlgRst(U16 statusBit)
{           
    if ((CMX7042Instance.StatusReg & statusBit) != 0)
    {
        CMX7042_ResetStatusBitFlg(statusBit);        
        
        return TRUE;    
    }
    else
    {
        /*lq 判断是否为中断标志位 */        
        if ((CMX7042Instance.IntMaskReg & statusBit) != 0)
        {
            /*lq 中断标志位查询，有中断时才读状态寄存器 */
            if (CMX7042_GetIntFlgRst() == TRUE)                    
            {
        	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
                if ((CMX7042Instance.StatusReg & statusBit) != 0)
                {
                    CMX7042_ResetStatusBitFlg(statusBit);        

                    return TRUE;    
                }
                else
                {
                    return FALSE;
                }             
            }
        }
        else
        {
            /*lq 非中断标志位查询，直接读状态寄存器 */
    	    CMX7042Instance.StatusReg |= CMX7042_ReadWord(CMX7042_REGR_ADDR_Status);    //lq 状态寄存器读一次后所有的状态位都会清零
            if ((CMX7042Instance.StatusReg & statusBit) != 0)
            {
                CMX7042_ResetStatusBitFlg(statusBit);        

                return TRUE;    
            }
            else
            {
                return FALSE;
            }        
        }
    }
}

/*******************************************************************************
* 名称  : CMX7042_SlotClkGenerator
* 描述  : 生成CMX7042的SlotClk，该函数由48k定时中断调用
* 输入  : slotNum - 当前时隙号		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_SlotClkGenerator(S16 slotNum)
{
	if (CMX7042Instance.SlotCnt != slotNum)
	{
		/*lq 时隙号已改变 */
		CMX7042_SLOTCLK_HIGH();

		if ((CMX7042Instance.SlotCnt + 1) % 2250 != slotNum)
		{
			/*lq 时隙已重新校准 */
			CMX7042Instance.SlotCalibFlg = TRUE;
		}
		CMX7042Instance.SlotCnt = slotNum;
		CMX7042Instance.PhaseCnt = 0;
	}
	else
	{
		if (CMX7042Instance.PhaseCnt < CMX7042_SLOTCLK_PULSEWIDTH)
		{
			CMX7042Instance.PhaseCnt++;
			if (CMX7042Instance.PhaseCnt >= CMX7042_SLOTCLK_PULSEWIDTH)
			{
				/*lq 脉冲时长到 */
				CMX7042_SLOTCLK_LOW();					
			}
		}
	}
}

/*******************************************************************************
* 名称  : CMX7042_GetSlotCntCalibFlgRst
* 描述  : 查询是否需更新CMX7042的时隙号
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 是，FALSE - 否
*******************************************************************************/
U8 CMX7042_GetSlotCntCalibFlgRst(void)
{
	if (CMX7042Instance.SlotCalibFlg == TRUE)
	{
		CMX7042Instance.SlotCalibFlg = FALSE;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

/*******************************************************************************
* 名称  : CMX7042_IsRx1Ready
* 描述  : 查询CMX7042 Rx1是否有数据
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 有，FASLE - 无
*******************************************************************************/
U8 CMX7042_IsRx1Ready(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_R1BRDY) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_IsRx2Ready
* 描述  : 查询CMX7042 Rx2是否有数据
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 有，FASLE - 无
*******************************************************************************/
U8 CMX7042_IsRx2Ready(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_R2BRDY) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_IsTxBufferFree
* 描述  : 查询CMX7042 Tx Buffer是否可用
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 可用，FASLE - 不可用
*******************************************************************************/
U8 CMX7042_IsTxBufferFree(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_TBFree) == TRUE)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_IsTxDone
* 描述  : 查询CMX7042 发射是否完成
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 已完成，FASLE - 未完成
*******************************************************************************/
U8 CMX7042_IsTxDone(void)
{
    U8 txStatus;

    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_TxDone) == TRUE)
    {
        /*lq 发射完成，读取当前的发射状态 */
        CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
        CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_IsRxStateChange
* 描述  : 查询CMX7042接收状态是否改变
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE - 有，FASLE - 无
*******************************************************************************/
U8 CMX7042_IsRxStateChange(void)
{
    if (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_RxState) == TRUE)
    {
        CMX7042Instance.Status2RegOld = CMX7042Instance.Status2Reg;
        CMX7042Instance.Status2Reg = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);

        /*lq 判断RX1是否出现接收错误 */
        if ((CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_TwoLong)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_CRC)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            || (CMX7042_GetRxState(CMX7042_RX_1) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
        {
            CMX7042_SetRxError(CMX7042_RX_1);                        
        }

        /*lq 判断RX2是否出现接收错误 */
        if ((CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_TwoLong)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_CRC)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull)
            || (CMX7042_GetRxState(CMX7042_RX_2) == CMX7042_RX_STATE_ERROR_EndFlgNotByteBound))
        {
            CMX7042_SetRxError(CMX7042_RX_2);                        
        }
                
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_GetRxErrorRst
* 描述  : 查询CMX7042 Rx是否接收出错
* 输入  : rxChannel - 接收机		  
* 输出  : 无
* 返回  : TRUE - 有，FASLE - 无
*******************************************************************************/
U8 CMX7042_GetRxErrorRst(U8 rxChannel)
{
    if (rxChannel == CMX7042_RX_1)
    {
        if (CMX7042Instance.Rx1ErrFlg == TRUE)
        {
            CMX7042Instance.Rx1ErrFlg = FALSE;
            return TRUE;    
        }
        else
        {
            return FALSE;
        }    
    } 
    else
    {
        if (CMX7042Instance.Rx2ErrFlg == TRUE)
        {
            CMX7042Instance.Rx2ErrFlg = FALSE;
            return TRUE;    
        }
        else
        {
            return FALSE;
        }    
    }      
}

/*******************************************************************************
* 名称  : CMX7042_SetRxError
* 描述  : 置位CMX7042 Rx接收出错标志
* 输入  : rxChannel - 接收机		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_SetRxError(U8 rxChannel)
{
    if (rxChannel == CMX7042_RX_1)
    {
        CMX7042Instance.Rx1ErrFlg = TRUE;   
    } 
    else
    {
        CMX7042Instance.Rx2ErrFlg = TRUE;     
    }      
}

/*******************************************************************************
* 名称  : CMX7042_GetRxState
* 描述  : 查询CMX7042 rxChannel信道接收机的状态
* 输入  : rxChannel - 接收机		  
* 输出  : 无
* 返回  : state - 指定接收机的状态
*******************************************************************************/
U8 CMX7042_GetRxState(U8 rxChannel)
{
    U8 state;

    if (rxChannel == CMX7042_RX_1)
    {
        state = CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_Rx1State;    
    }
    else
    {
        state = (CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_Rx2State) >> 4;
    }

    return state;
}

/*******************************************************************************
* 名称  : CMX7042_GetRxStateOld
* 描述  : 查询CMX7042 rxChannel信道接收机的上一次的状态
* 输入  : rxChannel - 接收机		  
* 输出  : 无
* 返回  : state - 指定接收机的状态
*******************************************************************************/
U8 CMX7042_GetRxStateOld(U8 rxChannel)
{
    U8 state;

    if (rxChannel == CMX7042_RX_1)
    {
        state = CMX7042Instance.Status2RegOld & CMX7042_STATUS2_BITMASK_Rx1State;    
    }
    else
    {
        state = (CMX7042Instance.Status2RegOld & CMX7042_STATUS2_BITMASK_Rx2State) >> 4;
    }

    return state;
}

/*******************************************************************************
* 名称  : CMX7042_GetTxState
* 描述  : 查询CMX7042发射机的状态
* 输入  : 无		  
* 输出  : 无
* 返回  : 发射机的状态
*******************************************************************************/
U8 CMX7042_GetTxState(void)
{
    return ((CMX7042Instance.Status2Reg & CMX7042_STATUS2_BITMASK_TxState) >> 8);
}

/*******************************************************************************
* 名称  : CMX7042_GetIntFlgRst
* 描述  : 查询CMX7042中断标志，若已经置位则将其复位
* 输入  : 无		  
* 输出  : 无
* 返回  : 中断标志的当前状态
*******************************************************************************/
U8 CMX7042_GetIntFlgRst(void)
{
    if (CMX7042Instance.IntFlg == TRUE)
    {
        CMX7042Instance.IntFlg = FALSE;      

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*******************************************************************************
* 名称  : CMX7042_ReadWord
* 描述  : 从CMX7042读取一个字
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
U16 CMX7042_ReadWord(U8 addr)
{
    U16 RegVal;
    
    /*lq 启动SPI2 */
#if 0
    SPI_Cmd(SPI2, ENABLE);
#endif
    SPI2_NSS_LOW();             

    /*lq 发送地址 */
    SPI2_SendByte(addr);

    /*lq 接收数据*/
    RegVal = SPI2_SendByte(SPI_DUMMY_BYTE); 
    RegVal <<= 8;
    RegVal |= SPI2_SendByte(SPI_DUMMY_BYTE); 

    /*lq 关闭SPI2 */
    SPI2_NSS_HIGH();
#if 0                 
    SPI_Cmd(SPI2, DISABLE);
#endif   
    return RegVal;        
}

/*******************************************************************************
* 名称  : CMX7042_WriteWord
* 描述  : 往CMX7042写一个字
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_WriteWord(U8 addr, U16 data)
{
    /*lq 启动SPI2 */
#if 0   
    SPI_Cmd(SPI2, ENABLE);
#endif
    SPI2_NSS_LOW();             

    /*lq 发送地址 */
    SPI2_SendByte(addr);

    /*lq 发送高8位 */
    SPI2_SendByte(data >> 8); 

    /*lq 发送低8位 */
    SPI2_SendByte(data & 0xFF); 

    /*lq 关闭SPI */
    SPI2_NSS_HIGH();
#if 0                 
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* 名称  : CMX7042_WriteByte
* 描述  : 往CMX7042写一个字节
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_WriteByte(U8 addr, U8 data)
{
    /*lq 启动SPI2 */
#if 0   
    SPI_Cmd(SPI2, ENABLE);
#endif    
    SPI2_NSS_LOW();             

    /*lq 发送地址 */
    SPI2_SendByte(addr);

    /*lq 发送数据 */
    SPI2_SendByte(data); 

    /*lq 关闭SPI */
    SPI2_NSS_HIGH(); 
#if 0                
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* 名称  : CMX7042_WriteCommand
* 描述  : 往CMX7042写一个命令字节
* 输入  : 无		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_WriteCommand(U8 command)
{
    /*lq 启动SPI2 */
#if 0
    SPI_Cmd(SPI2, ENABLE);
#endif    
    SPI2_NSS_LOW();             

    /*lq 发送命令 */
    SPI2_SendByte(command);

    /*lq 关闭SPI */
    SPI2_NSS_HIGH();
#if 0                     
    SPI_Cmd(SPI2, DISABLE);
#endif
}

/*******************************************************************************
* 名称  : CMX7042_EnInt
* 描述  : 只使能指定的中断，未指定的中断保持原状态
* 输入  : interMask - 中断使能掩码		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_EnInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg |= intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* 名称  : CMX7042_UnInt
* 描述  : 只失能指定的中断，未指定的中断保持原状态
* 输入  : interMask - 中断使能掩码		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_UnInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg &= ~intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* 名称  : CMX7042_CfgInt
* 描述  : 只使能指定的中断，未指定的中断失能
* 输入  : interMask - 中断使能掩码		  
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_CfgInt(U16 intMask)
{
    CMX7042Instance.IntMaskReg = intMask;
    CMX7042_WriteWord(CMX7042_REGW_ADDR_InterruptMask, CMX7042Instance.IntMaskReg);
}

/*******************************************************************************
* 名称  : CMX7042_DTaskDataWrite
* 描述  : 执行一个数据写的数据任务
* 输入  : data - 要写的数据	
	      bitLen - 比特数（1-64）
          pointRst - data buffer point 是否复位
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_DTaskDataWrite(U16 data[4], U16 bitLen, U8 pointRst)
{
    U16 wordNum;
    U16 bitNum;

    /*lq 溢出保护 */
    if (bitLen > 64)    //lq CMX7042的每个数据任务最大转移4个字
    {
        bitLen = 64;
    }
    else if (bitLen <= 0)
    {
        return ;
    }
    wordNum = bitLen / 16;
    bitNum = bitLen % 16;

    /*lq 整字转移 */
    if (wordNum != 0)
    {
        /*lq 向数据寄存器中写数据 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif
        if (wordNum >= 1)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[0]);                       
        }
        if (wordNum >= 2)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite2, data[1]);                             
        }
        if (wordNum >= 3)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[2]);                            
        }
        if (wordNum >= 4)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, data[3]); 
        }

        /*lq 执行整字转移任务 */    
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
#endif        
        if (pointRst == TRUE)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_Tx(wordNum));                        
        }
        else
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordWrite_Tx(wordNum));                
        }  
    }

    /*lq 比特转移 */        
    if (bitNum != 0)
    {               
        /*lq 向数据寄存器DataWrite1中写数据 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif        
        CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[wordNum]);                

        /*lq 执行比特转移任务 */        
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);                   
#endif        
        if ((pointRst == TRUE) && wordNum == 0)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Tx(bitNum));                
        }
        else
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitWrite_Tx(bitNum));        
        }        
    }
}

/*******************************************************************************
* 名称  : CMX7042_DTaskDataRead
* 描述  : 执行一个数据读的数据任务
* 输入  : data - 存放读出的数据	
	      bitLen - 比特数（1-64）
          pointRst - data buffer point 是否复位
          rxBuff - 数据缓存Rx1 or Rx2
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_DTaskDataRead(U16 data[4], U16 bitLen, U8 pointRst, U8 rxBuff)
{
    U16 wordNum;
    U16 bitNum;

    /*lq 溢出保护 */
    if (bitLen > 64)    //lq CMX7042的每个数据任务最大转移4个字
    {
        bitLen = 64;
    }
    else if (bitLen <= 0)
    {
        return ;
    }
    wordNum = bitLen / 16;
    bitNum = bitLen % 16;
 

    /*lq 整字转移 */
    if (wordNum != 0)
    {
        /*lq 执行整字转移任务 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);         
#endif        
        if (pointRst == TRUE)
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_RX1(wordNum));                                    
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordReset_RX2(wordNum));                                                
            }
        }
        else
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordRead_RX1(wordNum));                                    
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataWordRead_RX2(wordNum));                                                
            }
        }

        /*lq 从数据寄存器中读数据 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);
#endif
        if (wordNum >= 1)
        {
            data[0] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);                
        }
        if (wordNum >= 2)
        {
            data[1] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead2);                
        }
        if (wordNum >= 3)
        {
            data[2] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead3);                
        }
        if (wordNum >= 4)
        {
            data[3] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead4);                
        }  
    }

    /*lq 比特转移 */        
    if (bitNum != 0)
    {               
        /*lq 执行比特转移任务 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);           
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE); 
#endif
        if ((pointRst == TRUE) && wordNum == 0)
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Rx1(bitNum));                            
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitReset_Rx2(bitNum));                                        
            }
        }
        else
        {
            if (rxBuff == CMX7042_RX_1)
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitRead_RX1(bitNum));                            
            }
            else
            {
                CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_DTASK_DataBitRead_RX2(bitNum));                                        
            }
        }
        
        /*lq 从数据寄存器DataRead1中读数据 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);         
#endif        
        data[wordNum] = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);                        
    }
}

/*******************************************************************************
* 名称  : CMX7042_DTaskDataBulkWrite
* 描述  : 执行一个批量写数据的数据任务
* 输入  : data - 要写的数据	
	      bitLen - 比特数
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_DTaskDataBulkWrite(U8* pData, U16 bitLen)
{
    U16 taskNum;    //lq 每个数据任务至多写16*4=64bit
    U16 bitNum;     //lq 最后一个数据任务需写的位数
    U16 byteNum;    //lq pData的字节总数
    U16 i;
    U16 data[4];
    U16 temp1, temp2;
    
    byteNum = (bitLen % 8) == 0 ? (bitLen / 8) : ((bitLen / 8) + 1);
    taskNum = bitLen / 64;
    bitNum = bitLen % 64;

    if (bitLen == 0)
    {
        return ;    
    }

    for (i = 0; i < taskNum; i++)    
    {
        data[0] = (pData[i * 8] << 8) | pData[i * 8 + 1];
        data[1] = (pData[i * 8 + 2] << 8) | pData[i * 8 + 3];
        data[2] = (pData[i * 8 + 4] << 8) | pData[i * 8 + 5];
        data[3] = (pData[i * 8 + 6] << 8) | pData[i * 8 + 7];
        
        if (i == 0)
        {
            CMX7042_DTaskDataWrite(data, 64, TRUE);
        }
        else
        {
            CMX7042_DTaskDataWrite(data, 64, FALSE);                   
        }             
    }

    if (bitNum != 0)
    {
        temp1 = byteNum - taskNum * 8;                          //lq 剩余字节数
        temp2 = (temp1 % 2) == 0 ? temp1 / 2 : temp1 / 2 + 1;   //lq 剩余字数

        for (i = 0; i < temp2; i++)
        {
            data[i] = pData[taskNum * 8 + i * 2] << 8;
            if ((taskNum * 8 + i * 2 + 1) < byteNum)            //lq 边界溢出保护
            {
                data[i] |= pData[taskNum * 8 + i * 2 + 1];
            }
        }

        if (taskNum == 0)
        {
            CMX7042_DTaskDataWrite(data, bitNum, TRUE);                           
        }
        else
        {
            CMX7042_DTaskDataWrite(data, bitNum, FALSE); 
        }        
    } 
}


/*******************************************************************************
* 名称  : CMX7042_DTaskDataBulkWrite
* 描述  : 执行一个批量写数据的数据任务
* 输入  : data - 要写的数据	
	      bitLen - 比特数
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_DTaskDataBulkRead(U8* pData, U16 bitLen,  U8 pointRst, U8 rxBuff)
{
    U16 taskNum;    //lq 每个数据任务至多写16*4=64bit
    U16 bitNum;     //lq 最后一个数据任务需写的位数
    U16 byteNum;    //lq pData的字节总数
    U16 i, k;
    U16 data[4];
    U16 temp1, temp2;
    
    byteNum = (bitLen % 8) == 0 ? (bitLen / 8) : ((bitLen / 8) + 1);
    taskNum = bitLen / 64;
    bitNum = bitLen % 64;

    if (bitLen == 0)
    {
        return ;    
    }

    /*lq 整数个字读取 */
    for (i = 0; i < taskNum; i++)    
    {
        if (i == 0)
        {
            CMX7042_DTaskDataRead(data, 64, pointRst, rxBuff);
        }
        else
        {
            CMX7042_DTaskDataRead(data, 64, FALSE, rxBuff);
        } 

        for (k = 0; k < 4; k++)
        {
            pData[i * 8 + k * 2] = (data[k] >> 8) & 0xFF;
            pData[i * 8 + k * 2 + 1] = data[k] & 0xFF;

        }           
    }

    /*lq 不足一个字的读取 */
    if (bitNum != 0)
    {
        temp1 = byteNum - taskNum * 8;                          //lq 剩余字节数
        temp2 = (temp1 % 2) == 0 ? temp1 / 2 : temp1 / 2 + 1;   //lq 剩余字数

        if (taskNum == 0)
        {
            CMX7042_DTaskDataRead(data, bitNum, pointRst, rxBuff);
        }
        else
        {
            CMX7042_DTaskDataRead(data, bitNum, FALSE, rxBuff);
        } 

        for (i = 0; i < temp2; i++)
        {
            pData[taskNum * 8 + i * 2] = (data[i] >> 8) & 0xFF;
            if ((taskNum * 8 + i * 2 + 1) < byteNum)            //lq 边界溢出保护
            {
                pData[taskNum * 8 + i * 2 + 1] = data[i] & 0xFF;                    
            }
        }       
    } 
}


/*******************************************************************************
* 名称  : CMX7042_ExecCfgTask
* 描述  : 执行配置任务，该配置任务可以有参数也可无参数
* 输入  : cfgTask - 配置任务
          data - 配置参数
	      wordNum - 参数字数，0-4          
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_ExecCfgTask(U16 cfgTask, U16 data[4], U16 wordNum)
{
    /*lq 溢出保护 */
    if (wordNum > 4)
    {
        wordNum = 4;
    }

    if (wordNum != 0)
    {
        /*lq 向数据寄存器中写数据 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);        
#endif
        if (wordNum >= 1)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite1, data[0]);                
        }
        if (wordNum >= 2)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite2, data[1]);                
        }
        if (wordNum >= 3)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[2]);                
        }
        if (wordNum >= 4)
        {
            CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, data[3]);                
        }
    
        /*lq 同时执行数据任务和配置任务 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif
#if 0        
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 
                          CMX7042_DTASK_DataWordWrite_Tx(wordNum) | cfgTask);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 
                          CMX7042_DTASK_DataWordReset_Tx(wordNum) | cfgTask);
#endif
    }
    else
    {
        /*lq 执行配置任务 */
#if CMX7042_FLAG_CHECK
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, cfgTask);
    }
}

/*******************************************************************************
* 名称  : CMX7042_ExecCfgTask
* 描述  : 执行不带参数的配置任务
* 输入  : cfgTask - 配置任务
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_ExecCfgTaskNoPara(U16 cfgTask)
{
    /*lq 执行配置任务 */
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);                              
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, cfgTask);
}

/*******************************************************************************
* 名称  : CMX7042_ExecModemTaskNoPara
* 描述  : 执行不带参数的调制解调任务
* 输入  : task - 任务
* 输出  : 无
* 返回  : 无
*******************************************************************************/
void CMX7042_ExecModemTaskNoPara(U16 task)
{
    /*lq 执行配置任务 */
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, task);
}

/*******************************************************************************
* 名称  : CMX7042_LoadFI
* 描述  : 载入FI
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE-成功，FALSE-失败
*******************************************************************************/
U8 CMX7042_LoadFI(void)
{
    U16 i;
    U16 db1ChkHi = 0;
    U16 db1ChkLo = 0;
    U16 db2ChkHi = 0;
    U16 db2ChkLo = 0;

    //lq... 每次重载FI之前先软件复位CMX7042

	/*lq 喂狗*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();

    /*lq 等CMX7042启动，若CMX7042启动失败，则会导致看门狗复位重启 */
    while((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status) & 0x0001) != 1);	

    /*lq 写数据块1 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, DB1_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, DB1_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    for (i = 0; i < DB1_LEN; i++)
    {
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, db1[i]);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    }
    usart_printf("$CMX7042 Data block1: done!\r\n");

	/*lq 喂狗*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();

    /*lq 写数据块2 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, DB2_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, DB2_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);   
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    for (i = 0; i < DB2_LEN; i++)
    {
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, db2[i]);
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    }
    usart_printf("$CMX7042 Data block2: done!\r\n");

	/*lq 喂狗*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();
    
    /*lq 写数据块3 */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite3, ACTIVATE_PTR);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_DataWrite4, ACTIVATE_LEN);
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, 0x0001);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
    usart_printf("$CMX7042 Data block3: done!\r\n");
    
    /*lq 读取校验码 */
    db1ChkHi = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead3);
    db1ChkLo = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead4);
    db2ChkHi = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead1);
    db2ChkLo = CMX7042_ReadWord(CMX7042_REGR_ADDR_DataRead2);
    usart_printf("$CMX7042 DB1_CHK_HI: %04X!\r\n", db1ChkHi);
    usart_printf("$CMX7042 DB1_CHK_LO: %04X!\r\n", db1ChkLo);
    usart_printf("$CMX7042 DB2_CHK_HI: %04X!\r\n", db2ChkHi);
    usart_printf("$CMX7042 DB2_CHK_LO: %04X!\r\n", db2ChkLo);
          
    if ((db1ChkHi == DB1_CHK_HI)
        && (db1ChkLo == DB1_CHK_LO)
        && (db2ChkHi == DB2_CHK_HI)
        && (db2ChkLo == DB2_CHK_LO))
    {
        /*lq 读取设备ID和FI版本号 */
        CMX7042Instance.ProductIdCode = CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2);
        CMX7042Instance.FIVersionCode = CMX7042_ReadWord(CMX7042_REGR_ADDR_ADCData);
        usart_printf("$CMX7042 ProductIdCode:%04X!\r\n", CMX7042Instance.ProductIdCode);
        usart_printf("$CMX7042 FIVersionCode:%04X!\r\n", CMX7042Instance.FIVersionCode);

        /*lq 发送激活码高位 */
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, ACTIVATE_CHK_HI);
        usart_printf("$CMX7042 Send activecodeHigh done!\r\n");
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
        
        /*lq 发送激活码低位 */
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, ACTIVATE_CHK_LO);
        usart_printf("$CMX7042 Send activecodeLow done!\r\n");
        while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ACT) == FALSE);
        CMX7042Instance.ActivedFlg = TRUE;
        usart_printf("$CMX7042 Actived successly!\r\n");

    	/*lq 喂狗*/
        Is_Feed_Dog = TRUE;
    	Feed_Dog();                             

        return TRUE;
    }
    else
    {       
        /*lq FI载入失败 */    
        return FALSE;
    }                                 
}

/*******************************************************************************
* 名称  : CMX7042_Configuration
* 描述  : CMX7042配置
* 输入  : 无		  
* 输出  : 无
* 返回  : TRUE-成功，FALSE-失败
*******************************************************************************/
void CMX7042_Configuration(void)
{
    U16 data[4];

    /*lq ---------------直接配置寄存器---------------------- */
    /*lq Interrupt Mask Register - $CE 16-bit write */      
	CMX7042_CfgInt(CMX7042_STATUS_BITMASK_IRQ
                   //lq | CMX7042_STATUS_BITMASK_TxDone
				   | CMX7042_STATUS_BITMASK_RxState
                   | CMX7042_STATUS_BITMASK_R1BRDY
                   | CMX7042_STATUS_BITMASK_R2BRDY
                   | CMX7042_STATUS_BITMASK_TBFree);		//lq 开启7042中断	

    /*lq System CLK 1 and 2 PLL Data Registers - $AB, $AD write */
    //lq 不需配置，硬件未使用

    /*lq System CLK 1 and 2 Ref Registers - $AC and $AE write */
    //lq 不需配置，硬件未使用

    /*lq Input/Output Gain and Routing Register - $B1 write */
    CMX7042_WriteWord(CMX7042_REGW_ADDR_IOGainAndRouting, 0xB480);//lq Input1 Gain = 16dB，Input2 Gain = 16dB，MOD2 Source: Output -> MOD2;

    /*lq Power Down Register - $C0 16-bit write */
    //lq 无需配置，由FI自动控制

    /*lq Mode Register - $C1 16-bit write: ADC Input Select, sleep EN, CSEN, CS Channel, Rx2 AIS/DSC, Rx1 AIS/DSC*/
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Mode, 0x0000);      //lq ADC Input Select = No input selected;sleep EN = false;CSEN = false;CS Channel = Rx1;Rx2 AIS/DSC = AIS; Rx1 AIS/DSC = AIS;    
    
    /*lq CS-Threshold Register - $C2 16-bit write */ 
    //lq 因CSEN = false，不需配置CS-Threshold Register

    /*lq ---------------进入配置模式---------------------- */
    while ((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2) & CMX7042_STATUS2_BITMASK_Conf) == 0)
    {
#if 0
        CMX7042_ExecCfgTaskNoPara(CMX7042_MTASK_EnterConfigMode);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_MTASK_EnterConfigMode);
#endif
        Delay_ms(5);    
    }

    /*lq ---------------使用Configuration Task 进行配置---------------------- */
    /*lq System Options */
    data[0] = 0x0001;                                               //lq 9.6MHz xtal or oscillator; enable CRC checking;
    CMX7042_ExecCfgTask(CMX7042_CTASK_SystemOptions, data, 1);      //lq 执行该配置任务

    /*lq Tx 2-point or I/Q mode*/
    //lq... 使用默认配置: 2-point modulation, MOD1 and MOD2 normal polarity.

    /*lq Tx MOD levels */
    //lq... 使用默认配置：MOD1 = 0dB, MOD2 = 0dB.

    /*lq RSSI look-up table */
    //lq... 使用默认值

    /*lq RSSI window */
    //lq... 使用默认值: start time = 60, 24bits; window length = 27, approx. 11bits;

    /*lq RSSI gain */
    //lq... 使用默认值：Default value is $025F ( x 0.0185)

    /*lq CSTDMA gain */
    //lq... 使用默认值：Default value is $025F ( x 0.0185)
#if 0 //lq 标准配置
    /*lq Tx Sequence */
    data[0] = 0x0000;                               //lq Sequence Start Time = 0bit
    data[1] = 0x0001;                               //lq TXENA initial state = 1
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_START event ID , 因CS未使能，故用dummy事件
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, TRUE);         //lq 该组数据任务的第一个任务需复位数据缓存指针
    
    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_START; //lq MODULATE_START event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_END event ID , 因CS未使能，故用dummy事件
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, FALSE);
    
    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_lo;       //lq Tx_en_lo event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq RAMDAC_UP event ID, 未使用RAMDAC_UP，用dummy事件占位
    data[3] = 0x0003;                               //lq timer ~ 1bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq RAMDAC_DOWN event ID，未使用RAMDAC_DOWN，用dummy事件占位
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_Tx_en_hi;       //lq Tx_en_hi event ID
    data[3] = 0x0014;                               //lq timer = 8bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_END;   //lq MODULATE_END event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    CMX7042_DTaskDataWrite(data, 32, FALSE);
#else
    /*lq Tx Sequence */
    data[0] = 0x0000;                               //lq Sequence Start Time = 0bit
    data[1] = 0x0001;                               //lq TXENA initial state = 1
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_START event ID , 因CS未使能，故用dummy事件
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, TRUE);         //lq 该组数据任务的第一个任务需复位数据缓存指针
    
    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_START; //lq MODULATE_START event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_DUMMY;          //lq CSTDMA_END event ID , 因CS未使能，故用dummy事件
    data[3] = 0x0000;                               //lq timer = 0
    CMX7042_DTaskDataWrite(data, 64, FALSE);
    
    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_lo;       //lq Tx_en_lo event ID
    data[1] = 0x0005;                               //lq timer = 2bit
    data[2] = CMX7042_TXSEQ_EVENTID_RAMDAC_UP;      //lq RAMDAC_UP event ID, 未使用RAMDAC_UP，用dummy事件占位
    data[3] = 0x0008;                               //lq timer ~ 1bit
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_MODULATE_END;   //lq MODULATE_END event ID
    data[1] = 0x0000;                               //lq timer = 0
    data[2] = CMX7042_TXSEQ_EVENTID_RAMDAC_DOWN;    //lq RAMDAC_DOWN event ID，未使用RAMDAC_DOWN，用dummy事件占位
    data[3] = 0x0000;                               //lq timer = 0    
    CMX7042_DTaskDataWrite(data, 64, FALSE);

    data[0] = CMX7042_TXSEQ_EVENTID_Tx_en_hi;       //lq Tx_en_hi event ID
    data[1] = 0x0014;                               //lq timer = 8bit
    CMX7042_DTaskDataWrite(data, 32, FALSE);
#endif
#if CMX7042_FLAG_CHECK
    while (CMX7042_GetStatusBitFlg(CMX7042_STATUS_BITMASK_DataTaskComp) == FALSE);  //lq 此处不能复位，因为下面不执行数据任务，只是确认前面通过数据任务来提供的参数已完成
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_ComRegFree) == FALSE);
    while (CMX7042_GetStatusBitFlgRst(CMX7042_STATUS_BITMASK_CnfgTaskComp) == FALSE);
#endif    
    CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_CTASK_Tx_sequence);    //lq 执行该配置任务
    
    /*lq Sleep Timing */    
    //lq... 使用默认值: start time = 120;end time = 625;

    /*lq RAMDAC Control and Load */
    //lq 使用默认值，硬件电路不支持
    data[0] = 0x0006;                                               //lq AuxDAC1 in RAMDAC mode; enable AuxDAC1; no ramp profile update;
    data[1] = 0x0007;                                               //lq RAMDAC CLK div1 = 8;
    data[2] = 0x001E;                                               //lq RAMDAC CLK div2 = 30;
    CMX7042_ExecCfgTask(CMX7042_CTASK_RAMDACLoad, data, 3);         //lq 执行该配置任务

    /*lq Enter Deep Sleep */
    //lq 无需配置，由FI自动控制

    /*lq Leave Deep Sleep */
    //lq 无需配置，由FI自动控制

    /*lq Slot Number Initialization */
    //lq... 暂不配置，由SlotClk脉冲控制

    /*lq Time of Arrival Compensation */
    data[0] = 0x0000;                                               //lq... ToA compensation = 0，需后期测量
    CMX7042_ExecCfgTask(CMX7042_CTASK_SetToACompensate, data, 1);   //lq 执行该配置任务    
    usart_printf("$CMX7042 config finished!\r\n");
    usart_printf("$CMX7042[Status2Reg]:%04X!\r\n", CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2));

	/*lq 喂狗*/
    Is_Feed_Dog = TRUE;
	Feed_Dog();
            
    /*lq ---------------退出配置模式---------------------- */
    while ((CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2) & CMX7042_STATUS2_BITMASK_Conf) != 0)
    {
#if 1
        CMX7042_ExecCfgTaskNoPara(CMX7042_CTASK_EXIT_CONFIG);
#else
        CMX7042_WriteWord(CMX7042_REGW_ADDR_Command, CMX7042_CTASK_EXIT_CONFIG);
#endif
        Delay_ms(5);    
    }

    usart_printf("$CMX7042 exit config mode!\r\n");
    usart_printf("$CMX7042[Status2Reg]:%04X!\r\n", CMX7042_ReadWord(CMX7042_REGR_ADDR_Status2));

    /*lq 启动接收任务 */
    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx1_RXB);
    CMX7042_ExecModemTaskNoPara(CMX7042_MTASK_Rx2_RXB);
}
