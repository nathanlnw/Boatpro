/*******************************************************************************
  Copyright (C), 2011, 712.Co.,Ltd.
  FileName: TestCase.c
  Author: lq       Version : V1.00        Date: 2012.04.24
  Description:     测试用例
  Version:         V1.00
  Function List:   // 主要函数及其功能
  			
  History:         // 历史修改记录
      <author>  <time>   <version >   <desc>
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
//#include "gentype.h"
#include "Ptl_Interface.h"


S32 curr_frame = 0;
S16 curr_slot = 0;


S32 GetFrameNum(void)
{
	return curr_frame;
}
S16 GetSlotNum(void)
{
	return curr_slot;
}
U8 GetMMISOSState(void)
{
	return 0;
}
void LinkPtlPort (MessManaStr *mmMessCell)
{

}
U8 GetGpsFixSta(GPSStruct *pGpsCell)
{
	pGpsCell->GPS_Sog = 1;
	pGpsCell->GPS_Longitud = 2;
	pGpsCell->GPS_Latitude = 3;
	pGpsCell->GPS_Cog = 4;
	return 'A';	
}

#if 1
void TestCase(void)
{
	curr_slot++;
	if(curr_slot == 2250)
	{
		curr_slot = 0;
		curr_frame++;
	}

}
#else

__declspec(naked) void __cdecl TestCase(S32 curr_frame, S16 curr_slot)
{
	__asm{
		PUSH EBP
		MOV EBP,ESP
		SUB ESP,0C0h
		PUSH EBX
		PUSH ESI
		PUSH EDI
		LEA EDI,[EBP-0C0h]
		MOV ECX,30h
		MOV EAX,0CCCCCCCCh
		REP STOS DWORD PTR ES:[EDI]
		MOV AX,WORD PTR DS:[curr_slot]
		ADD AX,1
		MOV WORD PTR DS:[curr_slot],AX
		MOVSX EAX,WORD PTR DS:[curr_slot]
		CMP EAX,8CAh
		JNZ LABEL_1
		MOV WORD PTR DS:[curr_slot],0h
		MOV EAX,DWORD PTR DS:[curr_frame]
		ADD EAX,1h
		MOV DWORD PTR DS:[curr_frame],EAX
LABEL_1:
		POP EDI
		POP ESI
		POP EBX
		MOV ESP,EBP
		POP EBP
		RETN
	}
}

#endif
int main(void)
{
	Ptl_Init();

	while(!_kbhit())
	{
		TestCase(curr_frame, curr_slot);
		Ptl_Main();
	}

	system("pause");//真正的'按任意键继续' 


	return 0;
}

