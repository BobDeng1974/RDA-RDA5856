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

#ifndef _AP_MUSIC_H_
#define _AP_MUSIC_H_

#include "ap_gui.h"
#include "ap_media.h"


#define MUSIC_FREQ_GRAPH_W    (6)
#define MUSIC_FREQ_GRAPH_H    (6)

#if (APP_SUPPORT_RGBLCD==1)
#define MUSIC_FREQ_GRAPH_X    (18)
#define MUSIC_FREQ_GRAPH_Y    (100)

#define MUSCI_SCROLL_TXT_X    0
#define MUSCI_SCROLL_TXT_Y    80

#define MUSCI_CURRENT_TIME_X    36
#define MUSCI_CURRENT_TIME_Y    36

#define MUSCI_DEVICE_IMG_X    121
#define MUSCI_DEVICE_IMG_Y    0

#elif (APP_SUPPORT_LCD==1)
#define MUSIC_FREQ_GRAPH_X    (4)
#define MUSIC_FREQ_GRAPH_Y    (48)

#define MUSCI_SCROLL_TXT_X    0
#define MUSCI_SCROLL_TXT_Y    32


#define MUSCI_CURRENT_TIME_X    20
#define MUSCI_CURRENT_TIME_Y    16

#define MUSCI_DEVICE_IMG_X    97
#define MUSCI_DEVICE_IMG_Y    0

#endif


//���������ĵ��ڼ���  
#define MUSIC_MAX_VOLUME           AUD_MAX_LEVEL

//------------------------------
#define FollowReadTempShortName   "FLWWAV  TMP"
#define FollowReadTempLongName    L"FLWWAV.TMP"
#define scrolltime_def           260/20           //20ms * 13 = 260ms//����Ƶ�ʷ�Ƶϵ��
#define ABReplayMiniTime_def     400/20           //20ms * 10 = 200ms//AB��������С����
//������Щ��������ʱ��Ƚϳ�����ÿ�׸��һʱ�䶼�ȼ������ʱ�䣬�����������ֹͣ
//  ״̬�³���NEXT���ʱ�ٶȽ������ֲ��������·�������ֹͣ״̬��ѡ��һ�׸�ʱ
//  ��ʼʱ��Ϊ0��û�м����ʱ�䣩������ʱҪ������һ�ף���ղ����׸��ʱ��Ͳ�
//  ���㣬����ֱ��������һ�ף�������ȴ� 2���ӻ�δ�а�������ʼ���㵱ǰ������
//  ��ʱ�䲢��ʱ��ʾ�� 
#define CaltimeCNT_def           10                //��1��û�а����Ϳ�ʼ����ʱ��

#define UiRefreshTimeInterval           5                

#define RESULT_RE_INIT           (RESULT_USER1+1)//��ʼ��
#define RESULT_UI_STOP           (RESULT_USER1+2)// STOP ����
#define RESULT_UI_FOLLOW         (RESULT_USER1+3)// ����/��������
#define RESULT_UI_PLAY           (RESULT_USER1+4)// PLAY ����
#define RESULT_UI_EXIT           (RESULT_USER1+5)//�˳�

#define Music_Reset_Head         0
#define Music_Reset_End          1
#define Music_Cur                2//ѡ��ǰ
#define Music_Next               3//ѡ��һ��
#define Music_Prev               4//ѡ��һ��
#if APP_SUPPORT_CHANGE_FOLDER == 1
#define Music_NextFolder    5
#define Music_PrevFolder    6
#endif

//changeFolderStatus
#if APP_SUPPORT_CHANGE_FOLDER == 1
#define CHANGE_FOLDER_STATUS_NOKEY         0
#define CHANGE_FOLDER_STATUS_PREV           1
#define CHANGE_FOLDER_STATUS_NEXT           2
#endif

//g_DiskStateFlag mask 
#define SetDiskOKFlag_def         0x80
#define ClrDiskOKFlag_def         0x7f
#define SetDiskNoFile_def         0x40
#define ClrDiskNoFile_def         0xbf
#define SetCardOKFlag_def         0x08
#define ClrCardOKFlag_def         0xf7
#define SetCardNoFile_def         0x04
#define ClrCardNoFile_def         0xfb

#define BASE_SRS_WOW              0x40
#define eqcount_def               8
#define DEF_RecLowBatt            5//���������¼���ĵ�ѹ

#define  AUDPLY_LYRIC_MAX_LINE_SIZE   64   /* pre-defined max lyric buffer size for one line : bytes ; must be two bytes aligned */

//�ж��ǵ͵�Ĵ���������ѹֵ����DEF_RecLowBattֵ�����������ֵʱȷ��Ϊ�͵�
#define DEF_RecLowBatTime         5
#define ExproTimeDef              5//�������ʱֵ
#define IntCntValDef              5  // second

