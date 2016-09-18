/****************************************************************************
*  Copyright (C), 2012, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.c 				                                *
*  Description:  底层与协议栈反馈接口实现									*
*  Function List:   					                                    *
*	1--  void InitaReportPara (void);							     		*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 12/01/08     1.0       build this moudle		            *
****************************************************************************/

#include "def.h"
#include "LinkPtlReport.h"

LinkPtlReportStruct LinkPtlReportPara;

/************************************************************
*  Function:       InitaReportPara							*
*  Description:    初始化Report参数							*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void InitaReportPara (void)
{
	LinkPtlReportPara.uiATval = 0;

	LinkPtlReportPara.uiServicNum = 0;
	LinkPtlReportPara.uiTranSta = REPORT_TRANERR_NONE;
}

/************************************************************
*  Function:       ReportToPtl								*
*  Description:    向协议栈发送报告							*
*  Input:          无										*	
*  Output:         无										*
*  Return:         无										*
*  Others:         无										*
************************************************************/
void ReportToPtl (void)
{
	//有报告生成
	if (LinkPtlReportPara.uiTranSta != REPORT_TRANERR_NONE)
	{
		

		LinkPtlReportPara.uiTranSta = REPORT_TRANERR_NONE;

	}
	
}

























