/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SI446X.h
  Author: lq       Version : V1.00        Date: 2014.06.30
  Description:     SI446X驱动
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SI446X_H_
#define _SI446X_H_
#include "def.h"
#include "Si446xDefs.h"

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq ---------------------SI446X_A引脚定义------------------------------- */
/*lq SI446X_A SPI */
#define SI446X_A_SDI_PORT               GPIOA
#define SI446X_A_SDI_PIN                GPIO_Pin_7
#define SI446X_A_SDI_HIGH()             GPIO_SetBits(SI446X_A_SDI_PORT, SI446X_A_SDI_PIN)
#define SI446X_A_SDI_LOW()              GPIO_ResetBits(SI446X_A_SDI_PORT, SI446X_A_SDI_PIN)

#define SI446X_A_SDO_PORT               GPIOC
#define SI446X_A_SDO_PIN                GPIO_Pin_4
#define SI446X_A_SDO_IN()               GPIO_ReadInputDataBit(SI446X_A_SDO_PORT, SI446X_A_SDO_PIN)
#define SI446X_A_SDO_OUT()              GPIO_ReadOutputDataBit(SI446X_A_SDO_PORT, SI446X_A_SDO_PIN)

#define SI446X_A_NSEL_PORT              GPIOB
#define SI446X_A_NSEL_PIN               GPIO_Pin_13
#define SI446X_A_NSEL_HIGH()            GPIO_SetBits(SI446X_A_NSEL_PORT, SI446X_A_NSEL_PIN)
#define SI446X_A_NSEL_LOW()             GPIO_ResetBits(SI446X_A_NSEL_PORT, SI446X_A_NSEL_PIN)

#define SI446X_A_SCLK_PORT              GPIOC
#define SI446X_A_SCLK_PIN               GPIO_Pin_5
#define SI446X_A_SCLK_HIGH()            GPIO_SetBits(SI446X_A_SCLK_PORT, SI446X_A_SCLK_PIN)
#define SI446X_A_SCLK_LOW()             GPIO_ResetBits(SI446X_A_SCLK_PORT, SI446X_A_SCLK_PIN)
#define SI446X_A_SCLK_IN()              GPIO_ReadInputDataBit(SI446X_A_SCLK_PORT, SI446X_A_SCLK_PIN)
#define SI446X_A_SCLK_OUT()             GPIO_ReadOutputDataBit(SI446X_A_SCLK_PORT, SI446X_A_SCLK_PIN)
								 
/*lq SI446X_A中断 */
#define SI446X_A_NIRQ_PORT              PUBLIC_PORT
#define SI446X_A_NIRQ_PIN               PUBLIC_PIN
#define SI446X_A_NIRQ_HIGH()            GPIO_SetBits(SI446X_A_NIRQ_PORT, SI446X_A_NIRQ_PIN)
#define SI446X_A_NIRQ_LOW()             GPIO_ResetBits(SI446X_A_NIRQ_PORT, SI446X_A_NIRQ_PIN)
#define SI446X_A_NIRQ_IN()              GPIO_ReadInputDataBit(SI446X_A_NIRQ_PORT, SI446X_A_NIRQ_PIN)
#define SI446X_A_NIRQ_OUT()             GPIO_ReadOutputDataBit(SI446X_A_NIRQ_PORT, SI446X_A_NIRQ_PIN)

/*lq SI446X_A芯片使能 */
#define SI446X_A_SDN_PORT               GPIOB
#define SI446X_A_SDN_PIN                GPIO_Pin_12
#define SI446X_A_SDN_HIGH()             GPIO_SetBits(SI446X_A_SDN_PORT, SI446X_A_SDN_PIN)
#define SI446X_A_SDN_LOW()              GPIO_ResetBits(SI446X_A_SDN_PORT, SI446X_A_SDN_PIN)

/*lq SI446X_A TxRx */
#define SI446X_A_GPIO0_PORT             GPIOB
#define SI446X_A_GPIO0_PIN              GPIO_Pin_15
#define SI446X_A_GPIO0_HIGH()           GPIO_SetBits(SI446X_A_GPIO0_PORT, SI446X_A_GPIO0_PIN)
#define SI446X_A_GPIO0_LOW()            GPIO_ResetBits(SI446X_A_GPIO0_PORT, SI446X_A_GPIO0_PIN)
#define SI446X_A_GPIO0_IN()             GPIO_ReadInputDataBit(SI446X_A_GPIO0_PORT, SI446X_A_GPIO0_PIN)
#define SI446X_A_GPIO0_OUT()            GPIO_ReadOutputDataBit(SI446X_A_GPIO0_PORT, SI446X_A_GPIO0_PIN)

