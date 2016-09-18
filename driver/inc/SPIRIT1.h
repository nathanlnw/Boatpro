/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: SPIRIT1.h
  Author: lq       Version : V1.00        Date: 2014.06.30
  Description:     SPIRIT1驱动
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _SPIRIT1_H
#define _SPIRIT1_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq ---------------------SPIRIT1_A引脚定义------------------------------- */
/*lq SPIRIT1_A SDN */
#define SPIRIT1_A_SDN_PORT        GPIOC
#define SPIRIT1_A_SDN_PIN         GPIO_Pin_10
#define SPIRIT1_A_SDN_HIGH()      GPIO_SetBits(SPIRIT1_A_SDN_PORT, SPIRIT1_A_SDN_PIN)
#define SPIRIT1_A_SDN_LOW()       GPIO_ResetBits(SPIRIT1_A_SDN_PORT, SPIRIT1_A_SDN_PIN)

/*lq SPIRIT1_A GPIO */
#define SPIRIT1_A_GPIO0_PORT        GPIOC
#define SPIRIT1_A_GPIO0_PIN         GPIO_Pin_6
#define SPIRIT1_A_GPIO0_HIGH()      GPIO_SetBits(SPIRIT1_A_GPIO0_PORT, SPIRIT1_A_GPIO0_PIN)
#define SPIRIT1_A_GPIO0_LOW()       GPIO_ResetBits(SPIRIT1_A_GPIO0_PORT, SPIRIT1_A_GPIO0_PIN)
#define SPIRIT1_A_GPIO0_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_A_GPIO0_PORT, SPIRIT1_A_GPIO0_PIN)
#define SPIRIT1_A_GPIO0_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_A_GPIO0_PORT, SPIRIT1_A_GPIO0_PIN)

#define SPIRIT1_A_GPIO1_PORT        GPIOC
#define SPIRIT1_A_GPIO1_PIN         GPIO_Pin_7
#define SPIRIT1_A_GPIO1_HIGH()      GPIO_SetBits(SPIRIT1_A_GPIO1_PORT, SPIRIT1_A_GPIO1_PIN)
#define SPIRIT1_A_GPIO1_LOW()       GPIO_ResetBits(SPIRIT1_A_GPIO1_PORT, SPIRIT1_A_GPIO1_PIN)
#define SPIRIT1_A_GPIO1_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_A_GPIO1_PORT, SPIRIT1_A_GPIO1_PIN)
#define SPIRIT1_A_GPIO1_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_A_GPIO1_PORT, SPIRIT1_A_GPIO1_PIN)

#define SPIRIT1_A_GPIO2_PORT        GPIOC
#define SPIRIT1_A_GPIO2_PIN         GPIO_Pin_8
#define SPIRIT1_A_GPIO2_HIGH()      GPIO_SetBits(SPIRIT1_A_GPIO2_PORT, SPIRIT1_A_GPIO2_PIN)
#define SPIRIT1_A_GPIO2_LOW()       GPIO_ResetBits(SPIRIT1_A_GPIO2_PORT, SPIRIT1_A_GPIO2_PIN)
#define SPIRIT1_A_GPIO2_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_A_GPIO2_PORT, SPIRIT1_A_GPIO2_PIN)
#define SPIRIT1_A_GPIO2_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_A_GPIO2_PORT, SPIRIT1_A_GPIO2_PIN)

#define SPIRIT1_A_GPIO3_PORT        GPIOC
#define SPIRIT1_A_GPIO3_PIN         GPIO_Pin_9
#define SPIRIT1_A_GPIO3_HIGH()      GPIO_SetBits(SPIRIT1_A_GPIO3_PORT, SPIRIT1_A_GPIO3_PIN)
#define SPIRIT1_A_GPIO3_LOW()       GPIO_ResetBits(SPIRIT1_A_GPIO3_PORT, SPIRIT1_A_GPIO3_PIN)
#define SPIRIT1_A_GPIO3_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_A_GPIO3_PORT, SPIRIT1_A_GPIO3_PIN)
#define SPIRIT1_A_GPIO3_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_A_GPIO3_PORT, SPIRIT1_A_GPIO3_PIN)
#if 0
/*lq SPIRIT1_A SPI */
#define SPIRIT1_A_SPI_CS_PORT       GPIOB
#define SPIRIT1_A_SPI_CS_PIN        GPIO_Pin_12
#define SPIRIT1_A_SPI_CS_HIGH()     GPIO_SetBits(SPIRIT1_A_SPI_CS_PORT, SPIRIT1_A_SPI_CS_PIN)
#define SPIRIT1_A_SPI_CS_LOW()      GPIO_ResetBits(SPIRIT1_A_SPI_CS_PORT, SPIRIT1_A_SPI_CS_PIN)

