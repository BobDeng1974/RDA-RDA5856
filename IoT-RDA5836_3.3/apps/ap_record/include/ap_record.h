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


#ifndef _AP_RECORD_H_
#define _AP_RECORD_H_

#include "resource_id.h"
#include "ap_common.h" 
#include "ap_gui.h"
#include "ap_message.h"


#define RECORD_DIR              L"RECORD"
#define VOICE_DIR               L"VOICE" 
#define MUSIC_DIR               L"MUSIC" 

//���̵�ʣ��������ʱ������ʧ�ܣ��Լ���ѹ�ͣ���ʾ�Ĵ�����
#define	SPACE_LOW		      (20L*512)	//ʣ��bytes
#define LOW_POWER_COUNT		4	//ֹͣ¼��ʱ�͵�ѹ�����Ĵ���

typedef enum
{
	RECORD_STOP,
	RECORD_RECORD,
	RECORD_PALY
}RECORD_STATUS;


#define  MICREC_NOSTART      RESULT_RECORD_NOSTART                //��FM¼��,��ʱ¼��Դ��g_mrecord_vars.rec_source����
#define  MICREC_START   RESULT_RECORD_START         //��FM������,��REC��ֱ�ӽ���MIC¼��.
#define  FMREC_START    RESULT_FMREC_START    //FM¼��.FM����������̨ʱ���ȼ�REC���е�FM¼��
#define  FMREC_NOSTART  RESULT_FMREC_NOSTART  //FM¼��.FM����������̨ʱѡ��"��̨¼��"�˵����е�FM¼��

//¼��Դ��������(ȫ�ֱ���:g_rec_from���õ�!)
#define  R_SPDIF          0x80
#define  R_FM             0x40
#define  R_LINEIN         0x20
#define  R_MIC            0x10

#if (APP_SUPPORT_RGBLCD==1)
#define RECORD_CUR_TIME_X   36
#define RECORD_TIME_Y   36

#define RECORD_FILENAME_Y   70

#define RECORD_FILETIME_X   18
#define RECORD_FILETIME_Y   100

#define RECORD_FILEDATE_X   106
#define RECORD_FILEDATE_Y   100
#elif (APP_SUPPORT_LCD==1)
#define RECORD_CUR_TIME_X   20
#define RECORD_TIME_Y   16

#define RECORD_FILENAME_Y   32

#define RECORD_FILETIME_X   0
#define RECORD_FILETIME_Y   48

#define RECORD_FILEDATE_X   88
#define RECORD_FILEDATE_Y   48

#endif


typedef struct
{
	UINT16 rec_sample_rate;	//������	
	UINT16 wav_num;       //��ǰ¼���ļ��ı��
    UINT8 volume;
	file_location_t location;
	UINT16 maigc;		//�����ж�vm��������Ƿ���Ч
}record_vars_t;

typedef struct
{
    UINT16 maigc; //�����ж�vm��������Ƿ���Ч
    UINT8 track_mode;    //0���Զ�������1���رշ���
    UINT8 rec_bitrate;   //0��320kbps��1��256kbps������9��8kbps.(ͨ��BitRate_Tab[]ת����module��Ľӿڲ���)
    UINT8 rec_source;    //��ǰ���õ�¼��Դ��0��Line IN��1��SPDIF��2��MIC
    UINT8 rec_num;       //��ǰ¼���ļ��ı��
    file_location_t location;
}mrecord_vars_t;        //mrecord:ָMP3 ¼��.



/*�ر�¼��ͨ���Ľӿں���*/
void RECORD_CloseRecRoute(void);/*��ͬ�������ر�Ӳ��ͨ��*/
BOOL RECORD_ResetDir(file_location_t *location,WCHAR *DirName);
void RECORD_TotalNum(void);/*����¼�����ļ�����Ŀ*/

/*��ͬ¼��ģʽ����ں���*/
INT32 RECORD_Entry(INT32 param);//�ⲿ�ӿں���

INT32 record_op(INT32 status); 
INT32 record_read(VOID *buf, INT32 len); 

INT32 RECORD_Entry(INT32 param);

#endif  /* _AP_RECORD_H_*/



