/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: CMX7042.h
  Author: lq       Version : V1.00        Date: 2014.05.12
  Description:     CMX7042驱动
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#ifndef _CMX7042_H
#define _CMX7042_H

/* Include ------------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
/*lq CMX7042 FI 版本 */
#define CMX7042_FI_VER_1_2_4_0

/*lq CMX7042 激活码 */  
#define ACTIVATE_CHK_HI 0xD5D2
#define ACTIVATE_CHK_LO 0xAA8A

/*lq 执行操作前是否进行标志确认 */
#define CMX7042_FLAG_CHECK          1

/*lq CMX7042引脚定义 */
#define CMX7042_VCC_PORT            GPIOA
#define CMX7042_VCC_PIN             GPIO_Pin_7

#define CMX7042_VCC_HIGH()          GPIO_SetBits(CMX7042_VCC_PORT, CMX7042_VCC_PIN)
#define CMX7042_VCC_LOW()           GPIO_ResetBits(CMX7042_VCC_PORT, CMX7042_VCC_PIN)

#define CMX7042_SLOTCLK_PORT        GPIOC
#define CMX7042_SLOTCLK_PIN         GPIO_Pin_5

#define CMX7042_SLOTCLK_HIGH()      GPIO_SetBits(CMX7042_SLOTCLK_PORT, CMX7042_SLOTCLK_PIN)
#define CMX7042_SLOTCLK_LOW()       GPIO_ResetBits(CMX7042_SLOTCLK_PORT, CMX7042_SLOTCLK_PIN)

#define CMX7042_SLOTCLK_SOURCE_PORT GPIO_PortSourceGPIOC
#define CMX7042_SLOTCLK_SOURCE_PIN  GPIO_PinSource5
#define CMX7042_SLOTCLK_EXTI_LINE   EXTI_Line5

#define CMX7042_SLOTCLK_TRIG_SOURCE_PORT GPIO_PortSourceGPIOA
#define CMX7042_SLOTCLK_TRIG_SOURCE_PIN  GPIO_PinSource6
#define CMX7042_SLOTCLK_TRIG_EXTI_LINE   EXTI_Line6

#define CMX7042_IRQN_PORT           GPIOC
#define CMX7042_IRQN_PIN            GPIO_Pin_2

#define CMX7042_IRQN_SOURCE_PORT    GPIO_PortSourceGPIOC
#define CMX7042_IRQN_SOURCE_PIN     GPIO_PinSource2
#define CMX7042_IRQN_LINE        	EXTI_Line2

#define CMX7042_RSSI_PORT           GPIOC
#define CMX7042_RSSI1_PIN           GPIO_Pin_10
#define CMX7042_RSSI2_PIN           GPIO_Pin_11

#define CMX7042_RSSI1_HIGH()        GPIO_SetBits(CMX7042_RSSI_PORT, CMX7042_RSSI1_PIN)
#define CMX7042_RSSI1_LOW()         GPIO_ResetBits(CMX7042_RSSI_PORT, CMX7042_RSSI1_PIN)

#define CMX7042_RSSI2_HIGH()        GPIO_SetBits(CMX7042_RSSI_PORT, CMX7042_RSSI2_PIN)
#define CMX7042_RSSI2_LOW()         GPIO_ResetBits(CMX7042_RSSI_PORT, CMX7042_RSSI2_PIN)