#define SPIRIT1_A_SPI_SCLK_PORT     GPIOB
#define SPIRIT1_A_SPI_SCLK_PIN      GPIO_Pin_13
#define SPIRIT1_A_SPI_SCLK_HIGH()   GPIO_SetBits(SPIRIT1_A_SPI_SCLK_PORT, SPIRIT1_A_SPI_SCLK_PIN)
#define SPIRIT1_A_SPI_SCLK_LOW()    GPIO_ResetBits(SPIRIT1_A_SPI_SCLK_PORT, SPIRIT1_A_SPI_SCLK_PIN)

#define SPIRIT1_A_SPI_MOSI_PORT     GPIOB
#define SPIRIT1_A_SPI_MOSI_PIN      GPIO_Pin_15
#define SPIRIT1_A_SPI_MOSI_HIGH()   GPIO_SetBits(SPIRIT1_A_SPI_MOSI_PORT, SPIRIT1_A_SPI_MOSI_PIN)
#define SPIRIT1_A_SPI_MOSI_LOW()    GPIO_ResetBits(SPIRIT1_A_SPI_MOSI_PORT, SPIRIT1_A_SPI_MOSI_PIN)

#define SPIRIT1_A_SPI_MISO_PORT     GPIOB
#define SPIRIT1_A_SPI_MISO_PIN      GPIO_Pin_14
#define SPIRIT1_A_SPI_MISO_HIGH()   GPIO_SetBits(SPIRIT1_A_SPI_MISO_PORT, SPIRIT1_A_SPI_MISO_PIN)
#define SPIRIT1_A_SPI_MISO_LOW()    GPIO_ResetBits(SPIRIT1_A_SPI_MISO_PORT, SPIRIT1_A_SPI_MISO_PIN)
#endif

/*lq SPIRIT1_A 中断引脚定义 */
#define SPIRIT1_A_GPIO0_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_A_GPIO0_SOURCE_PIN      GPIO_PinSource6
#define SPIRIT1_A_GPIO0_LINE        	EXTI_Line6

#define SPIRIT1_A_GPIO1_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_A_GPIO1_SOURCE_PIN      GPIO_PinSource7
#define SPIRIT1_A_GPIO1_LINE        	EXTI_Line7

#define SPIRIT1_A_GPIO2_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_A_GPIO2_SOURCE_PIN      GPIO_PinSource8
#define SPIRIT1_A_GPIO2_LINE        	EXTI_Line8

#define SPIRIT1_A_GPIO3_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_A_GPIO3_SOURCE_PIN      GPIO_PinSource9
#define SPIRIT1_A_GPIO3_LINE        	EXTI_Line9

/*lq ---------------------SPIRIT1_B引脚定义------------------------------- */
/*lq SPIRIT1_B SDN */
#define SPIRIT1_B_SDN_PORT        GPIOD
#define SPIRIT1_B_SDN_PIN         GPIO_Pin_2
#define SPIRIT1_B_SDN_HIGH()      GPIO_SetBits(SPIRIT1_B_SDN_PORT, SPIRIT1_B_SDN_PIN)
#define SPIRIT1_B_SDN_LOW()       GPIO_ResetBits(SPIRIT1_B_SDN_PORT, SPIRIT1_B_SDN_PIN)

/*lq SPIRIT1_B GPIO */
#define SPIRIT1_B_GPIO0_PORT        GPIOA
#if 0
#define SPIRIT1_B_GPIO0_PIN         GPIO_Pin_6
#else
#define SPIRIT1_B_GPIO0_PIN         GPIO_Pin_7
#endif
#define SPIRIT1_B_GPIO0_HIGH()      GPIO_SetBits(SPIRIT1_B_GPIO0_PORT, SPIRIT1_B_GPIO0_PIN)
#define SPIRIT1_B_GPIO0_LOW()       GPIO_ResetBits(SPIRIT1_B_GPIO0_PORT, SPIRIT1_B_GPIO0_PIN)
#define SPIRIT1_B_GPIO0_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_B_GPIO0_PORT, SPIRIT1_B_GPIO0_PIN)
#define SPIRIT1_B_GPIO0_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_B_GPIO0_PORT, SPIRIT1_B_GPIO0_PIN)

