/****************************************************************************
*  Copyright (C), 2008, 712.Co.,Ltd.                                        *
*  FileName: DevicInit.C                                                    *
*  Description:     外设设置、中断响应函数                        			*
*  Function List:							                                *
*	1--  void FirInit(FIRStruct *pFir,										* 
*					  S16 *pCoef,											*
*					  S16 *pDline,											*
*					  S16 FirNum) 	//FIR滤波器初始化						*
*	2--  S16 FirFilter(FIRStruct *pFir,									*
*						S16 samplein)	//FIR滤波器							*
*****************************************************************************
*  History:   					                                            *
*      <author>  <time>    <version >   <desc>                              *
*      Lee    	 08/03/04     1.0       build this moudle                   *
****************************************************************************/
#include "def.h"
#include "FIRstruc.h"

void FirInit(FIRStruct *pFir, S16 *pCoef, S16 *pDline, S16 FirNum);
S16 FirFilter(FIRStruct *pFir, S16 samplein);

/************************************************************
*  Function:       	FirInit									*
*  Description:    	FIR滤波器初始化							*
*  Input:          	*pFir:	滤波器句柄						*
*				   	*pCoef:	滤波器系数指针					*
*				   	*pDline:滤波器缓存指针					*
*				   	FirNum:滤波器阶数						*
*  Output:         	无										*
*  Return:         	无										*
*  Others:         	无										*
************************************************************/
void FirInit(FIRStruct *pFir, S16 *pCoef, S16 *pDline, S16 FirNum)
{
	S16 i;
	
	pFir->pDline = pDline;  /* Double buffering  */
	pFir->pCoef  = pCoef;   /* the coefficient pointer */
	pFir->Offset = 0;
	pFir->FirNum = FirNum;
	for(i = 0; i < FirNum * 2; i++) 
	{
		pFir->pDline[i] = 0;
	}
}

/************************************************************
*  Function:       	FirFilter								*
*  Description:    	FIR滤波器运算							*
*  Input:          	*pFir:	滤波器句柄						*
*				   	samplein:采样点输入						*
*  Output:         	无										*
*  Return:         	滤波后数据输出							*
*  Others:         	无										*
************************************************************/
S16 FirFilter(FIRStruct *pFir, S16 samplein)
{
    S16  *pHead;
    S16  *pTail;
    S16  *pCoef;
	S32  sum;
	S16  sampleout;
	U8   i;
	
	
	/* This is the bengin point*/
	pHead = pFir->pDline + pFir->Offset;

	/* This is the end point*/
	pTail = pHead + pFir->FirNum;

	/* The pointer to the coefficient */
	pCoef = pFir->pCoef;
	
  	*pHead++ = samplein;
	*pTail = samplein;
	
	pFir->Offset ++;
	if(pFir->Offset >= pFir->FirNum)
	{
		pFir->Offset = 0;
		pHead = pFir->pDline;
	}
	
	sum = 0;
	for(i=0; i<pFir->FirNum; i++)        
	{
		sum += ((S32)*pHead++) * (*pCoef++);
	}                                     
    sampleout = (S16)((S32)(sum + 0x4000)>>15);
	
	return(sampleout);
}