#define SI446X_A_GPIO1_PORT             GPIOB
#define SI446X_A_GPIO1_PIN              GPIO_Pin_14
#define SI446X_A_GPIO1_HIGH()           GPIO_SetBits(SI446X_A_GPIO1_PORT, SI446X_A_GPIO1_PIN)
#define SI446X_A_GPIO1_LOW()            GPIO_ResetBits(SI446X_A_GPIO1_PORT, SI446X_A_GPIO1_PIN)
#define SI446X_A_GPIO1_IN()             GPIO_ReadInputDataBit(SI446X_A_GPIO1_PORT, SI446X_A_GPIO1_PIN)
#define SI446X_A_GPIO1_OUT()            GPIO_ReadOutputDataBit(SI446X_A_GPIO1_PORT, SI446X_A_GPIO1_PIN)

/*lq SI446X_A 中断引脚定义 */
#define SI446X_A_GPIO0_SOURCE_PORT      GPIO_PortSourceGPIOB
#define SI446X_A_GPIO0_SOURCE_PIN       GPIO_PinSource15
#define SI446X_A_GPIO0_LINE             EXTI_Line15

/*lq SI446X引脚逻辑定义 */
#define SI446X_A_CE_PORT                SI446X_A_SDN_PORT
#define SI446X_A_CE_PIN                 SI446X_A_SDN_PIN
#define SI446X_A_CE_ON()              	SI446X_A_SDN_LOW()
#define SI446X_A_CE_OFF()           	SI446X_A_SDN_HIGH()

#define SI446X_A_CS_PORT                SI446X_A_NSEL_PORT
#define SI446X_A_CS_PIN                 SI446X_A_NSEL_PIN
#define SI446X_A_CS_ON()                SI446X_A_NSEL_LOW()
#define SI446X_A_CS_OFF()               SI446X_A_NSEL_HIGH()

#define SI446X_A_DATA_PORT              SI446X_A_GPIO1_PORT
#define SI446X_A_DATA_PIN               SI446X_A_GPIO1_PIN
#define SI446X_A_DATA_HIGH()            SI446X_A_GPIO1_HIGH()
#define SI446X_A_DATA_LOW()             SI446X_A_GPIO1_LOW()
#define SI446X_A_DATA_IN()              SI446X_A_GPIO1_IN()
#define SI446X_A_DATA_OUT()             SI446X_A_GPIO1_OUT()

#define SI446X_A_DCLK_PORT              SI446X_A_GPIO0_PORT
#define SI446X_A_DCLK_PIN               SI446X_A_GPIO0_PIN
#define SI446X_A_DCLK_HIGH()            SI446X_A_GPIO0_HIGH()
#define SI446X_A_DCLK_LOW()             SI446X_A_GPIO0_LOW()
#define SI446X_A_DCLK_IN()              SI446X_A_GPIO0_IN()
#define SI446X_A_DCLK_OUT()             SI446X_A_GPIO0_OUT()

#define SI446X_A_DCLK_SOURCE_PORT       SI446X_A_GPIO0_SOURCE_PORT
#define SI446X_A_DCLK_SOURCE_PIN        SI446X_A_GPIO0_SOURCE_PIN
#define SI446X_A_DCLK_LINE              SI446X_A_GPIO0_LINE

/*******************************************************************/
/*lq ---------------------SI446X_B引脚定义------------------------------- */
/*lq SI446X_B SPI */
#define SI446X_B_SDI_PORT               GPIOA
#define SI446X_B_SDI_PIN                GPIO_Pin_7
#define SI446X_B_SDI_HIGH()             GPIO_SetBits(SI446X_B_SDI_PORT, SI446X_B_SDI_PIN)
#define SI446X_B_SDI_LOW()              GPIO_ResetBits(SI446X_B_SDI_PORT, SI446X_B_SDI_PIN)

#define SI446X_B_SDO_PORT               GPIOC
#define SI446X_B_SDO_PIN                GPIO_Pin_4
#define SI446X_B_SDO_IN()               GPIO_ReadInputDataBit(SI446X_B_SDO_PORT, SI446X_B_SDO_PIN)
#define SI446X_B_SDO_OUT()              GPIO_ReadOutputDataBit(SI446X_B_SDO_PORT, SI446X_B_SDO_PIN)