/*lq CMX7042寄存器地址定义 */
#define CMX7042_REGW_ADDR_CBUSRESET              0x01    //lq W     
#define CMX7042_REGW_ADDR_DataWrite1             0xA7    //lq W 
#define CMX7042_REGW_ADDR_DataWrite2             0xA8    //lq W
#define CMX7042_REGR_ADDR_DataRead1              0xA9    //lq R
#define CMX7042_REGR_ADDR_DataRead2              0xAA    //lq R
#define CMX7042_REGW_ADDR_SystemClk1PLLData      0xAB    //lq W
#define CMX7042_REGW_ADDR_SystemClk1Ref          0xAC    //lq W
#define CMX7042_REGW_ADDR_SystemClk2PLLData      0xAD    //lq W
#define CMX7042_REGW_ADDR_SystemClk2Ref          0xAE    //lq W
#define CMX7042_REGW_ADDR_IOGainAndRouting       0xB1    //lq W
#define CMX7042_REGW_ADDR_RFPLLData              0xB2    //lq W
#define CMX7042_REGW_ADDR_RFPLLControl           0xB3    //lq W
#define CMX7042_REGR_ADDR_RFPLLStatus            0xB4    //lq R
#define CMX7042_REGW_ADDR_RxRawDataWordCounter   0xB5    //lq W
#define CMX7042_REGW_ADDR_DataWrite3             0xB6    //lq W
#define CMX7042_REGW_ADDR_DataWrite4             0xB7    //lq W
#define CMX7042_REGR_ADDR_DataRead3              0xB8    //lq R
#define CMX7042_REGR_ADDR_DataRead4              0xB9    //lq R
#define CMX7042_REGR_ADDR_RSS1                   0xBA    //lq R
#define CMX7042_REGR_ADDR_RSS2                   0xBB    //lq R
#define CMX7042_REGW_ADDR_PowerDown              0xC0    //lq W
#define CMX7042_REGW_ADDR_Mode                   0xC1    //lq W
#define CMX7042_REGW_ADDR_CSThreshold            0xC2    //lq W
#define CMX7042_REGR_ADDR_Status2                0xC5    //lq R
#define CMX7042_REGR_ADDR_Status                 0xC6    //lq R
#define CMX7042_REGW_ADDR_Command                0xC8    //lq W
#define CMX7042_REGR_ADDR_ADCData                0xC9    //lq R
#define CMX7042_REGW_ADDR_InterruptMask          0xCE    //lq W

