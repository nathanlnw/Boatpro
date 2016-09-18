/**************************************************************************
*                                                                         *
*   PROJECT     : ARM port for uC/OS-II                                   *
*                                                                         *
*   MODULE      : ARMDEFS.h                                               *
*                                                                         *
*   AUTHOR      : Michael Anburaj                                         *
*                 URL  : http://geocities.com/michaelanburaj/             *
*                 EMAIL: michaelanburaj@hotmail.com                       *
*                                                                         *
*   PROCESSOR   : ARM (32 bit ARM7's, ARM9's)                             *
*                                                                         *
*   TOOL-CHAIN  : Any                                                     *
*                                                                         *
*   DESCRIPTION :                                                         *
*   ARM definitions header file.                                          *
*                                                                         *
**************************************************************************/


#ifndef __ARMDEFS_H__
#define __ARMDEFS_H__
 

#ifdef __cplusplus
extern "C" {
#endif


/* ********************************************************************* */
/* Module configuration */


/* ********************************************************************* */
/* Interface macro & data definition */

/* ARM modes */
#define USERMODE        0x10
#define FIQMODE         0x11
#define IRQMODE         0x12
#define SVCMODE         0x13
#define ABORTMODE       0x17
#define UNDEFMODE       0x1b
#define MODEMASK        0x1f
#define NOINT           0xc0
#define TBIT            0x20


/* ********************************************************************* */
/* Interface function definition */


/* ********************************************************************* */

#ifdef __cplusplus
}
#endif

#endif /*__ARMDEFS_H__*/
