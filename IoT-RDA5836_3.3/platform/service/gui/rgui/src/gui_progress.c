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


#include "ap_gui.h"

#ifndef WIN32
#include "hal_debug.h"
#endif
//ˮƽ������λ��x
#if APP_SUPPORT_RGBLCD==1
#define PROGRESS_BAR_WIDTH    (130)
#elif APP_SUPPORT_LCD==1
#define PROGRESS_BAR_WIDTH    (98)
#else
#define PROGRESS_BAR_WIDTH    (4)
#endif
#define PROGRESS_BG_WIDTH     (4)
#define POSITION_PROGRESSBAR  (15)

extern BOOL ui_auto_update;

static const region_t region={0, 16, LCD_WIDTH, 16};	//�ؼ���������	

/*********************************************************************************
* Description : �������ؼ�
*
* Arguments   : progress, �������ṹ
*
* Returns     : 0
*
* Notes       : ����ؼ�û���Լ�����Ϣѭ��
*
*********************************************************************************/
INT32 GUI_Display_Progress(const progress_t *progress)
{
	INT32 start, end;

	BOOL need_draw=TRUE;

	APP_ASSERT(progress->value <= progress->max);
	APP_ASSERT(progress->value >= progress->min);

	
	while(1)
	{
		if(need_draw)
		{
			GUI_ClearScreen(&region);

			GUI_ResShowPic(GUI_IMG_PBAR, POSITION_PROGRESSBAR, 16);

			start = POSITION_PROGRESSBAR + 1;
			end   = (POSITION_PROGRESSBAR -1) + PROGRESS_BAR_WIDTH * (progress->value - progress->min)/ (progress->max - progress->min);
			APP_ASSERT(end < LCD_WIDTH);	
			//�ֶ���ʾ����
			while(start+PROGRESS_BG_WIDTH <= end)
			{
				GUI_ResShowPic(GUI_IMG_PBKGRD, start, 16);
				start += PROGRESS_BG_WIDTH;
			}	

			if(!ui_auto_update) GUI_UpdateScreen(NULL);

			need_draw = FALSE;
		}//need_draw

		//direct return
		return 0;
	}//while(1)
}