/*lq CMX7042 Task Code */
/*lq Data Task Code  */
#define CMX7042_DTASK_NULL                          0x0000
#define CMX7042_DTASK_DataWordRead_RX1(n)           (n << 8)            //lq n=1~4
#define CMX7042_DTASK_DataWordReset_RX1(n)          ((n << 8) | 0x1000) //lq n=1~4
#define CMX7042_DTASK_DataBitRead_RX1(n)            ((n << 8) | 0x2000) //lq n=0~15
#define CMX7042_DTASK_DataBitReset_Rx1(n)           ((n << 8) | 0x3000) //lq n=0~15
#define CMX7042_DTASK_DataWordRead_RX2(n)           ((n << 8) | 0x4000) //lq n=1~4
#define CMX7042_DTASK_DataWordReset_RX2(n)          ((n << 8) | 0x5000) //lq n=1~4
#define CMX7042_DTASK_DataBitRead_RX2(n)            ((n << 8) | 0x6000) //lq n=0~15
#define CMX7042_DTASK_DataBitReset_Rx2(n)           ((n << 8) | 0x7000) //lq n=0~15
#define CMX7042_DTASK_DataWordWrite_Tx(n)           ((n << 8) | 0x8000) //lq n=1~4
#define CMX7042_DTASK_DataWordReset_Tx(n)           ((n << 8) | 0x9000) //lq n=1~4
#define CMX7042_DTASK_DataBitWrite_Tx(n)            ((n << 8) | 0xA000) //lq n=0~15
#define CMX7042_DTASK_DataBitReset_Tx(n)            ((n << 8) | 0xB000) //lq n=0~15
#define CMX7042_DTASK_DACWrite(n)                   ((n << 8) | 0x0800) //lq n=1~4
#if 0
#define CMX7042_DTASK_DataWordRead1_RX1             0x0100
#define CMX7042_DTASK_DataWordRead2_RX1             0x0200
#define CMX7042_DTASK_DataWordRead3_RX1             0x0300
#define CMX7042_DTASK_DataWordRead4_RX1             0x0400
#define CMX7042_DTASK_DataWordReset1_RX1            0x1100
#define CMX7042_DTASK_DataWordReset2_RX1            0x1200
#define CMX7042_DTASK_DataWordReset3_RX1            0x1300
#define CMX7042_DTASK_DataWordReset4_RX1            0x1400
#define CMX7042_DTASK_DataBitRead0_RX1              0x2000
#define CMX7042_DTASK_DataBitRead1_RX1              0x2100
#define CMX7042_DTASK_DataBitRead2_RX1              0x2200
#define CMX7042_DTASK_DataBitRead3_RX1              0x2300
#define CMX7042_DTASK_DataBitRead4_RX1              0x2400
#define CMX7042_DTASK_DataBitRead5_RX1              0x2500
#define CMX7042_DTASK_DataBitRead6_RX1              0x2600
#define CMX7042_DTASK_DataBitRead7_RX1              0x2700
#define CMX7042_DTASK_DataBitRead8_RX1              0x2800
#define CMX7042_DTASK_DataBitRead9_RX1              0x2900
#define CMX7042_DTASK_DataBitRead10_RX1             0x2A00
#define CMX7042_DTASK_DataBitRead11_RX1             0x2B00
#define CMX7042_DTASK_DataBitRead12_RX1             0x2C00
#define CMX7042_DTASK_DataBitRead13_RX1             0x2D00
#define CMX7042_DTASK_DataBitRead14_RX1             0x2E00
#define CMX7042_DTASK_DataBitRead15_RX1             0x2F00
#define CMX7042_DTASK_DataBitReset0_Rx1             0x3000
#define CMX7042_DTASK_DataBitReset1_Rx1             0x3100
#define CMX7042_DTASK_DataBitReset2_Rx1             0x3200
#define CMX7042_DTASK_DataBitReset3_Rx1             0x3300
#define CMX7042_DTASK_DataBitReset4_Rx1             0x3400
#define CMX7042_DTASK_DataBitReset5_Rx1             0x3500
#define CMX7042_DTASK_DataBitReset6_Rx1             0x3600
#define CMX7042_DTASK_DataBitReset7_Rx1             0x3700
#define CMX7042_DTASK_DataBitReset8_Rx1             0x3800
#define CMX7042_DTASK_DataBitReset9_Rx1             0x3900
#define CMX7042_DTASK_DataBitReset10_Rx1            0x3A00
#define CMX7042_DTASK_DataBitReset11_Rx1            0x3B00
#define CMX7042_DTASK_DataBitReset12_Rx1            0x3C00
#define CMX7042_DTASK_DataBitReset13_Rx1            0x3D00
#define CMX7042_DTASK_DataBitReset14_Rx1            0x3E00
#define CMX7042_DTASK_DataBitReset15_Rx1            0x3F00
#define CMX7042_DTASK_DataWordRead1_RX2             0x4100
#define CMX7042_DTASK_DataWordRead2_RX2             0x4200
#define CMX7042_DTASK_DataWordRead3_RX2             0x4300
#define CMX7042_DTASK_DataWordRead4_RX2             0x4400
#define CMX7042_DTASK_DataWordReset1_RX2            0x5100
#define CMX7042_DTASK_DataWordReset2_RX2            0x5200
#define CMX7042_DTASK_DataWordReset3_RX2            0x5300
#define CMX7042_DTASK_DataWordReset4_RX2            0x5400
#define CMX7042_DTASK_DataBitRead0_RX2              0x6000
#define CMX7042_DTASK_DataBitRead1_RX2              0x6100
#define CMX7042_DTASK_DataBitRead2_RX2              0x6200
#define CMX7042_DTASK_DataBitRead3_RX2              0x6300
#define CMX7042_DTASK_DataBitRead4_RX2              0x6400
#define CMX7042_DTASK_DataBitRead5_RX2              0x6500
#define CMX7042_DTASK_DataBitRead6_RX2              0x6600
#define CMX7042_DTASK_DataBitRead7_RX2              0x6700
#define CMX7042_DTASK_DataBitRead8_RX2              0x6800
#define CMX7042_DTASK_DataBitRead9_RX2              0x6900
#define CMX7042_DTASK_DataBitRead10_RX2             0x6A00
#define CMX7042_DTASK_DataBitRead11_RX2             0x6B00
#define CMX7042_DTASK_DataBitRead12_RX2             0x6C00
#define CMX7042_DTASK_DataBitRead13_RX2             0x6D00
#define CMX7042_DTASK_DataBitRead14_RX2             0x6E00
#define CMX7042_DTASK_DataBitRead15_RX2             0x6F00
#define CMX7042_DTASK_DataBitReset0_Rx2             0x7000
#define CMX7042_DTASK_DataBitReset1_Rx2             0x7100
#define CMX7042_DTASK_DataBitReset2_Rx2             0x7200
#define CMX7042_DTASK_DataBitReset3_Rx2             0x7300
#define CMX7042_DTASK_DataBitReset4_Rx2             0x7400
#define CMX7042_DTASK_DataBitReset5_Rx2             0x7500
#define CMX7042_DTASK_DataBitReset6_Rx2             0x7600
#define CMX7042_DTASK_DataBitReset7_Rx2             0x7700
#define CMX7042_DTASK_DataBitReset8_Rx2             0x7800
#define CMX7042_DTASK_DataBitReset9_Rx2             0x7900
#define CMX7042_DTASK_DataBitReset10_Rx2            0x7A00
#define CMX7042_DTASK_DataBitReset11_Rx2            0x7B00
#define CMX7042_DTASK_DataBitReset12_Rx2            0x7C00
#define CMX7042_DTASK_DataBitReset13_Rx2            0x7D00
#define CMX7042_DTASK_DataBitReset14_Rx2            0x7E00
#define CMX7042_DTASK_DataBitReset15_Rx2            0x7F00
#define CMX7042_DTASK_DataWordWrite1_Tx             0x8100
#define CMX7042_DTASK_DataWordWrite2_Tx             0x8200
#define CMX7042_DTASK_DataWordWrite3_Tx             0x8300
#define CMX7042_DTASK_DataWordWrite4_Tx             0x8400
#define CMX7042_DTASK_DataWordReset1_Tx             0x9100
#define CMX7042_DTASK_DataWordReset2_Tx             0x9200
#define CMX7042_DTASK_DataWordReset3_Tx             0x9300
#define CMX7042_DTASK_DataWordReset4_Tx             0x9400
#define CMX7042_DTASK_DataBitWrite0_Tx              0xA000
#define CMX7042_DTASK_DataBitWrite1_Tx              0xA100
#define CMX7042_DTASK_DataBitWrite2_Tx              0xA200
#define CMX7042_DTASK_DataBitWrite3_Tx              0xA300
#define CMX7042_DTASK_DataBitWrite4_Tx              0xA400
#define CMX7042_DTASK_DataBitWrite5_Tx              0xA500
#define CMX7042_DTASK_DataBitWrite6_Tx              0xA600
#define CMX7042_DTASK_DataBitWrite7_Tx              0xA700
#define CMX7042_DTASK_DataBitWrite8_Tx              0xA800
#define CMX7042_DTASK_DataBitWrite9_Tx              0xA900
#define CMX7042_DTASK_DataBitWrite10_Tx             0xAA00
#define CMX7042_DTASK_DataBitWrite11_Tx             0xAB00
#define CMX7042_DTASK_DataBitWrite12_Tx             0xAC00
#define CMX7042_DTASK_DataBitWrite13_Tx             0xAD00
#define CMX7042_DTASK_DataBitWrite14_Tx             0xAE00
#define CMX7042_DTASK_DataBitWrite15_Tx             0xAF00
#define CMX7042_DTASK_DataBitReset0_Tx              0xB000
#define CMX7042_DTASK_DataBitReset1_Tx              0xB100
#define CMX7042_DTASK_DataBitReset2_Tx              0xB200
#define CMX7042_DTASK_DataBitReset3_Tx              0xB300
#define CMX7042_DTASK_DataBitReset4_Tx              0xB400
#define CMX7042_DTASK_DataBitReset5_Tx              0xB500
#define CMX7042_DTASK_DataBitReset6_Tx              0xB600
#define CMX7042_DTASK_DataBitReset7_Tx              0xB700
#define CMX7042_DTASK_DataBitReset8_Tx              0xB800
#define CMX7042_DTASK_DataBitReset9_Tx              0xB900
#define CMX7042_DTASK_DataBitReset10_Tx             0xBA00
#define CMX7042_DTASK_DataBitReset11_Tx             0xBB00
#define CMX7042_DTASK_DataBitReset12_Tx             0xBC00
#define CMX7042_DTASK_DataBitReset13_Tx             0xBD00
#define CMX7042_DTASK_DataBitReset14_Tx             0xBE00
#define CMX7042_DTASK_DataBitReset15_Tx             0xBF00
#endif

