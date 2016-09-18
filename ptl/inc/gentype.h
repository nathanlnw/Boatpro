/****************************************************************************
*  Copyright (C), 2008, 712.Co.,Ltd.                                        *
*  FileName: Gentype.C                                                      *
*  Description:    数据类型定义,数据运算定义                       			*
*  Function List:							                                *
*****************************************************************************
*  History:                                               				    *
*      <author>  <time>    <version >   <desc>                              *
*      Lee    	 08/03/04     1.0       build this moudle                   *
****************************************************************************/

#ifndef _GENTYPE_H
#define _GENTYPE_H

#ifndef FALSE
#define FALSE                 (0)
#endif

#ifndef TRUE
#define TRUE                  (1)
#endif


#define U8  unsigned char
#define S8  signed char	
#define U16 unsigned short
#define S16 signed short
#define U32 unsigned long
#define S32 signed long int

/*-- Global type definition for bit sensitive data ------------*/
typedef unsigned char         UCHAR;  /* Unsigned 8 bit data   */
typedef signed char           SCHAR;  /* Signed 8 bit data     */
typedef char           	      CHAR;   /* 8 bit char		*/

typedef unsigned short int    USINT;  /* Unsigned 16 bit data  */
typedef signed short int      SINT;  /* Signed 16 bit data    */

typedef unsigned long         ULINT; /* Unsigned 32 bit data  */
typedef signed long           SLINT; /* Signed 32 bit data    */

/*---------- DSP data type definition in fixed point ----------*/
typedef SINT                  QSINT;
typedef SLINT                 QSLINT;
/*------------------- End of DSP data type definition ---------*/

typedef QSINT				Q15;

#define q15_MAX				(32767)
#define q15_MIN				(-32768)

#define Q_MAX				(32767)
#define Q_MIN				(-32767)     

#define QD_MAX				(QSLINT)(0x3FFFFFFF)	/* 1073741823 */
#define QD_MIN				(QSLINT)(-1073741823)	/* 0xC000 0000 */

// saturate a Q in QD form
#define Q_SAT(x)			{ if      ((x)>Q_MAX)   (x)=Q_MAX;   else if ((x)<Q_MIN)   (x)=Q_MIN;  }

// saturate a QD 
#define QD_SAT(d)			{ if      ((d)>QD_MAX)  (d)=QD_MAX;  else if ((d)<QD_MIN)    (d)=QD_MIN; }

#define QROUND_SAT(d)		{ (d) = ((d)+0x4000)>>15; if      ((d)>Q_MAX) (d)=Q_MAX; else if ((d)<Q_MIN) (d)=Q_MIN;  }
#define QROUNDT(d)			{ (d) = ((d)+0x4000)>>15; }

#define Q_ABS(x)			(((x)<0)?(-(x)):(x))

#define CONST				const


#endif
