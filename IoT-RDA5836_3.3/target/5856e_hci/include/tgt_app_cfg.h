/* 
* Copyright (c) 2006-2016 RDA Microelectronics, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef _TGT_APP_CFG_H_
#define _TGT_APP_CFG_H_

#define APP_SUPPORT_ONLY_ONE_LINK       0  //only support one link if 1
#define APP_SUPPORT_BLUETOOTH           1
#define APP_SUPPORT_RECORD              0
#define APP_SUPPORT_BT_RECORD           0
#define APP_SUPPORT_FM                  0
#define APP_SUPPORT_FM_RECORD           0
#define APP_SUPPORT_FM_OUT              0
#define APP_SUPPORT_LINEIN              0
#define APP_SUPPORT_FOLLOW_COMPARE      0
#define APP_SUPPORT_LYRIC               0
#define APP_SUPPORT_FADE_INOUT          0
#define APP_SUPPORT_INDEPEND_VOLUMNE    0
#define APP_SUPPORT_MUSIC               1
#define APP_SUPPORT_CHANGE_FOLDER       0
#define APP_SUPPORT_HOLD_NEXT_FOR_VOL          0
#define APP_SUPPORT_HOLD_VOL_FOR_NEXT          0
#define APP_SUPPORT_PBAP                0
#define APP_SUPPORT_MAP                 0
#define APP_SUPPORT_OPP                 0
#define APP_SUPPORT_SPP                 0
#define APP_SUPPORT_A2DP                0
#define APP_SUPPORT_AVRCP           	0
#define APP_SUPPORT_AVRCP_SYNC_VOL	    1
#define APP_SUPPORT_HFP                 0
#define APP_SUPPORT_HID                 0
#define APP_SUPPORT_LE                  0
#define APP_SUPPORT_BTBATTERY           0
#define APP_SUPPORT_BT_AUTOVISIBLE      0
#define APP_SUPPORT_BT_REMOTE_VOL       0
#define APP_SUPPORT_USBDEVICE           0
#define APP_SUPPORT_USBAUDIO            0
#define APP_SUPPORT_USBSTORAGE_BG       0
#define APP_SUPPORT_CALIB_KEY           0
#define APP_SUPPORT_NUMBER_VOICE        0
#define APP_SUPPORT_DELAY_VOLUME        0

#define APP_SUPPORT_LCD                 0
#define APP_SUPPORT_RGBLCD              0
#define APP_SUPPORT_LED                 1   // led��???��?
#define APP_SUPPORT_LED8S               0   // 8 ??��y??1��??��?
#define APP_SUPPORT_USB                 0
#define APP_SUPPORT_FLASHDISK           0
#define APP_SUPPORT_MENU                0
#define APP_SUPPORT_REMOTECONTROL       0
#define APP_SUPPORT_CHARGING            0 
#define APP_SUPPORT_TEST_BOX			0

#define APP_SUPPORT_MICA                            1
#define APP_SUPPORT_MICB                            0
#define APP_SUPPORT_DIF_DAC                       0
/**********************************************
* suppport lanaguages
**********************************************/
#define APP_SUPPORT_LANG_ENGLISH        0   //English
#define APP_SUPPORT_LANG_FRENCH         0    //French
#define APP_SUPPORT_LANG_ITALIAN        0    //Italian
#define APP_SUPPORT_LANG_VIETNAMESE     0    //Vietnamese
#define APP_SUPPORT_LANG_RUSSIAN        0    //Russian
#define APP_SUPPORT_LANG_ARABIC         0    //Arabic
#define APP_SUPPORT_LANG_THAI           0    //Thai
#define APP_SUPPORT_LANG_SM_CHINESE     1    //SM Chinese
#define APP_SUPPORT_LANG_TR_CHINESE     0    //TR Chinese
#define APP_SUPPORT_LANG_GERMAN         0    //German
#define APP_SUPPORT_LANG_SPANISH        0    //Spanish
#define APP_SUPPORT_LANG_PORTUGUESE     0    //Portuguese
#define APP_SUPPORT_LANG_PERSIAN        0    //Persian
#define APP_SUPPORT_LANG_TURKISH        0    //Turkish
#define APP_SUPPORT_LANG_INDONESIAN     0    //Indonesian
#define APP_SUPPORT_LANG_MALAY          0    //Malay
#define APP_SUPPORT_LANG_JAPANESE       0    //JAPANESE
#define APP_SUPPORT_LANG_KOREAN         0    //KOREAN
#define APP_SUPPORT_LANG_DANISH         0    //DANISH

