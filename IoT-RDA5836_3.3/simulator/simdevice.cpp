// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "simdevice.h"

// Dispatch interfaces referenced by this interface
#include "screen.h"
#include "keypad.h"

/////////////////////////////////////////////////////////////////////////////
// CCoolPhone

IMPLEMENT_DYNCREATE(CCoolPhone, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CCoolPhone properties

/////////////////////////////////////////////////////////////////////////////
// CCoolPhone operations

CScreen CCoolPhone::Screen()
{
	LPDISPATCH pDispatch;
	InvokeHelper(0x1, DISPATCH_METHOD, VT_DISPATCH, (void*)&pDispatch, NULL);
	return CScreen(pDispatch);
}

CScreen CCoolPhone::SubScreen()
{
	LPDISPATCH pDispatch;
	InvokeHelper(0x2, DISPATCH_METHOD, VT_DISPATCH, (void*)&pDispatch, NULL);
	return CScreen(pDispatch);
}

CKeypad CCoolPhone::Keypad()
{
	LPDISPATCH pDispatch;
	InvokeHelper(0x3, DISPATCH_METHOD, VT_DISPATCH, (void*)&pDispatch, NULL);
	return CKeypad(pDispatch);
}

BOOL CCoolPhone::LoadPhoneScript(LPCTSTR lpszScript)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x4, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		lpszScript);
	return result;
}

BOOL CCoolPhone::SetBackgroundImage(LPCTSTR lpszImageFile)
{
	BOOL result;
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x5, DISPATCH_METHOD, VT_BOOL, (void*)&result, parms,
		lpszImageFile);
	return result;
}

void CCoolPhone::LoadSkinProfile(LPCTSTR lpszSkinProfile)
{
	static BYTE parms[] =
		VTS_BSTR;
	InvokeHelper(0x6, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 lpszSkinProfile);
}

void CCoolPhone::ShowSkinSetupDlg()
{
	InvokeHelper(0x7, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}

void CCoolPhone::ShowLeds(long leds_mask)
{
	static BYTE parms[] =
		VTS_I4;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 leds_mask);
}

void CCoolPhone::AboutBox()
{
	InvokeHelper(0xfffffdd8, DISPATCH_METHOD, VT_EMPTY, NULL, NULL);
}
