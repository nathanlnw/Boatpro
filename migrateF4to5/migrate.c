/*
     This  file is created  by  NathanLi   ,  it  services  for  migrating project from  Kei4  to  keil 5 
*/
#include "Include.h"
#include "migrate.h"


#define AIRCR_VECTKEY_MASK    ((uint32_t)0x05FA0000)


/*******************************************************************************
* Function Name  : NVIC_DeInit
* Description    : Deinitializes the NVIC peripheral registers to their default
*                  reset values.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_DeInit(void)
{
  u32 index = 0;
  
  NVIC->ICER[0] = 0xFFFFFFFF;
  NVIC->ICER[1] = 0x0FFFFFFF;
  NVIC->ICPR[0] = 0xFFFFFFFF;
  NVIC->ICPR[1] = 0x0FFFFFFF;
  
  for(index = 0; index < 240; index++)  // 0x0F old     240 new Library
  {
     NVIC->IP[index] = 0x00000000;
  } 
}

/*******************************************************************************
* Function Name  : NVIC_GenerateSystemReset
* Description    : Generates a system reset.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_GenerateSystemReset(void)
{
  SCB->AIRCR = AIRCR_VECTKEY_MASK | (u32)0x04;
}

