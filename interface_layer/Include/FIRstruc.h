/****************************************************************************
*  Copyright (C), 2008, 712.Co.,Ltd.                                        *
*  FileName: Firstruc.C                                                      *
*  Description:    数据类型定义,数据运算定义                       			*
*  Function List:							                                *
*	1--		FIRStruct;		//滤波器数据结构								*
*****************************************************************************
*  History:                                               				    *
*      <author>  <time>    <version >   <desc>                              *
*      Lee    	 08/03/04     1.0       build this moudle                   *
****************************************************************************/
#ifndef _FIRSTRUC_H_
#define _FIRSTRUC_H_

typedef struct {
    S16 *pDline;    /* delayline  */
    S16  *pCoef;    /* pointer to coefficients of filter */ /* Constant during filtering */
    S16  Offset;    /* Delayline pointer offset */
    S16  Dummy;
    S16  FirNum;
} FIRStruct;

S16 FirFilter(FIRStruct *pFir, S16 samplein);
void FirInit(FIRStruct *pFir, S16 *pCoef, S16 *pDline, S16 FirNum);

#endif