#define SPIRIT1_B_GPIO1_PORT        GPIOA
#define SPIRIT1_B_GPIO1_PIN         GPIO_Pin_7
#define SPIRIT1_B_GPIO1_HIGH()      GPIO_SetBits(SPIRIT1_B_GPIO1_PORT, SPIRIT1_B_GPIO1_PIN)
#define SPIRIT1_B_GPIO1_LOW()       GPIO_ResetBits(SPIRIT1_B_GPIO1_PORT, SPIRIT1_B_GPIO1_PIN)
#define SPIRIT1_B_GPIO1_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_B_GPIO1_PORT, SPIRIT1_B_GPIO1_PIN)
#define SPIRIT1_B_GPIO1_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_B_GPIO1_PORT, SPIRIT1_B_GPIO1_PIN)

#define SPIRIT1_B_GPIO2_PORT        GPIOC
#define SPIRIT1_B_GPIO2_PIN         GPIO_Pin_4
#define SPIRIT1_B_GPIO2_HIGH()      GPIO_SetBits(SPIRIT1_B_GPIO2_PORT, SPIRIT1_B_GPIO2_PIN)
#define SPIRIT1_B_GPIO2_LOW()       GPIO_ResetBits(SPIRIT1_B_GPIO2_PORT, SPIRIT1_B_GPIO2_PIN)
#define SPIRIT1_B_GPIO2_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_B_GPIO2_PORT, SPIRIT1_B_GPIO2_PIN)
#define SPIRIT1_B_GPIO2_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_B_GPIO2_PORT, SPIRIT1_B_GPIO2_PIN)

#define SPIRIT1_B_GPIO3_PORT        GPIOC
#define SPIRIT1_B_GPIO3_PIN         GPIO_Pin_5
#define SPIRIT1_B_GPIO3_HIGH()      GPIO_SetBits(SPIRIT1_B_GPIO3_PORT, SPIRIT1_B_GPIO3_PIN)
#define SPIRIT1_B_GPIO3_LOW()       GPIO_ResetBits(SPIRIT1_B_GPIO3_PORT, SPIRIT1_B_GPIO3_PIN)
#define SPIRIT1_B_GPIO3_IN_READ()   GPIO_ReadInputDataBit(SPIRIT1_B_GPIO3_PORT, SPIRIT1_B_GPIO3_PIN)
#define SPIRIT1_B_GPIO3_OUT_READ()  GPIO_ReadOutputDataBit(SPIRIT1_B_GPIO3_PORT, SPIRIT1_B_GPIO3_PIN)
#if 0
/*lq SPIRIT1_B SPI */
#define SPIRIT1_B_SPI_CS_PORT       GPIOB
#define SPIRIT1_B_SPI_CS_PIN        GPIO_Pin_0
#define SPIRIT1_B_SPI_CS_HIGH()     GPIO_SetBits(SPIRIT1_B_SPI_CS_PORT, SPIRIT1_B_SPI_CS_PIN)
#define SPIRIT1_B_SPI_CS_LOW()      GPIO_ResetBits(SPIRIT1_B_SPI_CS_PORT, SPIRIT1_B_SPI_CS_PIN)

#define SPIRIT1_B_SPI_SCLK_PORT     GPIOB
#define SPIRIT1_B_SPI_SCLK_PIN      GPIO_Pin_1
#define SPIRIT1_B_SPI_SCLK_HIGH()   GPIO_SetBits(SPIRIT1_B_SPI_SCLK_PORT, SPIRIT1_B_SPI_SCLK_PIN)
#define SPIRIT1_B_SPI_SCLK_LOW()    GPIO_ResetBits(SPIRIT1_B_SPI_SCLK_PORT, SPIRIT1_B_SPI_SCLK_PIN)

#define SPIRIT1_B_SPI_MOSI_PORT     GPIOB
#define SPIRIT1_B_SPI_MOSI_PIN      GPIO_Pin_10
#define SPIRIT1_B_SPI_MOSI_HIGH()   GPIO_SetBits(SPIRIT1_B_SPI_MOSI_PORT, SPIRIT1_B_SPI_MOSI_PIN)
#define SPIRIT1_B_SPI_MOSI_LOW()    GPIO_ResetBits(SPIRIT1_B_SPI_MOSI_PORT, SPIRIT1_B_SPI_MOSI_PIN)