#define SI446X_B_NSEL_PORT              GPIOA
#define SI446X_B_NSEL_PIN               GPIO_Pin_6
#define SI446X_B_NSEL_HIGH()            GPIO_SetBits(SI446X_B_NSEL_PORT, SI446X_B_NSEL_PIN)
#define SI446X_B_NSEL_LOW()             GPIO_ResetBits(SI446X_B_NSEL_PORT, SI446X_B_NSEL_PIN)

#define SI446X_B_SCLK_PORT              GPIOC
#define SI446X_B_SCLK_PIN               GPIO_Pin_5
#define SI446X_B_SCLK_HIGH()            GPIO_SetBits(SI446X_B_SCLK_PORT, SI446X_B_SCLK_PIN)
#define SI446X_B_SCLK_LOW()             GPIO_ResetBits(SI446X_B_SCLK_PORT, SI446X_B_SCLK_PIN)
#define SI446X_B_SCLK_IN()              GPIO_ReadInputDataBit(SI446X_B_SCLK_PORT, SI446X_B_SCLK_PIN)
#define SI446X_B_SCLK_OUT()             GPIO_ReadOutputDataBit(SI446X_B_SCLK_PORT, SI446X_B_SCLK_PIN)

/*lq SI446X_B中断 */
#define SI446X_B_NIRQ_PORT              PUBLIC_PORT
#define SI446X_B_NIRQ_PIN               PUBLIC_PIN
#define SI446X_B_NIRQ_HIGH()            GPIO_SetBits(SI446X_B_NIRQ_PORT, SI446X_B_NIRQ_PIN)
#define SI446X_B_NIRQ_LOW()             GPIO_ResetBits(SI446X_B_NIRQ_PORT, SI446X_B_NIRQ_PIN)
#define SI446X_B_NIRQ_IN()              GPIO_ReadInputDataBit(SI446X_B_NIRQ_PORT, SI446X_B_NIRQ_PIN)
#define SI446X_B_NIRQ_OUT()             GPIO_ReadOutputDataBit(SI446X_B_NIRQ_PORT, SI446X_B_NIRQ_PIN)

/*lq SI446X_B芯片使能 */
#define SI446X_B_SDN_PORT               GPIOB
#define SI446X_B_SDN_PIN                GPIO_Pin_1
#define SI446X_B_SDN_HIGH()             GPIO_SetBits(SI446X_B_SDN_PORT, SI446X_B_SDN_PIN)
#define SI446X_B_SDN_LOW()              GPIO_ResetBits(SI446X_B_SDN_PORT, SI446X_B_SDN_PIN)

/*lq SI446X_B TxRx */
#define SI446X_B_GPIO0_PORT             GPIOB
#define SI446X_B_GPIO0_PIN              GPIO_Pin_10
#define SI446X_B_GPIO0_HIGH()           GPIO_SetBits(SI446X_B_GPIO0_PORT, SI446X_B_GPIO0_PIN)
#define SI446X_B_GPIO0_LOW()            GPIO_ResetBits(SI446X_B_GPIO0_PORT, SI446X_B_GPIO0_PIN)
#define SI446X_B_GPIO0_IN()             GPIO_ReadInputDataBit(SI446X_B_GPIO0_PORT, SI446X_B_GPIO0_PIN)
#define SI446X_B_GPIO0_OUT()            GPIO_ReadOutputDataBit(SI446X_B_GPIO0_PORT, SI446X_B_GPIO0_PIN)

#define SI446X_B_GPIO1_PORT             GPIOB
#define SI446X_B_GPIO1_PIN              GPIO_Pin_0
#define SI446X_B_GPIO1_HIGH()           GPIO_SetBits(SI446X_B_GPIO1_PORT, SI446X_B_GPIO1_PIN)
#define SI446X_B_GPIO1_LOW()            GPIO_ResetBits(SI446X_B_GPIO1_PORT, SI446X_B_GPIO1_PIN)
#define SI446X_B_GPIO1_IN()             GPIO_ReadInputDataBit(SI446X_B_GPIO1_PORT, SI446X_B_GPIO1_PIN)
#define SI446X_B_GPIO1_OUT()            GPIO_ReadOutputDataBit(SI446X_B_GPIO1_PORT, SI446X_B_GPIO1_PIN)

