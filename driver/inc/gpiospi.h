/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: gpiospi.h
  Author: lq       Version : V1.00        Date: 2014.07.01
  Description:     通过GPIO模拟spi
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _GPIOSPI_H
#define _GPIOSPI_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq 实例编号 */
#define GPART_INSTANCE_NUM_1	    1

#define GPIOSPI1_CS_PORT            PUBLIC_PORT
#define GPIOSPI1_CS_PIN             PUBLIC_PIN
#define GPIOSPI1_CS_HIGH()          GPIO_SetBits(GPIOSPI1_CS_PORT, GPIOSPI1_CS_PIN)
#define GPIOSPI1_CS_LOW()           GPIO_ResetBits(GPIOSPI1_CS_PORT, GPIOSPI1_CS_PIN)

#define GPIOSPI1_SCLK_PORT          PUBLIC_PORT
#define GPIOSPI1_SCLK_PIN           PUBLIC_PIN
#define GPIOSPI1_SCLK_HIGH()        GPIO_SetBits(GPIOSPI1_SCLK_PORT, GPIOSPI1_SCLK_PIN)
#define GPIOSPI1_SCLK_LOW()         GPIO_ResetBits(GPIOSPI1_SCLK_PORT, GPIOSPI1_SCLK_PIN)

#define GPIOSPI1_MOSI_PORT          PUBLIC_PORT
#define GPIOSPI1_MOSI_PIN           PUBLIC_PIN
#define GPIOSPI1_MOSI_HIGH()        GPIO_SetBits(GPIOSPI1_MOSI_PORT, GPIOSPI1_MOSI_PIN)
#define GPIOSPI1_MOSI_LOW()         GPIO_ResetBits(GPIOSPI1_MOSI_PORT, GPIOSPI1_MOSI_PIN)

#define GPIOSPI1_MISO_PORT          GPIOB
#define GPIOSPI1_MISO_PIN           GPIO_Pin_11
#define GPIOSPI1_MISO_HIGH()        GPIO_SetBits(GPIOSPI1_MISO_PORT, GPIOSPI1_MISO_PIN)
#define GPIOSPI1_MISO_LOW()         GPIO_ResetBits(GPIOSPI1_MISO_PORT, GPIOSPI1_MISO_PIN)
#define GPIOSPI1_MISO_IN_READ()     GPIO_ReadInputDataBit(GPIOSPI1_MISO_PORT, GPIOSPI1_MISO_PIN)
#define GPIOSPI1_MISO_OUT_READ()    GPIO_ReadOutputDataBit(GPIOSPI1_MISO_PORT, GPIOSPI1_MISO_PIN)

/*lq 方案2 */
//SPI端口定义
#define SEN     PBout(1)	
#define SCLK    PBout(11)
#define SDI     PBout(10)
#define SDO     PBin(10)

/* Private typedef -----------------------------------------------------------*/
typedef enum GpioSpiType
{
    GPIOSPI1,
}GpioSpiTypeEnum;

typedef enum GpioSpiCPOL
{
    GPIOSPI_CPOL_Low,
    GPIOSPI_CPOL_High,
}GpioSpiCPOLEnum;

typedef enum GpioSpiCPHA
{
    GPIOSPI_CPHA_1Edge,
    GPIOSPI_CPHA_2Edge,
}GpioSpiCPHAEnum;

typedef enum GpioSpiFirstBit
{
    GPIOSPI_FirstBit_MSB,
    GPIOSPI_FirstBit_LSB,
}GpioSpiFirstBitEnum;

typedef enum GpioSpiDataSize
{
    GPIOSPI_DataSize_8b,
    GPIOSPI_DataSize_16b,
}GpioSpiDataSizeEnum;

/*lq GpioSpi 结构体*/
typedef struct GpioSpi
{
    U8  InstanceNum;            //lq 实例编号
    U8  SPI_CPOL;
    U8  SPI_CPHA;
    U16 SPI_BaudRatePrescaler;
    U8  SPI_FirstBit;
    U8  SPI_DataSize; 
    U8  SPI_ClockCounter;       
}GpioSpiStruct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/

/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/

#endif