#define SPIRIT1_B_SPI_MISO_PORT     GPIOB
#define SPIRIT1_B_SPI_MISO_PIN      GPIO_Pin_11
#define SPIRIT1_B_SPI_MISO_HIGH()   GPIO_SetBits(SPIRIT1_B_SPI_MISO_PORT, SPIRIT1_B_SPI_MISO_PIN)
#define SPIRIT1_B_SPI_MISO_LOW()    GPIO_ResetBits(SPIRIT1_B_SPI_MISO_PORT, SPIRIT1_B_SPI_MISO_PIN)
#endif

/*lq SPIRIT1_B 中断引脚定义 */
#define SPIRIT1_B_GPIO0_SOURCE_PORT     GPIO_PortSourceGPIOA
#define SPIRIT1_B_GPIO0_SOURCE_PIN      GPIO_PinSource6
#define SPIRIT1_B_GPIO0_LINE        	EXTI_Line6

#define SPIRIT1_B_GPIO1_SOURCE_PORT     GPIO_PortSourceGPIOA
#define SPIRIT1_B_GPIO1_SOURCE_PIN      GPIO_PinSource7
#define SPIRIT1_B_GPIO1_LINE        	EXTI_Line7

#define SPIRIT1_B_GPIO2_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_B_GPIO2_SOURCE_PIN      GPIO_PinSource4
#define SPIRIT1_B_GPIO2_LINE        	EXTI_Line4

#define SPIRIT1_B_GPIO3_SOURCE_PORT     GPIO_PortSourceGPIOC
#define SPIRIT1_B_GPIO3_SOURCE_PIN      GPIO_PinSource5
#define SPIRIT1_B_GPIO3_LINE        	EXTI_Line5


/*lq SPIRIT 数据定义 */
#define SPIRIT1_HEADER_ADDRESS_READ    0x01
#define SPIRIT1_HEADER_ADDRESS_WRITE   0x00
#define SPIRIT1_HEADER_COMMAND_WRITE   0x80