/*lq SI446X_B 中断引脚定义 */
#define SI446X_B_GPIO0_SOURCE_PORT      GPIO_PortSourceGPIOB
#define SI446X_B_GPIO0_SOURCE_PIN       GPIO_PinSource10
#define SI446X_B_GPIO0_LINE             EXTI_Line10

/*lq SI446X引脚逻辑定义 */
#define SI446X_B_CE_PORT                SI446X_B_SDN_PORT
#define SI446X_B_CE_PIN                 SI446X_B_SDN_PIN
#define SI446X_B_CE_ON()              	SI446X_B_SDN_LOW()
#define SI446X_B_CE_OFF()           	SI446X_B_SDN_HIGH()

#define SI446X_B_CS_PORT                SI446X_B_NSEL_PORT
#define SI446X_B_CS_PIN                 SI446X_B_NSEL_PIN
#define SI446X_B_CS_ON()                SI446X_B_NSEL_LOW()
#define SI446X_B_CS_OFF()               SI446X_B_NSEL_HIGH()

#define SI446X_B_DATA_PORT              SI446X_B_GPIO1_PORT
#define SI446X_B_DATA_PIN               SI446X_B_GPIO1_PIN
#define SI446X_B_DATA_HIGH()            SI446X_B_GPIO1_HIGH()
#define SI446X_B_DATA_LOW()             SI446X_B_GPIO1_LOW()
#define SI446X_B_DATA_IN()              SI446X_B_GPIO1_IN()
#define SI446X_B_DATA_OUT()             SI446X_B_GPIO1_OUT()

#define SI446X_B_DCLK_PORT              SI446X_B_GPIO0_PORT
#define SI446X_B_DCLK_PIN               SI446X_B_GPIO0_PIN
#define SI446X_B_DCLK_HIGH()            SI446X_B_GPIO0_HIGH()
#define SI446X_B_DCLK_LOW()             SI446X_B_GPIO0_LOW()
#define SI446X_B_DCLK_IN()              SI446X_B_GPIO0_IN()
#define SI446X_B_DCLK_OUT()             SI446X_B_GPIO0_OUT()

#define SI446X_B_DCLK_SOURCE_PORT       SI446X_B_GPIO0_SOURCE_PORT
#define SI446X_B_DCLK_SOURCE_PIN        SI446X_B_GPIO0_SOURCE_PIN
#define SI446X_B_DCLK_LINE              SI446X_B_GPIO0_LINE

#if 0
/*lq SI446X 寄存器索引定义 */
#define SI446X_REG_INDEX_R0_N           0
#define SI446X_REG_TOTAL                16

/*lq SI446X 寄存器地址定义 */
#define SI446X_REG_ADDR_REG0            0x00000000u

/*lq SI446X 寄存器字段掩码定义 */
#define SI446X_FEILD_MASK_REG_ADDR      0x0000000Fu

/*lq SI446X 寄存器字段值定义 */
#define SI446X_FEILD_VALUE_R0_FRACTIONAL_N_A    15102   //lq 161.975MHz

/*lq Readback Register 配置 */
#define SI446X_REG_VALUE_REG7_READBACK_RSSI     0x00000147
#endif

/*lq SI446X_A和SI446X_B的LDO控制 */
#define SI446X_POWER_PORT               GPIOA
#define SI446X_POWER_PIN                GPIO_Pin_12
#define SI446X_POWER_HIGH()             GPIO_SetBits(SI446X_POWER_PORT, SI446X_POWER_PIN)
#define SI446X_POWER_LOW()              GPIO_ResetBits(SI446X_POWER_PORT, SI446X_POWER_PIN)

/*lq 逻辑定义 */
#define SI446X_POWER_ON()               SI446X_POWER_HIGH()
#define SI446X_POWER_OFF()              SI446X_POWER_LOW()

/*============================================================================*/
/*lq Output Divider (Outdiv) Values */
#define SI446X_FREQ_OUTDIV_4460_61_63_24_LOWER  142000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_24_UPPER  175000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_12_LOWER  284000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_12_UPPER  350000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_8_LOWER   420000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_8_UPPER   525000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_4_LOWER   850000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_4_UPPER   1050000000  //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4460_61_63_DEFAULT   24

#define SI446X_FREQ_OUTDIV_4464_24_LOWER    119000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_24_UPPER    159000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_16_LOWER    177000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_16_UPPER    239000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_12_LOWER    235000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_12_UPPER    319000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_8_LOWER     353000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_8_UPPER     479000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_6_LOWER     470000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_6_UPPER     639000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_4_LOWER     705000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_4_UPPER     960000000   //lq unit: Hz
#define SI446X_FREQ_OUTDIV_4464_DEFAULT     24

