/*******************************************************************************
  Copyright (C), 2016, 712.Co.,Ltd.
  FileName:    PTT_lnw.c 
  Author:	 Nathanlnw	  Version : V1.00		 Date: 2012.07.10
  Description:		Realize  PTT  Related  fucntions
  Version:		   V1.00
  Function List:   1.  PTT control	Function   
						2.	  Cut out power  check	
						3. When  mainpower	cut  hanppened,  device's  working mode enter into	low power status
							which	disable  AIS RX   but  enable	AIS  TX.
  History:		   // 历史修改记录
	  <author>	<time>		<version >	 <desc>

Note:  If you make   PTT  related  work normal  Please  do two steps as  below:
         step 1 :  Enable   "Define  LNW "   in  file   "Inlclude.h"
         step 2:   Make   the     "CONFIG_PTT_EN"    define  value  =1   in file  "config.h"
*******************************************************************************/


#include "Include.h"

#if  CONFIG_PTT_EN


PTT_CLASS  PTT_OBJ;

/************************************************************
*  Function:       PTT_Init_IO								*
*  Description:                                                                    	*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void PTT_Init_IO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);//Disable JTAG SWD
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//Disable JTAG SWD
	//  1.  OUTPUT PIN  SET

	//  PTT   报警灯 
	GPIO_InitStructure.GPIO_Pin = PTT_WARNLIGHT_PIN_OUT;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PTT_WARNLIGHT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PTT_WARNLIGHT_PIN_OUT_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(PTT_WARNLIGHT_PORT_2, &GPIO_InitStructure);

    //  PTT  语音收发控制
	GPIO_InitStructure.GPIO_Pin = PTT_SPK_CTL_PIN_OUT;
	GPIO_Init(PTT_SPK_CTL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PTT_SPK_CTL_PIN_OUT_2;
	GPIO_Init(PTT_SPK_CTL_PORT_2, &GPIO_InitStructure);

	//  PTT   电源控制
	GPIO_InitStructure.GPIO_Pin = PTT_MODULE_POWERCTL_OUT;
	GPIO_Init(PTT_MODULE_POWERCTL_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PTT_MODULE_POWERCTL_OUT_2;
	GPIO_Init(PTT_MODULE_POWERCTL_PORT_2, &GPIO_InitStructure);

    //  2. INPUT PIN  SET
      //  语音接收状态检测
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = PTT_RX_PIN_IN;
	GPIO_Init(PTT_RX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PTT_RX_PIN_IN_2;
	GPIO_Init(PTT_RX_PORT_2, &GPIO_InitStructure);


	GPIO_InitStructure.GPIO_Pin = PTT_MAINPOWERCUT_IN;
	GPIO_Init(PTT_MAINPOWERCUT_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = PTT_MAINPOWERCUT_IN_2;
	GPIO_Init(PTT_MAINPOWERCUT_PORT_2, &GPIO_InitStructure);
	

    //  3. OUTPUT  INIT STATE
     PTT_WARNLIGHT_OFF;         //   OFF  Init 
	 PTT_SPK_CTL_RX;           //   SPK Keep  Rx STATE
	 PTT_MODULE_POWER_OFF;      //  Module Power ON  
   	
}

/************************************************************
*  Function:       PTT_OBJ_INIT								*
*  Description:                                                        			*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         NULL								*
************************************************************/
void PTT_OBJ_INIT(void)
{
   memset((U8*)&PTT_OBJ,0, sizeof(PTT_OBJ));
   PTT_OBJ.WarnLight_Keep_Duraiton=WARNLIGHT_KEEP;
   PTT_OBJ.PTT_TX_Keep_Duraiton=PTT_WAIT_KEEP;
   PTT_OBJ.PTT_IDLE_Duration=PTT_WAIT_KEEP;
   PTT_OBJ.PTT_Demo_Enable=Enable_PTT; 
   PTT_OBJ.PTT_TX_Enable=Disable_PTT;
   PTT_OBJ.PTT_powerOn_RunOnce=0;
}



/************************************************************
*  Function:       PTT_Warnlight_Control								*
*  Description:                                                               		*
*  Input:           无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         Put this function in  1s   timer 										*
************************************************************/
void PTT_Warnlight_Control(void)
{
   if(1==PTT_OBJ.WarnLight_Control_Enable)
   	{
   	    PTT_WARNLIGHT_ON;
        PTT_OBJ.WarnLight_time_counter++;
		if(PTT_OBJ.WarnLight_time_counter>=PTT_OBJ.WarnLight_Keep_Duraiton)
		{
            PTT_OBJ.WarnLight_time_counter=0;
			PTT_OBJ.WarnLight_Control_Enable=0;			
		}
   	}
   else
   	   PTT_WARNLIGHT_OFF;
}

/************************************************************
*  Function:       PTT_WARNLIGHT_ENABLE								*
*  Description:                                                               		*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         NULL								*
************************************************************/
void PTT_WARNLIGHT_ENABLE(void)
{
   PTT_OBJ.WarnLight_Control_Enable=1;
   PTT_OBJ.WarnLight_time_counter=0;  // start 
}



