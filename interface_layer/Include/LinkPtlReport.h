/****************************************************************************
*  Copyright (C), 2010, 712.Co.,Ltd.                                        *
*  FileName: LinkPtlReport.h								   						*
*  Description:  底层与协议栈反馈接口函数相关变量定义及函数声明							*
*****************************************************************************
*  History:               				                                    *
*      <author>  <time>    <version >   <desc>                              *
*      vozhj   	 10/11/25     1.0       Build this file					    *
****************************************************************************/
#ifndef _LINKPTLREPORT_H
#define _LINKPTLREPORT_H


/*****************************************************************************
***********************			结构体定义   		***********************
****************************************************************************/

#define REPORT_TRANERR_NONE			0
#define REPORT_TRANERR_PHASEERR		1
#define REPORT_TRANERR_CHERR		2
#define REPORT_TRANERR_SLOTERR0		3
#define REPORT_TRANERR_OK			4
#define REPORT_TRANERR_SLOTERR1		5
#define REPORT_TRANERR_ATERR		6
#define REPORT_TRANERR_LENERR		7
#define REPORT_TRANERR_ENDFRAME		8
#define REPORT_TRANERR_PLLERR		9

typedef struct {
	U16 uiTranSta;			//发送成功失败状态
	U16 uiServicNum;			//当前状态业务号

	U16 uiATval;				//AT标志
}LinkPtlReportStruct;

extern LinkPtlReportStruct LinkPtlReportPara;
/*****************************************************************************
***********************			函数声明    		***********************
****************************************************************************/

extern void InitaReportPara (void);
extern void ReportToPtl (void);
#endif