#define SI446X_FREQ_BAND_142_175        0   //lq 161.975MHz
#define SI446X_FREQ_BAND_420_525        1   //lq 433MHz
#define SI446X_FREQ_BAND                SI446X_FREQ_BAND_142_175

/*lq frequence of crystal oscillate */
#define SI446X_FREQ_XO                  26000000    //lq unit: Hz
#define SI446X_FREQ_DEVIATION           2400        //lq unit: Hz

#if (SI446X_FREQ_BAND == SI446X_FREQ_BAND_142_175)
#if 0
#define SI446X_FREQ_OFFSET              3600        //lq unit: Hz，161.975MHz
#else
#define SI446X_FREQ_OFFSET_A            -300        //lq unit: 1，SI446X_A，SI446X_FREQ_OFFSET值与频率值的对应关系约为1比1（经验值）
#define SI446X_FREQ_OFFSET_B            -300        //lq unit: 1，SI446X_B
#endif
#elif (SI446X_FREQ_BAND == SI446X_FREQ_BAND_420_525)            
#define SI446X_FREQ_OFFSET              9800        //lq unit: Hz，433MHz
#endif

#define SI446X_FREQ_RF_AIS_A            161975000   //lq unit: Hz
#define SI446X_FREQ_RF_AIS_B            162025000   //lq unit: Hz

//#if (SI446X_FREQ_BAND == SI446X_FREQ_BAND_142_175)
//#define SI446X_FREQ_RF_BASE             SI446X_FREQ_RF_AIS_A
//#elif (SI446X_FREQ_BAND == SI446X_FREQ_BAND_420_525)            
//#define SI446X_FREQ_RF_BASE             433000000   //lq unit: Hz
//#endif

#define SI446X_FREQ_CHANNEL_STEPSIZE    25000       //lq unit: Hz

/*lq 中频模式 */
#define SI446X_IF_MODE_ZERO             0
#define SI446X_IF_MODE_SCALED           1
#define SI446X_IF_MODE_FIXED            2
#define SI446X_IF_MODE                  SI446X_IF_MODE_FIXED

/*lq 中频计算公式中N值的指数 */
#define SI446X_IF_FREQ_N_EXP_FIXED      0           //lq i.e. N = 2^0 = 1
#define SI446X_IF_FREQ_N_EXP_SCALED     2           //lq i.e. N = 2^2
#define SI446X_IF_FREQ_N_EXP_ZERO       0           //lq i.e. N = 2^0 = 1，ZERO-IF模式下该值不起作用
#if (SI446X_IF_MODE == SI446X_IF_MODE_FIXED)
#define SI446X_IF_FREQ_N_EXP            SI446X_IF_FREQ_N_EXP_FIXED
#elif (SI446X_IF_MODE == SI446X_IF_MODE_SCALED)
#define SI446X_IF_FREQ_N_EXP            SI446X_IF_FREQ_N_EXP_SCALED
#else
#define SI446X_IF_FREQ_N_EXP            SI446X_IF_FREQ_N_EXP_ZERO
#endif

#define SI446X_MODEM_DATA_RATE          9600        //lq unit: bps
#define SI446X_CHANNEL_AIS_A            0
#define SI446X_CHANNEL_AIS_B            2

#define SI446X_RSSI_CAL                 130         //lq unit: 1

#define SI446X_PA_PWR_LVL               0x0A        //lq unit: 1，5dBm

/*lq SI446X工作模式 */
#define SI446X_MODE_TX                  1
#define SI446X_MODE_RX                  2
#define SI446X_MODE_IDLE                3

/*lq SI446X工作状态 */
#define SI446X_STATE_NO_CHANGE          0
#define SI446X_STATE_SLEEP              1
#define SI446X_STATE_SPI_ACTIVE         2
#define SI446X_STATE_READY              3
#define SI446X_STATE_READY2             4   //lq Another enumeration for Ready state
#define SI446X_STATE_TX_TUNE            5
#define SI446X_STATE_RX_TUNE            6
#define SI446X_STATE_TX                 7
#define SI446X_STATE_RX                 8

