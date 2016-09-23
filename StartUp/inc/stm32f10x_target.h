
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

U32 BSP_CPU_ClkFreq(void);




//lnw  add
extern void Target_SysClock_Init(void);
extern void Target_Periph_Init(void);



#endif /* __STM32F10x_TARGET_H */

