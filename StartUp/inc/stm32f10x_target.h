
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_TARGET_H
#define __STM32F10x_TARGET_H

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/
/* Public  function  ---------------------------------------------------------*/

void Target_Init(void);

void Tmr_TickInit(void);

void Wdg_Init(void);

void Delay_us(u32 nTime);

void delay_ms(u32 nTime);

u32 BSP_CPU_ClkFreq(void);

#endif /* __STM32F10x_TARGET_H */