/*lq SI446X型号 */
#define SI446X_TYPE_ID_4460             0x4460
#define SI446X_TYPE_ID_4461             0x4461
#define SI446X_TYPE_ID_4463             0x4463
#define SI446X_TYPE_ID_4464             0x4464
#define SI446X_TYPE_ID_4467             0x4467
#define SI446X_TYPE_ID_4468             0x4468

/* Private typedef -----------------------------------------------------------*/
typedef enum SI446XChip
{
   SI446X_CHIP_A,
   SI446X_CHIP_B
}SI446XChipEnum;

typedef enum SI446XType
{
   SI446X_TYPE_4460,
   SI446X_TYPE_4461,
   SI446X_TYPE_4463,
   SI446X_TYPE_4464,
   SI446X_TYPE_4467,
   SI446X_TYPE_4468,
   SI446X_TYPE_OTHER
}SI446XTypeEnum;

//-----------------------SI446X寄存器配置-------------------------------
typedef struct 
{
    U8 SI446XStatus;

    U32 SI446X_N_Integer_Base;
    U32 SI446X_N_Fractional_Base;
    U32 SI446X_Freq_Dev;
    S16 SI446X_Freq_Offset;
    U32 SI446X_IF_Freq;
    U16 SI446X_Channel_Step;

    U8  SI446X_PaPwrLvl;
}SI446XStruct;

typedef struct Si446xSelfCheckStruct
{
    U8  PdnOkFlg;
    U8  SenOkFlg;
    U8  DataCommOkFlg;   //lq clk and SDI/SDO
    U8  TypeOkFlg;
    SI446XTypeEnum Si446xType;        
    U8  StartFlg;
    U8  CompleteFlg;
}Si446xSelfCheckStruct;

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern U32 SI446X_CalcRfFreqInteger(U32 freq, SI446XTypeEnum si446xType);
extern U32 SI446X_CalcRfFreqFractional(U32 freq, SI446XTypeEnum si446xType);
extern void SI446X_RX_FIFO_RESET(SI446XChipEnum cs);
extern void SI446X_TX_FIFO_RESET(SI446XChipEnum cs);
extern void SI446X_GPIO_PIN_CFG(U8 G0, U8 G1, U8 G2, U8 G3, U8 IRQ, U8 sdo,
                                U8 GEN_CONFIG, SI446XChipEnum cs);
extern void SI446X_ReadResponse(U8 *buffer, U8 size, SI446XChipEnum cs);
extern void SI446X_WaitCts(SI446XChipEnum cs);
extern void SI446X_WRITE_TX_FIFO(U8 *txbuffer, U8 size, SI446XChipEnum cs);
extern void SI446X_ConfigInit(SI446XChipEnum cs);
extern void SI446X_START_TX(U8 channel, U8 condition, U16 tx_len, SI446XChipEnum cs);
extern void SI446X_START_RX(U8 channel, U8 condition, U16 rx_len, U8 n_state1,
                            U8 n_state2, U8 n_state3, SI446XChipEnum cs);
extern void SI446X_SET_PROPERTY_1(SI446XPropertyEnum GROUP_NUM, U8 para,
                                  SI446XChipEnum cs);
extern void SI446X_GET_MODEM_STATUS(U8 *buffer, SI446XChipEnum cs);
extern void SI446X_GpioPinCfg(U8 txRxMode, SI446XChipEnum cs);
extern void SI446X_Reset(SI446XChipEnum cs);
extern U32 SI446X_CalcRfDeviation(U32 freq, U32 freqDev, SI446XTypeEnum si446xType);
extern U32 SI446X_CalcIFFreq(U32 freq, SI446XTypeEnum si446xType);
extern void SI446X_CHANGE_STATE(U8 newState, SI446XChipEnum cs);
extern S16 SI446X_CalcRfOffset(U32 freq, S32 freqOffset, SI446XTypeEnum si446xType);
extern U16 SI446X_CalcRfChannelStep(U32 freq, U32 channelStep,
                                    SI446XTypeEnum si446xType);
extern void SI446X_SelfCheck(Si446xSelfCheckStruct *pSi446xSelfCheckStruct,
                      SI446XChipEnum cs);
void SI446X_SETFREBASE(U32 ulFreBase);


extern void SI446X_ModuleInit(void);// lnw add 
extern void SI446X_A_TxRxClockIRQS(void);//lnw  add 


#if  CONFIG_PTT_EN    // lnw
extern U8 SI446X_RXSTTUS_Return(void);
#endif



#endif
/*==============================================================================
------------------------------------End of File---------------------------------
==============================================================================*/