/*lq ----------SPIRIT 寄存器地址----------- */
/*lq General configuration registers */
#define SPIRIT1_REGADDR_GC_ANA_FUNC_CONF1           0x00
#define SPIRIT1_REGADDR_GC_ANA_FUNC_CONF0           0x01
#define SPIRIT1_REGADDR_GC_GPIO3_CONF               0x02
#define SPIRIT1_REGADDR_GC_GPIO2_CONF               0x03
#define SPIRIT1_REGADDR_GC_GPIO1_CONF               0x04
#define SPIRIT1_REGADDR_GC_GPIO0_CONF               0x05
#define SPIRIT1_REGADDR_GC_MCU_CK_CONF              0x06
#define SPIRIT1_REGADDR_GC_XO_RCO_TEST              0xB4
#define SPIRIT1_REGADDR_GC_SYNTH_CONFIG0            0x9F
#define SPIRIT1_REGADDR_GC_SYNTH_CONFIG1            0x9E
#define SPIRIT1_REGADDR_GC_IF_OFFSET_ANA            0x07
/*lq Radio configuration registers (analog blocks) */
#define SPIRIT1_REGADDR_RCA_SYNT3                   0x08
#define SPIRIT1_REGADDR_RCA_SYNT2                   0x09
#define SPIRIT1_REGADDR_RCA_SYNT1                   0x0A
#define SPIRIT1_REGADDR_RCA_SYNT0                   0x0B
#define SPIRIT1_REGADDR_RCA_CHSPACE                 0x0C
#define SPIRIT1_REGADDR_RCA_IF_OFFSET_DIG           0x0D
#define SPIRIT1_REGADDR_RCA_FC_OFFSET1              0x0E
#define SPIRIT1_REGADDR_RCA_FC_OFFSET0              0x0F
#define SPIRIT1_REGADDR_RCA_PA_POWER(n)             (0x18 - n)  //lq n = 0~8
/*lq Radio configuration registers (digital blocks) */
#define SPIRIT1_REGADDR_RCD_MOD1                    0x1A
#define SPIRIT1_REGADDR_RCD_MOD0                    0x1B
#define SPIRIT1_REGADDR_RCD_FDEV0                   0x1C
#define SPIRIT1_REGADDR_RCD_CHFLT                   0x1D
#define SPIRIT1_REGADDR_RCD_AFC2                    0x1E
#define SPIRIT1_REGADDR_RCD_AFC1                    0x1F
#define SPIRIT1_REGADDR_RCD_AFC0                    0x20
#define SPIRIT1_REGADDR_RCD_RSSI_FLT                0x21
#define SPIRIT1_REGADDR_RCD_RSSI_TH                 0x22
#define SPIRIT1_REGADDR_RCD_CLOCKREC                0x23
#define SPIRIT1_REGADDR_RCD_AGCCTRL2                0x24
#define SPIRIT1_REGADDR_RCD_AGCCTRL1                0x25
#define SPIRIT1_REGADDR_RCD_AGCCTRL0                0x26
#define SPIRIT1_REGADDR_RCD_ANT_SELECT_CONF         0x27
/*lq Packet/protocol configuration registers */
#define SPIRIT1_REGADDR_PPC_PCKTCTRL(n)             (0x34 - n)  //lq n = 1~4
#define SPIRIT1_REGADDR_PPC_PCKTLEN(n)              (0x35 - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_PPC_SYNC(n)                 (0x3A - n)  //lq n = 1~4
#define SPIRIT1_REGADDR_PPC_QI                      0x3A
#define SPIRIT1_REGADDR_PPC_MBUS_PRMBL              0x3B
#define SPIRIT1_REGADDR_PPC_MBUS_PSTMBL             0x3C
#define SPIRIT1_REGADDR_PPC_MBUS_CTRL               0x3D
#define SPIRIT1_REGADDR_PPC_FIFO_CONFIG(n)          (0x41 - n)  //lq n = 0~3
#define SPIRIT1_REGADDR_PPC_PCKT_FLT_GOALS(n)       (0x4E - n)  //lq n = 0~12
#define SPIRIT1_REGADDR_PPC_PCKT_FLT_OPTIONS        0x4F
#define SPIRIT1_REGADDR_PPC_PROTOCOL(n)             (0x52 - n)  //lq n = 0~2
#define SPIRIT1_REGADDR_PPC_TIMERS(n)               (0x58 - n)  //lq n = 0~5
#define SPIRIT1_REGADDR_PPC_CSMA_CONFIG(n)          (0x67 - n)  //lq n = 0~3
#define SPIRIT1_REGADDR_PPC_TX_CTRL_FIELD(n)        (0x6B - n)  //lq n = 0~3
#define SPIRIT1_REGADDR_PPC_PM_CONFIG(n)            (0xA6 - n)  //lq n = 0~2
#define SPIRIT1_REGADDR_PPC_XO_RCO_CONFIG           0xA7
#define SPIRIT1_REGADDR_PPC_TEST_SELECT             0xA8
#define SPIRIT1_REGADDR_PPC_PM_TEST                 0xB2
/*lq Frequently used registers */
#define SPIRIT1_REGADDR_FU_CHNUM                    0x6C
#define SPIRIT1_REGADDR_FU_VCO_CONFIG               0xA1
#define SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_IN(n)     (0x6F - n)  //lq n = 0~2
#define SPIRIT1_REGADDR_FU_AES_KEY_IN(n)            (0x7F - n)  //lq n = 0~15
#define SPIRIT1_REGADDR_FU_AES_DATA_IN(n)           (0x8F - n)  //lq n = 0~15
#define SPIRIT1_REGADDR_FU_IRQ_MASK(n)              (0x93 - n)  //lq n = 0~3
#define SPIRIT1_REGADDR_FU_TDEM_CONFIG              0xA3
#define SPIRIT1_REGADDR_FU_PM_CONFIG                0xA4
#define SPIRIT1_REGADDR_FU_MC_STATE(n)              (0xC1 - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_FU_TX_PCKT_INFO             0xC2
#define SPIRIT1_REGADDR_FU_RX_PCKT_INFO             0xC3
#define SPIRIT1_REGADDR_FU_AFC_CORR                 0xC4
#define SPIRIT1_REGADDR_FU_LINK_QUALIF(n)           (0xC7 - n)  //lq n = 0~2
#define SPIRIT1_REGADDR_FU_TX_PCKT_INFO             0xC2
#define SPIRIT1_REGADDR_FU_RSSI_LEVEL               0xC8
#define SPIRIT1_REGADDR_FU_RX_PCKT_LEN(n)           (0xCA - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_FU_CRC_FIELD(n)             (0xCD - n)  //lq n = 0~2
#define SPIRIT1_REGADDR_FU_RX_CTRL_FIELD(n)         (0xD1 - n)  //lq n = 0~3
#define SPIRIT1_REGADDR_FU_RX_ADDR_FIELD(n)         (0xD3 - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_FU_AES_DATA_OUT(n)          (0xE3 - n)  //lq n = 0~15
#define SPIRIT1_REGADDR_FU_RCO_VCO_CALIBR_OUT(n)    (0xE5 - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_FU_LINEAR_FIFO_STATUS(n)    (0xE7 - n)  //lq n = 0~1
#define SPIRIT1_REGADDR_FU_IRQ_STATUS(n)            (0xFD - n)  //lq n = 0~3
/*lq General information */
#define SPIRIT1_REGADDR_GI_PARTNUM                  0xF0
#define SPIRIT1_REGADDR_GI_VERSION                  0xF1