/*lq Modem Task Code  */
#define CMX7042_MTASK_Rx1_NULL                      0x0000
#define CMX7042_MTASK_Rx1_Abort                     0x0001
#define CMX7042_MTASK_Rx1_RXB                       0x0002
#define CMX7042_MTASK_Rx1_RXR                       0x0013
#define CMX7042_MTASK_Rx1_DSCFormatted              0x0042
#define CMX7042_MTASK_Rx1_DSCRaw                    0x0003
#define CMX7042_MTASK_Rx2_NULL                      0x0004
#define CMX7042_MTASK_Rx2_Abort                     0x0005
#define CMX7042_MTASK_Rx2_RXB                       0x0006
#define CMX7042_MTASK_Rx2_RXR                       0x0017
#define CMX7042_MTASK_Rx2_DSCFormatted              0x0046
#define CMX7042_MTASK_Rx2_DSCRaw                    0x0007
#define CMX7042_MTASK_Tx_NULL                       0x0008
#define CMX7042_MTASK_Tx_Abort                      0x0009
#define CMX7042_MTASK_Tx_TXB                        0x000A
#define CMX7042_MTASK_Tx_TDBS                       0x002A
#define CMX7042_MTASK_Tx_TDB                        0x002B
#define CMX7042_MTASK_Tx_PRBS                       0x002C
#define CMX7042_MTASK_Tx_TRW                        0x002D                                                    
#define CMX7042_MTASK_HardwareControl               0x002E                                                    
#define CMX7042_MTASK_EnterConfigMode               0x000F                                                    

