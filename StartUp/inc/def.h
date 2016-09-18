/****************************************************************************
*  Copyright (C), 2011, Institute Mod. Opt. NKU                             *
*  FileName: def.h	 	                        	          	            *
*  Description:  变量类型定义									*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
#ifndef __DEF_H__
#define __DEF_H__

#define U8  unsigned char
#define S8  signed char	
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned long
#define S32 signed long int

#define AU8 unsigned char
#define AS8 signed char
#define AU16 unsigned short
#define AS16 signed short
#define AU32 unsigned long int
#define AS32 signed long int

#define TRUE 	1   
#define FALSE 	0

/* 为方便移植，建议使用下面写法 */
typedef unsigned char bit;
typedef bit State;
typedef unsigned char	BOOL;
typedef unsigned char	uint8,BYTE;
typedef unsigned short	uint16,WORD;
typedef unsigned long   uint32,DWORD,ULONG;
typedef long LONG;
typedef unsigned int	UINT;

typedef signed  char	sint8;
typedef signed	int		sint16;
typedef signed	long	sint32;
typedef signed  char	int8;
typedef signed  int		int16;
typedef signed  long	int32;



#endif //__DEF_H__