/*lq ---------SPIRIT 寄存器位掩码 ----------------*/
#define SPIRIT1_GC_ANA_FUNC_CONF1_BITMASK_SET_BLD_LVL           0x03
#define SPIRIT1_GC_ANA_FUNC_CONF1_BITMASK_GM_CONF               0x1C
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_TS                    0x01
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_BATTERY_LEVEL         0x02
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_BROWN_OUT             0x04
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_EXT_REF               0x10
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_AES_ON                0x20
#define SPIRIT1_GC_ANA_FUNC_CONF0_BITMASK_24_26MHz_SELECT       0x40
#define SPIRIT1_GC_GPIO3_CONF_BITMASK_GPIO_MODE                 0x03
#define SPIRIT1_GC_GPIO3_CONF_BITMASK_GPIO_SELECT               0xF8
#define SPIRIT1_GC_GPIO2_CONF_BITMASK_GPIO_MODE                 0x03
#define SPIRIT1_GC_GPIO2_CONF_BITMASK_GPIO_SELECT               0xF8
#define SPIRIT1_GC_GPIO1_CONF_BITMASK_GPIO_MODE                 0x03
#define SPIRIT1_GC_GPIO1_CONF_BITMASK_GPIO_SELECT               0xF8
#define SPIRIT1_GC_GPIO0_CONF_BITMASK_GPIO_MODE                 0x03
#define SPIRIT1_GC_GPIO0_CONF_BITMASK_GPIO_SELECT               0xF8
#define SPIRIT1_GC_MCU_CK_CONF_BITMASK_RCO_RATIO                0x01
#define SPIRIT1_GC_MCU_CK_CONF_BITMASK_XO_RATIO                 0x1E
#define SPIRIT1_GC_MCU_CK_CONF_BITMASK_CLOCK_TAIL               0x60
#define SPIRIT1_GC_MCU_CK_CONF_BITMASK_EN_MCU_CLK               0x80
#define SPIRIT1_GC_XO_RCO_TEST_BITMASK_PD_CLKDIV                0x04
#define SPIRIT1_GC_SYNTH_CONFIG0_BITMASK_SEL_TSPLIT             0x80
#define SPIRIT1_GC_SYNTH_CONFIG1_VCO_H_SEL                      0x02
#define SPIRIT1_GC_SYNTH_CONFIG1_VCO_L_SEL                      0x04
#define SPIRIT1_GC_SYNTH_CONFIG1_REFDIV                         0x80
#define SPIRIT1_GC_IF_OFFSET_ANA_BITMASK_IF_OFFSET_ANA          0xFF
#define SPIRIT1_RCA_SYNT3_BITMASK_SYNT25_21                     0x1F
#define SPIRIT1_RCA_SYNT3_BITMASK_WCP                           0xE0
#define SPIRIT1_RCA_SYNT2_BITMASK_SYNT20_13                     0xFF
#define SPIRIT1_RCA_SYNT1_BITMASK_SYNT12_5                      0xFF
#define SPIRIT1_RCA_SYNT0_BITMASK_BS                            0x07
#define SPIRIT1_RCA_SYNT0_BITMASK_SYNT4_0                       0xF8
#define SPIRIT1_RCA_CHSPACE_BITMASK_CH_SPACING                  0xFF
#define SPIRIT1_RCA_IF_OFFSET_DIG_BITMASK_IF_OFFSET_DIG         0xFF
#define SPIRIT1_RCA_FC_OFFSET1_BITMASK_FC_OFFSET11_18           0x0F
#define SPIRIT1_RCA_FC_OFFSET0_BITMASK_FC_OFFSET7_0             0xFF
#define SPIRIT1_RCA_PA_POWER1_8_BITMASK_PA_LEVEL                0xEF
#define SPIRIT1_RCA_PA_POWER0_BITMASK_PA_LEVEL_MAX_INDEX        0x07
#define SPIRIT1_RCA_PA_POWER0_BITMASK_PA_RAMP_STEP_WIDTH        0x18
#define SPIRIT1_RCA_PA_POWER0_BITMASK_PA_RAMP_ENABLE            0x20
#define SPIRIT1_RCA_PA_POWER0_BITMASK_CWC                       0xC0
#define SPIRIT1_RCD_MOD1_BITMASK_DATARATE_M                     0xFF
#define SPIRIT1_RCD_MOD0_BITMASK_DATARATE_E                     0x0F
#define SPIRIT1_RCD_MOD0_BITMASK_MOD_TYPE                       0x30
#define SPIRIT1_RCD_MOD0_BITMASK_BT_SEL                         0x40
#define SPIRIT1_RCD_MOD0_BITMASK_CW                             0x80
#define SPIRIT1_RCD_FDEV0_BITMASK_FDEV_M                        0x07
#define SPIRIT1_RCD_FDEV0_BITMASK_CLOCK_REC_ALGO_SEL            0x08
#define SPIRIT1_RCD_FDEV0_BITMASK_FDEV_E                        0xF0
#define SPIRIT1_RCD_CHFLT_BITMASK_CHFLT_E                       0x0F
#define SPIRIT1_RCD_CHFLT_BITMASK_CHFLT_M                       0xF0
#define SPIRIT1_RCD_AFC2_BITMASK_AFC_PD_LEAKAGE                 0x1F
#define SPIRIT1_RCD_AFC2_BITMASK_AFC_MODE                       0x20
#define SPIRIT1_RCD_AFC2_BITMASK_AFC_ENABLE                     0x40
#define SPIRIT1_RCD_AFC2_BITMASK_AFC_FREEZE_ON_SYNC             0x80
#define SPIRIT1_RCD_AFC1_BITMASK_AFC_FAST_PERIOD                0xFF
#define SPIRIT1_RCD_AFC0_BITMASK_AFC_SLOW_GAIN_LOG2             0x0F
#define SPIRIT1_RCD_AFC0_BITMASK_AFC_FAST_GAIN_LOG2             0xF0
#define SPIRIT1_RCD_RSSI_FLT_BITMASK_OOK_PEAK_DECAY             0x03
#define SPIRIT1_RCD_RSSI_FLT_BITMASK_CS_MODE                    0x0C
#define SPIRIT1_RCD_RSSI_FLT_BITMASK_RSSI_FLT                   0xF0
#define SPIRIT1_RCD_RSSI_TH_BITMASK_RSSI_THRESHOLD              0xFF
#define SPIRIT1_RCD_CLOCKREC_BITMASK_CLK_REC_I_GAIN             0x0F
#define SPIRIT1_RCD_CLOCKREC_BITMASK_PSTFLT_LEN                 0x10
#define SPIRIT1_RCD_CLOCKREC_BITMASK_CLK_REC_P_GAIN             0xE0
#define SPIRIT1_RCD_AGCCTRL2_BITMASK_MEAS_TIME                  0x0F
#define SPIRIT1_RCD_AGCCTRL1_BITMASK_THRESHOLD_LOW              0x0F
#define SPIRIT1_RCD_AGCCTRL1_BITMASK_THRESHOLD_HIGH             0xF0
#define SPIRIT1_RCD_AGCCTRL0_BITMASK_AGC_ENABLE                 0x80
#define SPIRIT1_RCD_ANT_SELECT_CONF_BITMASK_AS_MEAS_TIME        0x07
#define SPIRIT1_RCD_ANT_SELECT_CONF_BITMASK_AS_ENABLE           0x08
#define SPIRIT1_RCD_ANT_SELECT_CONF_BITMASK_CS_BLANKING         0x10

