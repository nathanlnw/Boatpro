/****************************************************************************
*  Copyright (C), 2011, Institute Mod. Opt. NKU                             *
*  FileName: Dac.h	 	                        	          	            *
*  Description:  DAC驱动控制初始化函数声明									*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/

#ifndef __DAC_H
#define __DAC_H

#define DA_VALUE_01V    15  //lq 0.01v对应的变化值

extern void DAC_RegInit(void);
extern void SetDAC1Val(S16 siDACVal);
extern void SetDAC2Val(S16 siDACVal);

#endif
