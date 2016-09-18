/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: RxTx.h
  Author: lq       Version : V1.00        Date: 2014.05.06
  Description:     收发控制
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _RXTX_H
#define _RXTX_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq 收发控制标志 */
#define RXTX_SWITCH_TYPE_TX_TO_RX   0
#define RXTX_SWITCH_TYPE_RX_TO_TX   1

/*lq 端口引脚宏定义 */
#define RX_EN_PORT  GPIOC
#define RX_EN_PIN   GPIO_Pin_6

#define ANT_SW_PORT GPIOC
#define ANT_SW_PIN  GPIO_Pin_7

/*lq 控制功能物理定义 */
#define RX_EN_LOW()   GPIO_ResetBits(RX_EN_PORT, RX_EN_PIN)
#define RX_EN_HIGH()  GPIO_SetBits(RX_EN_PORT, RX_EN_PIN)

#define ANT_SW_LOW()  GPIO_ResetBits(ANT_SW_PORT, ANT_SW_PIN)
#define ANT_SW_HIGH() GPIO_SetBits(ANT_SW_PORT, ANT_SW_PIN)

/*lq 控制功能逻辑定义 */
#define RX_EN_OFF()                     RX_EN_LOW()
#define RX_EN_ON()                      RX_EN_HIGH()

#define ANT_SW_RX()                     ANT_SW_LOW()
#define ANT_SW_TX()                     ANT_SW_HIGH()

/* Private typedef -----------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/
extern void RxTxSwitch(U8 SwitchType);
extern void InitRxTx(void);
#endif