#define SPIRIT1_PPC_PCKTCTRL1_BITMASK_TXSOURCE                  0x0C

#define SPIRIT1_FU_RCO_VCO_CALIBR_IN1_BITMASK_VCO_CALIBR_TX     0x7F
#define SPIRIT1_FU_RCO_VCO_CALIBR_IN0_BITMASK_VCO_CALIBR_RX     0x7F


#define SPIRIT1_FU_PM_CONFIG_BITMASK_DISABLE_SMPS               0x20
#define SPIRIT1_FU_PM_CONFIG_BITMASK_EN_TS_BUFFER               0x40
#define SPIRIT1_FU_MC_STATE1_BITMASK_ERROR_LOCK                 0x01
#define SPIRIT1_FU_MC_STATE1_BITMASK_RX_FIFO_EMPTY              0x02
#define SPIRIT1_FU_MC_STATE1_BITMASK_TX_FIFO_FULL               0x04
#define SPIRIT1_FU_MC_STATE1_BITMASK_ANT_SELECT                 0x08
#define SPIRIT1_FU_MC_STATE0_BITMASK_XO_ON                      0x01
#define SPIRIT1_FU_MC_STATE0_BITMASK_STATE                      0xFE

#define SPIRIT1_FU_LINK_QUALIF1_BITMASK_CS                      0x80

