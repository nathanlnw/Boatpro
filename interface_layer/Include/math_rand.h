/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: math_rand.h
  Author: lq       Version : V1.00        Date: 2013.11.04
  Description:     随机数生成模块的宏、结构体定义，变量、函数声明
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/

#ifndef _MATH_RAND_H
#define _MATH_RAND_H

#include "def.h"

/*随机数结构体*/
struct math_rand 
{
	U32 seed;	/*随机数种子*/
};



extern U16 RandData(struct math_rand* _rand, U16 a, U16 b);
extern void SetRandSeed(struct math_rand* _rand, U32 seed);


#endif