/************************************************************
*  Function:       PTT_TX_Control								*
*  Description:                                                               		*
*  Input:           无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         Put this function in  1s   timer 										*
************************************************************/
void PTT_TX_Control(void)
{
//   if((PTT_MODULE_POWER_STATUS_GET)&&(Enable_PTT==PTT_OBJ.PTT_Demo_Enable)&&(PTT_OBJ.LowPowerMode_Staus==Disable_PTT))
 // if((Enable_PTT==PTT_OBJ.PTT_Demo_Enable)&&(PTT_OBJ.LowPowerMode_Staus==Disable_PTT))
 
   if(Enable_PTT==PTT_OBJ.PTT_Demo_Enable)
	{      

	        if(Enable_PTT==PTT_OBJ.PTT_TX_Enable)    
		    {
	           //if(PTT_RX_STATUS_GET)   //  HIGH  means  no  Rx : idle
			   {
					PTT_WARNLIGHT_ON;
					PTT_MODULE_POWER_ON;
			        PTT_OBJ.PTT_TX_timecounter++;
					if(PTT_OBJ.PTT_TX_timecounter>=5) 
						{
						   PTT_SPK_CTL_SD; 
						}
					else
						{
						   PTT_SPK_CTL_RX;
						}
					if(PTT_OBJ.PTT_TX_timecounter>=PTT_OBJ.PTT_TX_Keep_Duraiton)
					{
			            PTT_OBJ.PTT_TX_timecounter=0;
	                    // Convert  to   RX
						PTT_OBJ.PTT_TX_Enable=Disable_PTT;		
						PTT_SPK_CTL_RX;
						PTT_WARNLIGHT_OFF;
						PTT_MODULE_POWER_OFF; 
						PTT_OBJ.PTT_IDLE_timecouteer=0;
					}
					if(PTT_OBJ.PTT_powerOn_RunOnce==1)
							  PTT_OBJ.PTT_TX_Keep_Duraiton=PTT_TX_KEEP; 
	           	}	
			   // UartTxString("\r\n    -------PTT  IDLE \r\n");
		   	}
		   else
		   	{
	                PTT_OBJ.PTT_IDLE_timecouteer++;
					if(PTT_OBJ.PTT_IDLE_timecouteer>PTT_OBJ.PTT_TX_Keep_Duraiton) 
					{
	                    PTT_OBJ.PTT_IDLE_timecouteer=0;
	                    PTT_OBJ.PTT_TX_Enable=Enable_PTT;
						PTT_OBJ.PTT_TX_timecounter=0;//clear

						if(PTT_OBJ.PTT_powerOn_RunOnce==0)
							PTT_OBJ.PTT_powerOn_RunOnce=1;
						
						if(PTT_OBJ.PTT_powerOn_RunOnce==1)
							PTT_OBJ.PTT_TX_Keep_Duraiton=PTT_TX_KEEP;

				}
		   	}
  	}
    
  /*  #if  1
     else
	{
	  PTT_SPK_CTL_RX;
	  PTT_WARNLIGHT_OFF;
	  PTT_OBJ.PTT_TX_timecounter=0;
	  PTT_OBJ.PTT_IDLE_timecouteer=0;
	 // UartTxString("\r\n    -------PTT  RX2 \r\n");
    }  	
	#endif*/
}


/************************************************************
*  Function:       PTT_LowPower_Get							*
*  Description:                                                               		*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         Null				*
************************************************************/
U8 PTT_LowPower_Get(void)
{
   return   PTT_OBJ.LowPowerMode_Staus;
 }


/************************************************************
*  Function:       PTT_MainPower_Check								*
*  Description:                                                               		*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:          put it  into main  whie							*
************************************************************/
void PTT_MainPower_Check(void)
{
  if(PTT_MAINPOWERCUT_STATUS_GET)
  {                                    // power cut down  state
     if(MAIN__POWER_NORMAL==PTT_OBJ.MainPower_Status)
     {
         PTT_WARNLIGHT_ENABLE();  // once 

		 PTT_MODULE_POWER_OFF;   //  cut down PTT power
	   
         PTT_OBJ.LowPowerMode_Staus=Enable_PTT;

         //  Check   and  STOP   RX
         if(RX_EN_PIN_GET)
         {  // RX state
             RxTxCtrl(RXTX_CTRL_TYPE_TX, 0, 0);       // change  to send status                
         }
		 
     }
     
     PTT_OBJ.MainPower_Status=MAIN__POWER_CutDown;
  }
  else
  {  
     if(PTT_OBJ.LowPowerMode_Staus==Enable_PTT)
     {
	      //   RX   Recover   
	      PTT_OBJ.LowPowerMode_Staus=Disable_PTT;	 
		  
		  if(SI446X_RXSTTUS_Return())
	      {  // RX state
	             RxTxCtrl(RXTX_CTRL_TYPE_RX, 0, 0);       // Recover to Rx          
	      }		  
     }
  
     //  part 2 
     PTT_OBJ.MainPower_Status=MAIN__POWER_NORMAL;
	 PTT_OBJ.LowPowerMode_Staus=Disable_PTT;
	// PTT_MODULE_POWER_ON;  // main power recover   PTT power   


	
  }
   
}


/************************************************************
*  Function:       PTT_Work_Process								*
*  Description:                                                               		*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:          put it  into main  whie							*
************************************************************/
void PTT_Work_Process(void)
{
   //PTT_MainPower_Check();   
}

/************************************************************
*  Function:       PTT_Timer_Service								*
*  Description:                                                               		*
*  Input:          无										*
*  Output:         无										*
*  Return:         无										*
*  Others:         Put this function in  1ms   timer 						*
************************************************************/
void PTT_Timer_Service(void)
{
    PTT_OBJ.OneSecond_counter++;
	if(PTT_OBJ.OneSecond_counter>=1000)
	{ 
	  PTT_OBJ.OneSecond_counter=0;  // clear
     // PTT_Warnlight_Control();  
	  PTT_TX_Control();
	} 
}


#endif