#define SPIRIT1_FU_RCO_VCO_CALIBR_OUT0_BITMASK_VCO_CALIBR_DATA  0x7F

/*lq ---------SPIRIT 寄存器位序号 ----------------*/
#define SPIRIT1_FU_MC_STATE0_BITNUM_STATE                       1u


/*lq SPIRIT1 GPIO 功能定义*/
#define SPIRIT1_GPIO_MODE_ANA                                   0x00
#define SPIRIT1_GPIO_MODE_DIN                                   0x01
#define SPIRIT1_GPIO_MODE_DOUT_LOW                              0x02
#define SPIRIT1_GPIO_MODE_DOUT_HIGH                             0x03

#define SPIRIT1_GPIO_FUC_OUT_TX_CLOCK                           (4u << 3u)
#define SPIRIT1_GPIO_FUC_OUT_RX_DATA                            (8u << 3u)
#define SPIRIT1_GPIO_FUC_OUT_RX_CLOCK                           (9u << 3u)
#define SPIRIT1_GPIO_FUC_IN_TX_DATA                             (2u << 3u)

/*lq SPIRIT1 Command Code */
#define SPIRIT1_COMMAND_TX                                      0x60
#define SPIRIT1_COMMAND_RX                                      0x61
#define SPIRIT1_COMMAND_READY                                   0x62
#define SPIRIT1_COMMAND_STANDBY                                 0x63
#define SPIRIT1_COMMAND_SLEEP                                   0x64
#define SPIRIT1_COMMAND_LOCKRX                                  0x65
#define SPIRIT1_COMMAND_LOCKTX                                  0x66
#define SPIRIT1_COMMAND_SABORT                                  0x67
#define SPIRIT1_COMMAND_LDC_RELOAD                              0x68
#define SPIRIT1_COMMAND_SEQUENCE_UPDATE                         0x69
#define SPIRIT1_COMMAND_AES_Enc                                 0x6A
#define SPIRIT1_COMMAND_AES_Key                                 0x6B
#define SPIRIT1_COMMAND_AES_Dec                                 0x6C
#define SPIRIT1_COMMAND_AES_KeyDec                              0x6D
#define SPIRIT1_COMMAND_SRES                                    0x70
#define SPIRIT1_COMMAND_FLUSHRXFIFO                             0x71
#define SPIRIT1_COMMAND_FLUSHTXFIFO                             0x72

/*lq SPIRIT1 MC Status */
#define SPIRIT1_MC_STATE_STANDBY                                0x40
#define SPIRIT1_MC_STATE_SLEEP                                  0x36
#define SPIRIT1_MC_STATE_READY                                  0x03
#define SPIRIT1_MC_STATE_LOCK                                   0x0F
#define SPIRIT1_MC_STATE_RX                                     0x33
#define SPIRIT1_MC_STATE_TX                                     0x5F

/*lq 参数值 */
#define SPIRIT1_PARA_CH_NUM_A                                   0x0E    //lq AIS channelA
#define SPIRIT1_PARA_CH_NUM_B                                   0x0F    //lq AIS channelA

/*lq 发射数据源 */
#define SPIRIT1_TX_SOURCE_DATA_NORMAL                           0u
#define SPIRIT1_TX_SOURCE_DATA_FIFO                             1u
#define SPIRIT1_TX_SOURCE_DATA_GPIO                             2u
#define SPIRIT1_TX_SOURCE_DATA_PN9                              3u

/*lq AIS频率 */
#define AIS_FREQ_A                          161975000u  //lq 单位：1Hz
#define AIS_FREQ_B                          162025000u
#define AIS_CHANNEL_A                       'A'
#define AIS_CHANNEL_B                       'B'
/* Private typedef -----------------------------------------------------------*/
typedef enum SPIRIT1Type
{
   SPIRIT1TypeA,
   SPIRIT1TypeB,
}SPIRIT1TypeEnum;

#if 0
typedef struct SPIRIT1
{

}SPIRIT1Struct;
#endif

/* Private variables ---------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

/* Public define -------------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Public functions ----------------------------------------------------------*/
extern void SPIRIT1_InstConfigure(void);
extern U8 SPIRIT1_GetMcState(SPIRIT1TypeEnum cs);
extern void SPIRIT1_ReadRegisters(SPIRIT1TypeEnum cs);
extern void SPIRIT1_StateToReady(SPIRIT1TypeEnum cs);
#endif
