
/*
     This  file  created  by  NathanLi   ,  it  services  for  migrating project from  Kei4  to  keil 5 
*/
#ifndef  _MGRT
#define  _MGRT


/* Exported constants --------------------------------------------------------*/
/* IRQ Channels --------------------------------------------------------------*/
#define WWDG_IRQChannel              ((u8)0x00)  /* Window WatchDog Interrupt */
#define PVD_IRQChannel               ((u8)0x01)  /* PVD through EXTI Line detection Interrupt */
#define TAMPER_IRQChannel            ((u8)0x02)  /* Tamper Interrupt */
#define RTC_IRQChannel               ((u8)0x03)  /* RTC global Interrupt */
#define FLASH_IRQChannel             ((u8)0x04)  /* FLASH global Interrupt */
#define RCC_IRQChannel               ((u8)0x05)  /* RCC global Interrupt */
#define EXTI0_IRQChannel             ((u8)0x06)  /* EXTI Line0 Interrupt */
#define EXTI1_IRQChannel             ((u8)0x07)  /* EXTI Line1 Interrupt */
#define EXTI2_IRQChannel             ((u8)0x08)  /* EXTI Line2 Interrupt */
#define EXTI3_IRQChannel             ((u8)0x09)  /* EXTI Line3 Interrupt */
#define EXTI4_IRQChannel             ((u8)0x0A)  /* EXTI Line4 Interrupt */
#define DMA1_Channel1_IRQChannel     ((u8)0x0B)  /* DMA1 Channel 1 global Interrupt */
#define DMA1_Channel2_IRQChannel     ((u8)0x0C)  /* DMA1 Channel 2 global Interrupt */
#define DMA1_Channel3_IRQChannel     ((u8)0x0D)  /* DMA1 Channel 3 global Interrupt */
#define DMA1_Channel4_IRQChannel     ((u8)0x0E)  /* DMA1 Channel 4 global Interrupt */
#define DMA1_Channel5_IRQChannel     ((u8)0x0F)  /* DMA1 Channel 5 global Interrupt */
#define DMA1_Channel6_IRQChannel     ((u8)0x10)  /* DMA1 Channel 6 global Interrupt */
#define DMA1_Channel7_IRQChannel     ((u8)0x11)  /* DMA1 Channel 7 global Interrupt */
#define ADC1_2_IRQChannel            ((u8)0x12)  /* ADC1 et ADC2 global Interrupt */
#define USB_HP_CAN_TX_IRQChannel     ((u8)0x13)  /* USB High Priority or CAN TX Interrupts */
#define USB_LP_CAN_RX0_IRQChannel    ((u8)0x14)  /* USB Low Priority or CAN RX0 Interrupts */
#define CAN_RX1_IRQChannel           ((u8)0x15)  /* CAN RX1 Interrupt */
#define CAN_SCE_IRQChannel           ((u8)0x16)  /* CAN SCE Interrupt */
#define EXTI9_5_IRQChannel           ((u8)0x17)  /* External Line[9:5] Interrupts */
#define TIM1_BRK_IRQChannel          ((u8)0x18)  /* TIM1 Break Interrupt */
#define TIM1_UP_IRQChannel           ((u8)0x19)  /* TIM1 Update Interrupt */
#define TIM1_TRG_COM_IRQChannel      ((u8)0x1A)  /* TIM1 Trigger and Commutation Interrupt */
#define TIM1_CC_IRQChannel           ((u8)0x1B)  /* TIM1 Capture Compare Interrupt */
#define TIM2_IRQChannel              ((u8)0x1C)  /* TIM2 global Interrupt */
#define TIM3_IRQChannel              ((u8)0x1D)  /* TIM3 global Interrupt */
#define TIM4_IRQChannel              ((u8)0x1E)  /* TIM4 global Interrupt */
#define I2C1_EV_IRQChannel           ((u8)0x1F)  /* I2C1 Event Interrupt */
#define I2C1_ER_IRQChannel           ((u8)0x20)  /* I2C1 Error Interrupt */
#define I2C2_EV_IRQChannel           ((u8)0x21)  /* I2C2 Event Interrupt */
#define I2C2_ER_IRQChannel           ((u8)0x22)  /* I2C2 Error Interrupt */
#define SPI1_IRQChannel              ((u8)0x23)  /* SPI1 global Interrupt */
#define SPI2_IRQChannel              ((u8)0x24)  /* SPI2 global Interrupt */
#define USART1_IRQChannel            ((u8)0x25)  /* USART1 global Interrupt */
#define USART2_IRQChannel            ((u8)0x26)  /* USART2 global Interrupt */
#define USART3_IRQChannel            ((u8)0x27)  /* USART3 global Interrupt */
#define EXTI15_10_IRQChannel         ((u8)0x28)  /* External Line[15:10] Interrupts */
#define RTCAlarm_IRQChannel          ((u8)0x29)  /* RTC Alarm through EXTI Line Interrupt */
#define USBWakeUp_IRQChannel         ((u8)0x2A)  /* USB WakeUp from suspend through EXTI Line Interrupt */
#define TIM8_BRK_IRQChannel          ((u8)0x2B)  /* TIM8 Break Interrupt */
#define TIM8_UP_IRQChannel           ((u8)0x2C)  /* TIM8 Update Interrupt */
#define TIM8_TRG_COM_IRQChannel      ((u8)0x2D)  /* TIM8 Trigger and Commutation Interrupt */
#define TIM8_CC_IRQChannel           ((u8)0x2E)  /* TIM8 Capture Compare Interrupt */
#define ADC3_IRQChannel              ((u8)0x2F)  /* ADC3 global Interrupt */
#define FSMC_IRQChannel              ((u8)0x30)  /* FSMC global Interrupt */
#define SDIO_IRQChannel              ((u8)0x31)  /* SDIO global Interrupt */
#define TIM5_IRQChannel              ((u8)0x32)  /* TIM5 global Interrupt */
#define SPI3_IRQChannel              ((u8)0x33)  /* SPI3 global Interrupt */
#define UART4_IRQChannel             ((u8)0x34)  /* UART4 global Interrupt */
#define UART5_IRQChannel             ((u8)0x35)  /* UART5 global Interrupt */
#define TIM6_IRQChannel              ((u8)0x36)  /* TIM6 global Interrupt */
#define TIM7_IRQChannel              ((u8)0x37)  /* TIM7 global Interrupt */
#define DMA2_Channel1_IRQChannel     ((u8)0x38)  /* DMA2 Channel 1 global Interrupt */
#define DMA2_Channel2_IRQChannel     ((u8)0x39)  /* DMA2 Channel 2 global Interrupt */
#define DMA2_Channel3_IRQChannel     ((u8)0x3A)  /* DMA2 Channel 3 global Interrupt */
#define DMA2_Channel4_5_IRQChannel   ((u8)0x3B)  /* DMA2 Channel 4 and DMA2 Channel 5 global Interrupt */



//-----------  RCC   migrant  needing   below
#define RCC_APB2Periph_ALL               ((u32)0x0000FFFD)
#define RCC_APB1Periph_ALL               ((u32)0x3AFEC83F)







// Added   Function  migrate  from  old  library files
extern void NVIC_DeInit(void);
extern void NVIC_GenerateSystemReset(void);


#endif