/*lq Configuration Task Code 的高3位全为0:000xxxxx */
#define CMX7042_CTASK_NULL                          0x0000
#define CMX7042_CTASK_EXIT_CONFIG                   0x0001
#define CMX7042_CTASK_TxIQOr2point                  0x0002
#define CMX7042_CTASK_TxMODLevels                   0x0003
#define CMX7042_CTASK_RSSI_lookup                   0x0004
#define CMX7042_CTASK_RSSI_window                   0x0005
#define CMX7042_CTASK_RSSI_gain                     0x0006
#define CMX7042_CTASK_CSTDMA_gain                   0x0007
#define CMX7042_CTASK_Tx_sequence                   0x0008
#define CMX7042_CTASK_SleepTiming                   0x0009
#define CMX7042_CTASK_RAMDACLoad                    0x000A
#define CMX7042_CTASK_DeviceIdent                   0x000B
#define CMX7042_CTASK_EnterDeepSleep                0x000C
#define CMX7042_CTASK_LeaveDeepsleep                0x000D
#define CMX7042_CTASK_SetSlotCount                  0x000E
#define CMX7042_CTASK_SystemOptions                 0x000F
#define CMX7042_CTASK_SetToACompensate              0x0010

/*lq Tx Sequence 宏定义 */
#define CMX7042_TXSEQ_EVENTID_DUMMY                 0x0000
#define CMX7042_TXSEQ_EVENTID_CSTDMA_START          0x0001
#define CMX7042_TXSEQ_EVENTID_CSTDMA_END            0x0002
#define CMX7042_TXSEQ_EVENTID_Tx_en_hi              0x0003
#define CMX7042_TXSEQ_EVENTID_RAMDAC_UP             0x0004
#define CMX7042_TXSEQ_EVENTID_MODULATE_START        0x0005
#define CMX7042_TXSEQ_EVENTID_MODULATE_END          0x0006
#define CMX7042_TXSEQ_EVENTID_RAMDAC_DOWN           0x0007
#define CMX7042_TXSEQ_EVENTID_Tx_en_lo              0x0008

/*lq 状态寄存器状态位掩码 */
#define CMX7042_STATUS_BITMASK_ACT                  0x0001              
#define CMX7042_STATUS_BITMASK_RFPLLState           0x0002
#define CMX7042_STATUS_BITMASK_ComRegFree           0x0004
#define CMX7042_STATUS_BITMASK_TxDone               0x0020
#define CMX7042_STATUS_BITMASK_Rx1BOVF              0x0040
#define CMX7042_STATUS_BITMASK_Rx2BOVF              0x0080
#define CMX7042_STATUS_BITMASK_CnfgTaskComp         0x0100
#define CMX7042_STATUS_BITMASK_RxState              0x0200
#define CMX7042_STATUS_BITMASK_DataTaskComp         0x0400
#define CMX7042_STATUS_BITMASK_R1BRDY               0x0800
#define CMX7042_STATUS_BITMASK_R2BRDY               0x1000
#define CMX7042_STATUS_BITMASK_TBFree               0x2000
#define CMX7042_STATUS_BITMASK_IRQ                  0x8000
#define CMX7042_STATUS_BITMASK_ALL_7032             0xBFE7
#define CMX7042_STATUS_BITMASK_ALL_7042             0xBFE5

