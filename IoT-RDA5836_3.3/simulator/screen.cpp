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
// Machine generated IDispatch wrapper class(es) created by Microsoft Visual C++

// NOTE: Do not modify the contents of this file.  If this class is regenerated by
//  Microsoft Visual C++, your modifications will be overwritten.


#include "stdafx.h"
#include "screen.h"

/////////////////////////////////////////////////////////////////////////////
// CScreen properties

long CScreen::GetLeft()
{
	long result;
	GetProperty(0x1, VT_I4, (void*)&result);
	return result;
}

void CScreen::SetLeft(long propVal)
{
	SetProperty(0x1, VT_I4, propVal);
}

long CScreen::GetTop()
{
	long result;
	GetProperty(0x2, VT_I4, (void*)&result);
	return result;
}

void CScreen::SetTop(long propVal)
{
	SetProperty(0x2, VT_I4, propVal);
}

long CScreen::GetWidth()
{
	long result;
	GetProperty(0x3, VT_I4, (void*)&result);
	return result;
}

void CScreen::SetWidth(long propVal)
{
	SetProperty(0x3, VT_I4, propVal);
}

long CScreen::GetHeight()
{
	long result;
	GetProperty(0x4, VT_I4, (void*)&result);
	return result;
}

void CScreen::SetHeight(long propVal)
{
	SetProperty(0x4, VT_I4, propVal);
}

long CScreen::GetColor()
{
	long result;
	GetProperty(0x5, VT_I4, (void*)&result);
	return result;
}

long CScreen::GetBKColor()
{
	long result;
	GetProperty(0x6, VT_I4, (void*)&result);
	return result;
}


/////////////////////////////////////////////////////////////////////////////
// CScreen operations

unsigned long CScreen::SetPixel(long xPos, long yPos, unsigned long rcColor)
{
	unsigned long result;
	static BYTE parms[] =
		VTS_I4 VTS_I4 VTS_I4;
	InvokeHelper(0x7, DISPATCH_METHOD, VT_I4, (void*)&result, parms,
		xPos, yPos, rcColor);
	return result;
}

void CScreen::UpdateScreen(long* pFrameBuffer, long* pBitmapInfo)
{
	static BYTE parms[] =
		VTS_PI4 VTS_PI4;
	InvokeHelper(0x8, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pFrameBuffer, pBitmapInfo);
}

void CScreen::UpdateScreenEx(long* pFrameBuffer)
{
	static BYTE parms[] =
		VTS_PI4;
	InvokeHelper(0x9, DISPATCH_METHOD, VT_EMPTY, NULL, parms,
		 pFrameBuffer);
}
