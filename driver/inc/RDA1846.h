/******************** (C) COPYRIGHT 712 ***************************************
* File Name          : RDA1846.h
* Author             : Mobile Team 5
* Version            : V0.0.1
* Date               : 2011/09/01
* Description        : RDA1846.c的头文件。
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RDA1846_H
#define __RDA1846_H

/* Includes ------------------------------------------------------------------*/

/* Exported define -----------------------------------------------------------*/
/*lq RDA1846端口宏定义 */
#define RDA1846_SCLK_PORT   GPIOB
#define RDA1846_SCLK_PIN    GPIO_Pin_11

#define RDA1846_SDIO_PORT    GPIOB
#define RDA1846_SDIO_PIN     GPIO_Pin_10

#define RDA1846_SEN1_PORT   GPIOB
#define RDA1846_SEN1_PIN    GPIO_Pin_1

#define RDA1846_SEN2_PORT   GPIOC
#define RDA1846_SEN2_PIN    GPIO_Pin_6

#define RDA1846_PDN1_PORT   GPIOB
#define RDA1846_PDN1_PIN    GPIO_Pin_0

#define RDA1846_PDN2_PORT   GPIOC
#define RDA1846_PDN2_PIN    GPIO_Pin_7

/*lq RDA1846片选宏定义 */
#define RDA1846_CS_1   1
#define RDA1846_CS_2   2

//#define Channel_25K_Hz
#define RF_Freq 422995000
#define Chn_25K 0x01
#define Chn_12K5 0x00
#define SQ_On On
#define SQ_Off Off
/* Exported types ------------------------------------------------------------*/
typedef struct tagRDA1846Freq{
  U32 TX_Freq;		/* 发射频点 */
  U32 RX_Freq;		/* 接收频点 */
}stRDA1846Freq; 

typedef struct tagRDA1846SQ{
  U8 SQ_OnOff;
  U16 SQ_Open_Threhold;		/* 发射频点 */
  U16 SQ_Shut_Threhold;		/* 接收频点 */
}stRDA1846SQ;

#ifndef On
#define On 1
#endif

#ifndef Off
#define Off 0
#endif
/* Exported variables --------------------------------------------------------*/
extern stRDA1846Freq RDA1846Freq; 
extern stRDA1846SQ RDA1846SQ;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

extern void Init_RDA1846(void);

void Init_RDA1846_Rx(U32 Hz, U8 cs);
void Init_RDA1846_Tx(U32 Hz, U8 cs);
void Init_RDA1846_SQ(U16 SQ_Open_threshold, U16 SQ_Shut_threshold, U8 cs);

//读寄存器值函数
U16 Rd_RDA1846reg(U8 address, U8 cs);

void RDA1846_Sleep(U8 cs);
void RDA1846_Wakeup(U8 cs);
void RDA1846_Reset(U8 cs);

#endif
