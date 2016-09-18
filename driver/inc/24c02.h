/****************************************************************************
*  Copyright (C), 2011, Institute Mod. Opt. NKU                             *
*  FileName: 24c02.h	 	                        	          	        *
*  Description:  24c02驱动控制												*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/12/02     1.0       build this moudle		            *
****************************************************************************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __24C02_H
#define __24C02_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* The M24C08W contains 4 blocks (128byte each) with the adresses below: E2 = 0 */
/* EEPROM Addresses defines */
#define EEPROM_Block0_ADDRESS 0xA0   /* E2 = 0 */
//#define EEPROM_Block1_ADDRESS 0xA0 /* E2 = 0 */
//#define EEPROM_Block2_ADDRESS 0xA4 /* E2 = 0 */
//#define EEPROM_Block3_ADDRESS 0xA6 /* E2 = 0 */

/* Private define ------------------------------------------------------------*/
#define EEPROM_WriteAddress0    0x00
#define EEPROM_ReadAddress0     0x00
#define BufferSize1             (countof(Tx1_Buffer)-1)
#define BufferSize2             (countof(Tx2_Buffer)-1)
#define EEPROM_WriteAddress1    (EEPROM_WriteAddress0 + BufferSize1)
#define EEPROM_ReadAddress1     (EEPROM_ReadAddress0 + BufferSize1)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))
/* Private define ------------------------------------------------------------*/
#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xA0
#define CR1_START_Set          ((USINT)0x0100)
#define CR1_STOP_Set           ((USINT)0x0200)
#define CR1_ACK_Reset          ((USINT)0xFBFF)
#define CR1_ACK_Set            ((USINT)0x0400)
#define CR1_PE_Set             ((USINT)0x0001)

#define PIN_OUT		(1)
#define PIN_IN		(0)

#define WP_H          GPIOB->BSRR = GPIO_Pin_5
#define WP_L          GPIOB->BRR  = GPIO_Pin_5   

#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 
   
#define SDA_H         GPIOB->BSRR = GPIO_Pin_7                                  //lq GPIOB的pin7输出‘1’，GPIOB的其他引脚输出值不变
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7                                  //lq GPIOB的pin7输出‘0’，GPIOB的其他引脚输出值不变

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7

#define I2C_PageSize  8  //24C02每页8字节
#define I2c_TEST_STA	1 //I2c测试

typedef struct EepromSelfCheckStruct
{
    U8  WPOkFlg;
    U8  DataCommOkFlg;
    U8  StartFlg;
    U8  CompleteFlg;
}EepromSelfCheckStruct;

/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void I2C_EE_Init(void);
extern U8 I2C_EE_BufferRead(char* pBuffer, U8 ReadAddress, U16 NumByteToWrite);
extern U8 I2C_EE_BufferWrite(char* pBuffer, U8 WriteAddress, U16 NumByteToWrite);
extern U8 EraseAllData(void);



extern void I2C_Test(void);

#endif /* __I2C_EE_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/