#define APP_SUPPORT_INDIC_LANGUAGES     0    //
#define APP_SUPPORT_LANG_HINDI          0    //Hindi, no input method
#define APP_SUPPORT_LANG_TELUGU         0    //Telugu
#define APP_SUPPORT_LANG_PUNJABI        0    //Punjabi
#define APP_SUPPORT_LANG_BENGALI        0    //Bengali
#define APP_SUPPORT_LANG_MARATHI        0    //Marathi
#define APP_SUPPORT_LANG_ORIYA          0    //Malay
#define APP_SUPPORT_LANG_TAMIL          0    //Malay
#define APP_SUPPORT_LANG_KANNADA        0    //Malay
#define APP_SUPPORT_LANG_MALAYALAM      0    //Malay

/**********************************************
* LCD dimension 
**********************************************/
#define LCD_WIDTH               128
#define LCD_HEIGHT              64

#define APP_DEFAULT_RESULT      RESULT_BT   // ??��??�̨�3o������?��??DD��?��???

// copy from project_cfg.h end
// Partition count.
#define DSM_PART_COUNT   2
// =============================================================================
// TGT_DSM_PART_CONFIG
// -----------------------------------------------------------------------------
// This structure describes the DSM(Data Storage Mangage) configuration.
/// Field description:
/// szPartName: Partition name string,the max size is 15 bytes.
/// eDevType: can be either DSM_MEM_DEV_FLASH for onboard flash combo or DSM_MEM_DEV_TFLASH
/// eCheckLevel: VDS Module cheking level. 
//                        DSM_CHECK_LEVEL1: Check the PBD writing and PB Writing.
//                        DSM_CHECK_LEVEL2: Check the PDB writing only. 
//                        DSM_CHECK_LEVEL3: Not check.
/// uSecCnt: Number of sector used by this partition (when relevant)
/// uRsvBlkCnt: Number of reseved block. When want the write speed speedy, increase this field value.
/// eModuleId: Module identification.
//                    DSM_MODULE_FS_ROOT: FS Moudle for root directory.
//                    DSM_MODULE_FS:   FS Moudle for mounting device.
//                    DSM_MODULE_SMS: SMS_DM Module.
//                    DSM_MODULE_REG: REG Module
// =============================================================================

#define TGT_DSM_PART_CONFIG                                             \
{                                                                       \
/*   tianwq     {                                                                   \
        .szPartName  = "VDS0",                                          \
        .eDevType    = DSM_MEM_DEV_FLASH,                               \
        .eCheckLevel = DSM_CHECK_LEVEL_1,                               \
        .uSecCnt     = 8,                                               \
        .uRsvBlkCnt  = 1,                                             \
        .eModuleId   = DSM_MODULE_FS_ROOT                               \
    },                                                                  \
    {                                                                   \
        .szPartName  = "VDS1",                                          \
        .eDevType    = DSM_MEM_DEV_FLASH,                               \
        .eCheckLevel = DSM_CHECK_LEVEL_1,                               \
        .uSecCnt     = 10,                                               \
        .uRsvBlkCnt  = 1,                                             \
        .eModuleId   = DSM_MODULE_FS                                         \
    },													\
    {                                                                   \
        .szPartName  = "CSW",                                           \
        .eDevType    = DSM_MEM_DEV_FLASH,                               \
        .eCheckLevel = DSM_CHECK_LEVEL_1,                               \
        .uSecCnt     = 30,                                               \
        .uRsvBlkCnt  = 1,                                              \
        .eModuleId   = DSM_MODULE_CSW                                   \
    },  */                                                                \
    {                                                                   \
        .szPartName  = "MMC0",                                          \
        .eDevType    = DSM_MEM_DEV_TFLASH,                              \
        .eCheckLevel = DSM_CHECK_LEVEL_3,                               \
        .uSecCnt     = 0,                                               \
        .uRsvBlkCnt  = 0,                                               \
        .eModuleId   = DSM_MODULE_FS                                    \
    },                                                                   \
    {                                                                   \
        .szPartName  = "USB0",                                          \
        .eDevType    = DSM_MEM_DEV_USB,                              \
        .eCheckLevel = DSM_CHECK_LEVEL_3,                               \
        .uSecCnt     = 0,                                               \
        .uRsvBlkCnt  = 0,                                               \
        .eModuleId   = DSM_MODULE_FS                                    \
    }                                                                   \
}

#define TGT_DSM_CONFIG                                                  \
{                                                                       \
    .dsmPartitionInfo = TGT_DSM_PART_CONFIG ,                           \
    .dsmPartitionNumber = DSM_PART_COUNT                                \
}


// =============================================================================
// TGT_CSW_MEM_CONFIG
// -----------------------------------------------------------------------------
/// This structure describes the user heap size
/// cswHeapSize: Size of the heap available for csw
/// cosHeapSize: Size of the heap available for mmi
// =============================================================================
#define TGT_CSW_CONFIG                                                  \
{  																																			\
  .cswHeapSize = 350*1024,                                               \
  .cosHeapSize = 700*1024                                               \
}

// =============================================================================
// BT Stack config
// -----------------------------------------------------------------------------
/// This fills the TGT_BT_STACK_CONFIG structure for RDA5856
// =============================================================================
#ifndef TGT_BT_STACK_CONFIG