//���¶����˽ṹ�еĿ���λ
#define on_AllItem_def             0x80
#define on_PLAYCNT_def             0x20
#define on_BEGINDAT_def            0x10
#define on_EXPDATE_def             0x08
#define on_REMAINTM_def            0x04
#define on_REMAINST_def            0x02
#define on_DEALCLKBCK_def          0x01

#define DisplayItem0_def           0
#define DisplayItem1_def           1
#define DisplayItem2_def           2
#define DisplayItem3_def           3
#define DisplayItem4_def           4
#define DisplayItem5_def           5
#define DisplayItem6_def           6

#define RemainPlayCntDisp_def      DisplayItem0_def
#define OriginalPlayCntDisp_def    DisplayItem1_def
#define BeginDateTimeDisp_def      DisplayItem2_def
#define ExpDateTimeDisp_def        DisplayItem3_def
#define RemainTimeDisp_def         DisplayItem4_def
#define RemainStoreTimeDisp_def    DisplayItem5_def
#define DealOnClkBckDisp_def       DisplayItem6_def
#define lincensinf_maxitem_def     DisplayItem6_def


#define MUSIC_FFPLAY_STEP           7  // second
//#define MUSIC_ERROR_SKIP_STEP          2// 0.5second
#define MUSIC_INTRO_TIME             10 // second

#define MUSIC_MAX_TAGINFO           120


#define MAX_PROGRESS_CNT        (10000)


//===========================================================
//���涨��ṹ����
//===========================================================
/*for compiler compatible */
typedef struct
{
	UINT16 TimeMs;
	UINT8 TimeSecond;
	UINT8 TimeMinute;
	UINT8 TimeHour;
}Full_time_t;

typedef enum
{
    MusicStop,   // stop mode (0)
    NormalPlay,         //��ͨģʽ����ʾ(1)
    LoopPlay,         //��������(2)
    FollowRec,//����¼��(3)
    CompareAB,//�Աȷ�AB(4)
    ComparePlay//�ԱȲ���(5)
}Replay_status_t;

typedef enum
{
    Stop_key,
    Play_play,
    Play_key
}Music_sel_mod_t;

typedef enum
{               
    mplaylist,
    martists,
    malbum,
    mgenre,
    mcomposers,
    mlistnor
}play_listmod_t;

typedef struct
{
	UINT8 min;
	UINT8 sec;
	UINT8 p1s;
	UINT16 buffaddr;    
}LrcTime_t;

//AP_MUSICʹ�õ�ϵͳ����
typedef struct
{
    INT8 volume;               //       
    INT8 repeat_mode; //repeat �˵������  //+1
    INT8 loop_mode;
    eq_t eq_type;              //    
    //INT8 play_rate;     //�����ٶ�
    UINT8 replaytimes;          //��������
    UINT8 maxReplayTime;          //�������
    file_location_t location[FS_DEV_TYPE_MAX];  //
    UINT32 CurrentTime[FS_DEV_TYPE_MAX];    		//��ǰ��ʾ����ʱ��
    UINT8 Lyric_Size;
    BOOL show_lyric;
#if APP_SUPPORT_FM_OUT
    BOOL fm_out;
    UINT32 freq;  // fm output freq
#endif
    UINT16 magic;                // +0
} music_vars_t;

typedef struct
{
	//Current music screen status
	UINT8 screen_status;
	UINT8 DiskStateFlag;//b7 = 1:flash ���Ѹ�ʽ��
	                                //b6 = 1:flash �����ļ�
	                                //b3 = 1:���Ѹ�ʽ��
	                                //b2 = 1: �����ļ�
	                                //b5,b4,b1,b0 : ����
	                                //default = 0x00

	//---����������ֻ������ʱ����,��ҪĿ���ǽ�ʡָ�ʡ����ռ�,��Ϊ�ֲ������ϷѴ��뼰ָ��ʱ��
	BOOL isFileInfoGot;// = FALSE;//TRUE
	BOOL FFPlay_CNT; //�������
	BOOL Prevmusic_FLAG;

	UINT16 BitRate;
	UINT16 ERROR_EXIT;

	UINT32 timerid;
	UINT32 TotalTime ; //��ʱ��

	//EQ����������
	//static const INT8 EQVOLADJ_TAB[eqcount_def] = { 0,0,0,0,0,0,0,0};

	UINT8 TagInfoBuf[MUSIC_MAX_TAGINFO];//���ڷ�TAG��Ϣ��BUF

	//��ռ������ڴ�ʱ�������д��Ͳ���
	Open_param_t Openparam;

	Replay_status_t PlayStatus; //��ͨ����ģʽ

#if APP_SUPPORT_LYRIC==1//֧�ָ����ʾ����
	BOOL lyricflag;//���޸�ʱ�־,true��ʾ��
	INT8 lyric_buff[AUDPLY_LYRIC_MAX_LINE_SIZE];
	UINT32 lyric_next_time;
#endif

	Music_type_t currentMusicType;//��־��ǰ���ֵ�����(mp3/wma/asf/wav)
	UINT16 musicamount ;//��ǰĿ¼��������
	UINT16 musicsequence;//��ǰ�������

} music_screen_gui_t;