/*lq 状态寄存器2状态位掩码 */
#define CMX7042_STATUS2_BITMASK_Rx1State            0x0007
#define CMX7042_STATUS2_BITMASK_Rx2State            0x0070
#define CMX7042_STATUS2_BITMASK_TxState             0x0F00
#define CMX7042_STATUS2_BITMASK_Conf                0x1000
#define CMX7042_STATUS2_BITMASK_DeepSleep           0x2000

/*lq 状态寄存器状态位 */
#if 0
#define CMX7042_STATUS_BIT_ACT                      0
#define CMX7042_STATUS_BIT_RFPLLState               1
#define CMX7042_STATUS_BIT_ComRegFree               2
#define CMX7042_STATUS_BIT_TxDone                   5
#define CMX7042_STATUS_BIT_Rx1BOVF                  6
#define CMX7042_STATUS_BIT_Rx2BOVF                  7
#define CMX7042_STATUS_BIT_CnfgTaskComp             8
#define CMX7042_STATUS_BIT_RxState                  9
#define CMX7042_STATUS_BIT_DataTaskComp             10
#define CMX7042_STATUS_BIT_R1BRDY                   11
#define CMX7042_STATUS_BIT_R2BRDY                   12
#define CMX7042_STATUS_BIT_TBFree                   13
#define CMX7042_STATUS_BIT_IRQ                      15
#endif

/*lq 接收机状态 */
#define CMX7042_RX_STATE_Idle                       0
#define CMX7042_RX_STATE_Receiving                  1
#define CMX7042_RX_STATE_ERROR_TwoLong              2   //lq burst mode
#define CMX7042_RX_STATE_ERROR_CRC                  3   //lq burst mode
#define CMX7042_RX_STATE_ERROR_NewFrameTwoBuffFull  4   //lq burst mode
#define CMX7042_RX_STATE_ERROR_EndFlgNotByteBound   5   //lq burst mode
#define CMX7042_RX_STATE_ChannelAsleep              6   //lq burst mode

/*lq 发射机状态 */
#define CMX7042_TX_STATE_Idle                       0
#define CMX7042_TX_STATE_BuildingMsgBuff            1   //lq burst mode
#define CMX7042_TX_STATE_MsgBuffReady               2
#define CMX7042_TX_STATE_TxPending                  3
#define CMX7042_TX_STATE_TxInProcess                4
#define CMX7042_TX_STATE_TxAborted_CSTDMA           5
#define CMX7042_TX_STATE_TxAborted_BuffNotReady     6   //lq burst mode
#define CMX7042_TX_STATE_TxAborted_MsgTooLong       7   //lq burst mode

/*lq CMX7042 接收机标志 */
#define CMX7042_RX_1                                1
#define CMX7042_RX_2                                2

/*lq SlotClk 脉冲宽度 */
#define CMX7042_SLOTCLK_PULSEWIDTH					640	//lq 单位：1phase = 20.8us

/* Private typedef -----------------------------------------------------------*/
typedef struct CMX7042
{
    U16 ProductIdCode;
    U16 FIVersionCode;
    U8  ActivedFlg;
    U16 StatusReg;
    U16 Status2Reg;
    U16 Status2RegOld;
    U16 IntMaskReg;
    U8  Rx1ErrFlg;
    U8  Rx2ErrFlg;
    U8  IntFlg;

	S16 SlotCnt;
	S16 PhaseCnt;	
	U8	SlotCalibFlg;

}CMX7042Struct;

/* Private variables ---------------------------------------------------------*/
/* Private  function  ---------------------------------------------------------*/
/* Public define ------------------------------------------------------------*/
/* Public  variables ---------------------------------------------------------*/

/* Public  function  ---------------------------------------------------------*/
extern U16 CMX7042_ReadWord(U8 addr);
extern void CMX7042_SetStatusBitFlg(U16 statusBit);
extern U8 CMX7042_GetRxState(U8 rxChannel);
extern void CMX7042_SetRxError(U8 rxChannel);
extern U8 CMX7042_GetIntFlgRst(void);

#endif