#if APP_SUPPORT_HID==1
#define	BT_DEVICE_OF_CLASS	0x2540
#else
#define	BT_DEVICE_OF_CLASS   0x240404
#endif

#ifdef USE_VOC_CVSD
#define	BT_VOICE_SETTING 0xc040
#else
#define	BT_VOICE_SETTING 0x8060
#endif

#if APP_SUPPORT_ONLY_ONE_LINK==1
#define BT_ACL_LINKS       1
#else
#define BT_ACL_LINKS       2
#endif


#define TGT_BT_STACK_CONFIG                                                  \
    {  																		\
       .host_ctrl_config =                                                              \
	{																				\
		.max_timers = 10,                                                           \
		.max_buff = 20,                                                                 \
		.max_message = 30,                                                           \
		.message_length = 512,                                                          \
		.host_mem_support = 0,                                                          \
		.host_mem_size = 256                                                            \
	},                                                                                              \
	.hci_config =                                                                           \
		{                                                                                       \
			.flow_control_support = 1,                                              \
			.flow_control_timeout = 1,                                              \
			.flow_control_threshold = 2,                                            \
			.max_acl_number = 7,                                                    \
			.max_acl_size  = 508,                                                       \
			.max_sco_number = 4,                                                    \
			.max_sco_size = 60,                                                         \
			.max_acl_queue_size = 10240,                                            \
		},                                                                                          \
	.manager_config =                                                                           \
		{                                                                                           \
			.max_acl_links = BT_ACL_LINKS,                                                             \
			.max_device_list = 3,                                                          \
			.multi_connect_support = 1,                                                 \
			.device_class = BT_DEVICE_OF_CLASS,											\
			.security_support = 1,                                                      \
			.sco_packets = 0x4c,                                                        \
			.voice_setting = BT_VOICE_SETTING,                                                \
			.sco_via_hci = 0,                                                           \
			.ssp_support = 1,                                                           \
			.io_capability = 3,                                                          \
			.sniff_timeout = 100,                                                       \
			.sniff_interval = 0x320,                                                    \
			.unsniff_packets = 0,                                                       \
			.linkto_support = 0,                                                        \
			.linkto_value = 20000,                                                       \
			.auto_repair_support = 0,                                                    \
			.default_le_mode = 0,                                                    \
            .support_soundbar = 0,                                                      \
            .support_switch_role = 1,                                                  \
            .support_all_event_report=0,                                                \
    },                                                                                          \
	.l2cap_config =                                                                             \
	{                                                                                               \
		.auto_config = 1,                                                                   \
		.fragment_acl_support = 1,                                                      \
		.config_to_core = 0,                                                                \
		.max_channel = 10,                                                              \
		.max_links = 6,                                                                     \
		.max_in_mtu = 500,                                                                  \
		.max_out_mtu = 500,                                                                 \
		.use_piconet_broadcast = 0,                                                     \
		.max_broadcast_mtu = 0x04ED,                                                \
		.broadcast_handle = 0xef1,                                                      \
		.extended_features = 0                                                              \
	},                                                                                              \
	.sdp_config =                                                                               \
	{                                                                                               \
		.mtu = 0x80,                                                                            \
	},                                                                                                  \
	.obex_config =                                                                          \
	{                                                                                               \
		.local_mtu = 4096,                                                              \
        .server_transport = 3,                                                        \
		.pbap_buf_size = 700,																													\
	},                                                                                              \
	.rfcomm_config =                                                                    \
	{                                                                                           \
		.init_credit = 7,                                                                   \
		.max_credit = 255,                                                              \
		.min_credit = 3,                                                                    \
		.credit_issue = 7,                                                                  \
		.max_frame_size = 128                                                       \
	},                                                                                          \
	.hfp_config =                                                                          \
	{                                                                                           \
		.hf_feature = HF_FEATURE_CLIP|HF_FEATURE_REMOTE_VOL|HF_FEATURE_NREC_FUNCTION, \
		.ag_feature = AG_FEATURE_IN_BAND_RING,                          \
		.handle_at_cmd = 1                                                          \
	},                                                                                          \
	.a2dp_config =                                                                      \
	{                                                                                           \
		.media_mtu = 500,                                                           \
		.signal_mtu = 48,                                                           \
		.accetp_connect = 1,                                                        \
	},                                                                                          \
	.spp_config =                                                                       \
       {                                                                                        \
        .max_ports = 6,                                                                     \
        .server_num = 2,                                                                    \
        .max_data_len = 128                                                                 \
       },                                                                                           \
}                                                                                                   \

#endif // TGT_BT_STACK_CONFIG
// =============================================================================
// TGT_UCTLS_CONFIG
// -----------------------------------------------------------------------------
/// Default List of services
// =============================================================================
//#include "uctls_tgt_params.h"


#endif //_TGT_APP_CFG_H_
