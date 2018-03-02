/* 
 * Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/


// REVISION HISTORY:
// NAME              DATE                REMAKS
// Lixp      2012-2-20       Created initial version 1.0
//
// //////////////////////////////////////////////////////////////////////////////

/**************************************************************************************************
 *                                           INCLUDES
 **************************************************************************************************/
#include "at_common.h"
#include <drv_prv.h>
#include "at_module.h"
#include "at_cmd_special.h"

/**************************************************************************************************
 *                                          CONSTANTS
 **************************************************************************************************/

/**************************************************************************************************
 *                                           MACROS
 **************************************************************************************************/
//
// ERROR
//
#define AT_EX_FAIL_CreateRC(pResult,errCode,errorType,delayTime,pBuff,dataSize ,nDLCI)\
	do{                                                                                            \
		pResult = AT_CreateRC(CMD_FUNC_FAIL,CMD_RC_ERROR,errCode,errorType,delayTime,pBuff,dataSize,nDLCI);\
	} while(0)

/**************************************************************************************************
 *                                          TYPEDEFS
 **************************************************************************************************/

/**************************************************************************************************
 *                                       GLOBAL VARIABLES
 **************************************************************************************************/

/**************************************************************************************************
 *                                       FUNCTIONS - API
 **************************************************************************************************/

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_CmdFunc_PRDU(AT_CMD_PARA *pParam)
{
	INT32 eResult;
	PAT_CMD_RESULT pResult = NULL;

	eResult = ERR_INVALID_TYPE;
	AT_EX_FAIL_CreateRC(pResult, eResult, CMD_ERROR_CODE_TYPE_CME, 0, NULL, 0, pParam->nDLCI);

	// send result to atm

	if (NULL != pResult)
	{
		AT_Notify2ATM(pResult, pParam->nDLCI);
		AT_FREE(pResult);
		pResult = NULL;
	}
}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_CmdFunc_SCTM(AT_CMD_PARA *pParam)
{

}

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_CmdFunc_RCMD(AT_CMD_PARA *pParam)
{

}

/*----------------------------------------Asynchronism event process function--------------------------*/

/**************************************************************************************************
 * @fn
 *
 * @brief
 *
 * @param
 *
 * @return
 **************************************************************************************************/
VOID AT_EX_AsyncEventProcess(COS_EVENT *pEvent)
{
	// CSW_EMODE_RESULT*        pEmodeResult=NULL;
	PAT_CMD_RESULT pResult = NULL;

	// CFW_TSM_CURR_CELL_INFO*        pCurrCellInfo=NULL;
	// CSW_OFT_NEIGHBOR_CELL_INFO*      pNeiCellInfo=NULL;
	// CSW_OFT_ALL_NEBCELL_INFO*        pAllNeiCellInfo=NULL;
	// UINT8 i;
	// UINT32 nlen;
	// UINT8 res[1024];
	UINT8 nUTI = HIUINT16(pEvent->nParam3);

#if 0

	switch (pEvent->nEventId)
	{

	case EV_CFW_TSM_INFO_IND:
		T_MemZero(res, 1024);
		len           = 0;
		pCurrCellInfo = (CFW_TSM_CURR_CELL_INFO *)pEmodeResult->nData;

		T_Sprintf(res, "IMSI allowed: %u\r\n", pCurrCellInfo->bOFT_ATT);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Emergency call allowed: %u\r\n", pCurrCellInfo->bOFT_EC);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "SI7 or 8 broadcast: %u\r\n", pCurrCellInfo->bOFT_SI7_8);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "BCCH Dec Indicate: %u\r\n ", pCurrCellInfo->nOFT_BCCHDec);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "Absolute channel number: %u\r\n", pCurrCellInfo->nOFT_Arfcn);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "LAI:  %u", pCurrCellInfo->nOFT_LAI[0]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u", pCurrCellInfo->nOFT_LAI[1]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u", pCurrCellInfo->nOFT_LAI[2]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u", pCurrCellInfo->nOFT_LAI[3]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u\r\n", pCurrCellInfo->nOFT_LAI[4]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "RAC: %u\r\n", pCurrCellInfo->nOFT_RAC);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "CellID: %u", pCurrCellInfo->nOFT_CellID[0]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u\r\n", pCurrCellInfo->nOFT_CellID[1]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Bsic: %u\r\n", pCurrCellInfo->nOFT_Bsic);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Net Mode: %u\r\n", pCurrCellInfo->nOFT_NetMode);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "AvRxLevel(dBm): %u\r\n", pCurrCellInfo->nOFT_AvRxLevel);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "MaxTxPWR: %u\r\n", pCurrCellInfo->nOFT_MaxTxPWR);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "C1 Value: %d\r\n", pCurrCellInfo->nOFT_C1Value);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "TimeADV: %u\r\n", pCurrCellInfo->nOFT_TimeADV);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "CCCH Configuration: %u\r\n", pCurrCellInfo->nOFT_CCCHconf);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "BER: %u\r\n", pCurrCellInfo->nOFT_BER);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "Rx Full: %u\r\n", pCurrCellInfo->nOFT_RxQualFull);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "Rx Sub: %u\r\n", pCurrCellInfo->nOFT_RxQualSub);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "C2 Valure: %d\r\n", pCurrCellInfo->nOFT_C2);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Current chan type: %u\r\n", pCurrCellInfo->nOFT_CurrChanType);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "Current chan mode: %u\r\n", pCurrCellInfo->nOFT_CurrChanMode);
		len = AT_StrLen(res);

		T_Sprintf(res + nlen, "Current Band: %u\r\n", pCurrCellInfo->nOFT_CurrBand);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Max number of random access: %u\r\n", pCurrCellInfo->nOFT_MaxRetrans);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Number of AGCH in common channel: %u\r\n", pCurrCellInfo->nOFT_BsAgBlkRes);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "Access class: %u", pCurrCellInfo->nOFT_AC[0]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "%u\r\n", pCurrCellInfo->nOFT_AC[1]);
		len = AT_StrLen(res);
		T_Sprintf(res + nlen, "RxLevAccMin: %u\r\n", pCurrCellInfo->nOFT_RxLevAccMin);

		AT_EX_SUCC_CreateRC(pResult, 0, res, AT_StrLen(res));

		break;
	}

#endif
	// send result to atm
	if (NULL != pResult)
	{
		AT_Notify2ATM(pResult, nUTI);
		AT_FREE(pResult);
		pResult = NULL;
	}

}

// Only for at compiling issue
void lcd_present_log(VOID)
{
	return;
}
#ifndef MMI_WITH_AT
void MMIDisplayWaitingAnimation(void)
{
	return;
}
#endif
#if 0
BOOL mmi_vdoply_is_in_app(void)
{
	return 0;
}
BOOL mmi_audply_is_in_app(void)
{
	return 0;
}
#endif