typedef struct
{
	Open_param_t rec_file;
	UINT32 StartTime;
	UINT16 StartPos;  //Start percent
	UINT16 EndPos;	  //End percent
	UINT8 loop_count;
	UINT8 rtc_clk_count;
	UINT8 rtc_clk_length;     // in half second
}FOLLOW_STATUS_INFO_T, *FOLLOW_STATUS_INFO_PTR;


//===========================================================

//--------- [UpDisplayFlag] ---Ҫ��ˢ�±�־
#define up_musictypeflag     0x8000
#define up_cardflag         0x4000
#define up_musicbitrate      0x2000
#define up_musicloopmode     0x1000
#define up_musicsequence     0x0800
#define up_musiceqmode       0x0400
#define up_musiccurrenttime  0x0200
#define up_musictotaltime    0x0100
#define up_musicbattery      0x0080
#define up_musicinformation  0x0040
#define up_musicscroll      0x0020
#define up_musiclyric       0x0010
#define up_musicfmout       0x0008
#define up_followmode          0x0004

//-----------------------------���ˢ�±�־
#define clr_musictypeflag    0x7fff
#define clr_lyricflag        0xbfff
#define clr_musicbitrate     0xdfff
#define clr_musicloopmode    0xefff
#define clr_musicsequence    0xf7ff
#define clr_MPRLmode         clr_musicsequence
#define clr_musiceqmode      0xfbff
#define clr_musiccurrenttime 0xfdff
#define clr_musictotaltime   0xfeff
#define clr_musicbattery     0xff7f
#define clr_musicinformation 0xffbf

#define MUSIC_REDRAW_MASK    0xFFFF

#define MUSIC_REDRAW_FULLSCREEN	(up_musictypeflag |    \
                  up_cardflag |        \
                  up_musicbitrate |     \
                  up_musicloopmode |    \
                  up_musicsequence |    \
                  up_musiceqmode |      \
                  up_musiccurrenttime | \
                  up_musictotaltime |   \
                  up_musicbattery |     \
                  up_musicinformation |  \
                  up_musicfmout \
                  )

#define StartIntro(mode)                  
/*\
    {                                         \
    g_introCntMode = mode;                    \
    / *g_OldSecTempBackup = g_music_vars.BreakPTDisTime.second;* / \
    g_introCNT = 0;                           \
    }
*/


//===================��ʷ���ʱ�������(��ʾ����ʱ������ܳ������ֵ)============עҪ�ñ�׼ʱ��д��
#define         MaxOneScreenTimeMin        0                   //����ָ������   ��0-59��                     //
#define         MaxOneScreenTimeSec        5                   //����ָ�������� ��0-59��                     //��ʾ5.5��
#define         MaxOneScreenTime100Ms      5                   //����ָ����100������ 9 ��ʾ 900ms��0-9��      //
//=======================================================================       



void MUSIC_GetPara( void );/*Read Para From Flash */

//---------------------------

//-------class display-------
void RefreshMainScreen(UINT16 flag);
//---------------------------

//-------class functions-----
BOOL ToNextMusic( Music_sel_mod_t mod );
BOOL PrevMusicInPlay(void);
BOOL MUSIC_GetInformation( void );
BOOL  MUSIC_SelectAudio( UINT8 Direct, Music_sel_mod_t mod );
BOOL MUSIC_CommonKeyHandler(UINT32 key);
INT32 ChangeVolume( UINT32 key );
INT32 UpdateFileSelector(void);
//------------------------------


#if APP_SUPPORT_LYRIC==1//֧�ָ����ʾ����

// load the lyric matching the music file specified by strMusicFile. position initialized.
// return the page mode , 0 means fail.
BOOL  lyricLoad( const UINT32 file_entry);

// Get the lyric by time, limited len, return next time
BOOL  lyricGetNext( INT8 * strLyric, UINT8 len, UINT32 *nextTime);

// Load lyric background
BOOL  lyricPreLoad(void);

#endif

#if APP_SUPPORT_FOLLOW_COMPARE==1
static Replay_status_t MUSIC_FollowStatusMachine(Replay_status_t old_status, BOOL start_rec);
#endif

UINT32 MUSIC_GetPlaySeconds(void);//Get Cur Play Time

void MUSIC_PlayGuardHandler(void *param);
/* ------�����ļ�tag���� ------*/
BOOL mp3GetID3Info(const UINT32 fileEntry, ID3Info_t *ID3Info, INT32 string_id);
BOOL oggGetTAGInfo(const TCHAR *filename, ID3Info_t *ID3Info, INT32 string_id);

INT32 MUSIC_Entry(INT32 param);

UINT16 MUSIC_EnterStatus(INT32 status);

BOOL MUSIC_ChangeFolder( UINT8 direct, Music_sel_mod_t mode );
#endif     /*_AP_MUSIC_H_*/



