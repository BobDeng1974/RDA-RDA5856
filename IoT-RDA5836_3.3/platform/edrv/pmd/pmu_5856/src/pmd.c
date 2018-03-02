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


////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file pmd.c  
/// This file contain the PMD driver code related to the eco mode.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include "pmd_config.h"
#include "pmd_m.h"
#include "pmd_map.h"
//#include "pmd_calib.h"
#include "pmdp_debug.h"
#include "pmdp_charger.h"
#include "pmdp.h"
//#include "drv_dualsim.h"
//#include "rfd_xcv.h"

// Opal hardware
#include "opal.h"

// hal drivers
#include "hal_pwm.h"
#include "hal_ispi.h"
#include "hal_lps.h"
#include "hal_timers.h"
#include "hal_sys.h"
#include "hal_i2c.h"
#include "hal_rfspi.h"

#include "hal_rda_abb.h"
#include "hal_rda_audio.h"
#include "hal_error.h"
#include "hal_host.h"

// sx timers
#include "sxr_tls.h"
#include "sxr_tim.h"
#include "cos.h"
#include "event.h"
#include "tgt_board_cfg.h"
#include "tsd_config.h"

extern UINT32 key_enterInterrupt;
#ifdef VMIC_POWER_ON_WORKAROUND
extern VOID aud_InitEarpieceSetting(VOID);
extern VOID aud_EnableEarpieceSetting(BOOL enable);
#endif // VMIC_POWER_ON_WORKAROUND

extern VOID hal_AudForceDcdc4ClockOn(BOOL on);

#ifdef __PRJ_WITH_SPIFLSH__
extern VOID memd_FlashWakeup(VOID);
extern VOID memd_FlashSleep(VOID);
#endif

extern VOID hal_ConfigFmIo(BOOL enable);
extern VOID hal_ConfigBtIo(BOOL enable);

#ifdef FM_LDO_WORKAROUND
extern VOID rfd_XcvSetDcdcLdoMode(BOOL dcdcOn, BOOL ldoOn);
#endif
extern HAL_ANA_GPADC_MV_T hal_AnaGpadcGpadc2Volt(UINT16 gpadcVal);
extern PUBLIC VOID hal_AudEnableCodecAuxPll(BOOL on);

// =============================================================================
//  DEFINES
// =============================================================================

#if defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER)
#undef SPI_REG_DEBUG
#endif

#define PMD_FUNC_INTERNAL   // __attribute__((section(".sramtext")))
#define PMD_DATA_INTERNAL   __attribute__((section(".sramdata")))

#define ENABLE_KEYPAD_LIGHT_PWM 0
#define KEYPAD_LIGHT_PWM_BY_PMU 1

// Whether to enable charge pump high output for speaker
#define AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT 1

#define PMD_TIME_IRQ_ACT_CS_RETRY (2 MS_WAITING)
#define PMD_TIME_IRQ_DEBOUNCE (100 MS_WAITING)

#define PMD_IRQ_DEBOUNCE_REQ (1<<0)
#define PMD_IRQ_CHECK_CHARGER (1<<1)
#define PMD_IRQ_CHECK_HP (1<<2)

#define POWER_LDO_USER(id) (1<<id)
#define LEVEL_LDO_USER(id) (1<<(id+PMD_POWER_ID_QTY))
#define DEFAULT_LDO_USER   (1<<31)

#define PMD_TIME_HP_DETECT_STABLE (1200 MS_WAITING)

#define TSD_CONFIG_STRUCT_T TSD_CONFIG_OPAL_STRUCT_T

#if (ENABLE_KEYPAD_LIGHT_PWM)
#warning "ENABLE_KEYPAD_LIGHT_PWM might interfere with speaker PA"
#endif
#define CONFIG_CLASS_K_SPEAKER_PA 0

#define DYNAMIC_DCDC_VCORE_ADJUSTMENT 1

// Whether left speaker line is in class AB mode
// If 0, the line is in class D or K mode
#ifndef AUD_SPK_LINE_L_CLASS_AB
#define AUD_SPK_LINE_L_CLASS_AB 0
#endif

// Whether right speaker line is in class AB mode
// If 0, the line is in class D or K mode
#ifndef AUD_SPK_LINE_R_CLASS_AB
#define AUD_SPK_LINE_R_CLASS_AB 0
#endif

// Whether to disallow the speaker lines to be in class K mode
// If the speaker lines are in class AB mode, this macro
// is meaningless; otherwise, the lines are in class D mode
// if 1, or in class K mode if 0.
#ifndef AUD_SPK_LINE_NO_CLASS_K
#define AUD_SPK_LINE_NO_CLASS_K 0
#endif

// Whether to enable speaker L-R-double mode (the right speaker line is
// directly connected to the left one to increase drive capability)
#if defined(RECV_USE_SPK_LINE) || \
    (AUD_SPK_LINE_L_CLASS_AB != AUD_SPK_LINE_R_CLASS_AB)
#if defined(AUD_SPK_LINE_L_R_DOUBLE) && (AUD_SPK_LINE_L_R_DOUBLE != 0)
#error "Speaker line L-R-double mode cannot be enabled"
#endif
#define AUD_SPK_LINE_L_R_DOUBLE 0
#else // L/R modes are different
#ifndef AUD_SPK_LINE_L_R_DOUBLE
#define AUD_SPK_LINE_L_R_DOUBLE 1
#endif
#endif // L/R modes are differt

// Whether to enable charge pump for LCD backlight
#ifndef LCD_BACKLIGHT_CHARGE_PUMP
#define LCD_BACKLIGHT_CHARGE_PUMP 0
#endif

#define DEBOUNCE_TIME 5
#define _USED_TSC_KEY_SPI_

#if (LCD_BACKLIGHT_CHARGE_PUMP) && \
    (AUD_SPK_LINE_L_CLASS_AB || AUD_SPK_LINE_R_CLASS_AB || AUD_SPK_LINE_NO_CLASS_K)
#error "LCD backlight charge pump requires speaker to be in class K mode"
#endif


// Forward declarations
PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData);
PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data);
PUBLIC VOID Pmd_IrqTimerTimeOut(UINT16 key_Data);

//
// =============================================================================
//  TYPES
// =============================================================================

// =============================================================================
// PMD_LDO_SETTINGS_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct {
        BOOL fmEnable:1;
        BOOL btEnable:1;
        BOOL keyEnable:1;
        BOOL tscEnable:1;
        BOOL vLcdEnable:1;
        BOOL vIO1P8Enable:1;
        BOOL vMicEnable:1;
        BOOL vIbrEnable:1;
        UINT16 :1;
        BOOL vAswEnable:1;
        BOOL vAbbEnable:1;
        BOOL vMmcEnable:1;
        BOOL vSim4Enable:1;
        BOOL vSim3Enable:1;
        BOOL vSim2Enable:1;
        BOOL vSim1Enable:1;
    };
    UINT16 reg;
} PMD_LDO_SETTINGS_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING1_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct {
        BOOL normalMode:1;
        BOOL vSpiMemAct:1;
        // The hw bit for vBlLedOff is useless.
        // Sw should set rgbLedOff bit in LDO setting2 register instead.
        BOOL vBlLedAct:1;
        BOOL vMicAct:1;
        BOOL vUsbAct:1;
        BOOL vIbrAct:1;
        BOOL vMmcAct:1;
        BOOL vLcdAct:1;//bit 7
        BOOL vIO1P8Act:1;
        BOOL vAswAct:1;
        BOOL vAnaAct:1;
        BOOL vIO2P8Act:1;
        BOOL vMemAct:1;
        BOOL buck1LdoAct:1;
        BOOL buck1PfmOn:1;
        BOOL buck1On:1;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING1_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING2_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct {
        UINT16 vResBit:2;
        BOOL buck2LdoOff:1;
        BOOL buck2PfmOn:1;
        BOOL buck2On:1;
        BOOL vBoost:1;
        BOOL vRf28:1;
        BOOL vIbr_sel:1;// bit 7
        BOOL vMmc_sel:1;
        BOOL vLcd_sel:1;
        BOOL vIo1p8_sel:1;
        BOOL vAsw_sel:1;
        BOOL vIo2p8_sel:1;
        UINT16 vRtcVbit:3;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING2_T;

// =============================================================================
// PMD_LDO_PROFILE_SETTING5_T
// -----------------------------------------------------------------------------
/// Profile register mapping, directly related to hardware
// =============================================================================
typedef union
{
    struct {
        // NOTE:
        //   vRgbLed & vAbb bits are located in RDA_ADDR_LDO_ACTIVE_SETTING5
        //   only, and they are valid for all profiles
        UINT16 vRgbLedIbit:3;
        UINT16 vRgbLedVsel:3;
        UINT16 vAbbIbit:3;
        // End of NOTE
        UINT16 vUsbIbit:3;
        BOOL vACcDisable:1;
        BOOL vUsbCcDisable:1;
        BOOL vSim2Is1_8:1;
        BOOL vSim1Is1_8:1;
    };
    UINT16 reg;
} PMD_LDO_PROFILE_SETTING5_T;

// =============================================================================
// PMD_BL_LED_OUT_SETTING_T
// -----------------------------------------------------------------------------
/// BL_LED_OUT setting
// =============================================================================
typedef struct
{
    UINT8 vRgbLedIbit:3;
    UINT8 vRgbLedVsel:3;
} PMD_BL_LED_OUT_SETTING_T;


// ============================================================================
//  GLOBAL VARIABLES
// ============================================================================

// Whether charger in IRQ is enabled
PROTECTED BOOL g_pmdChargerInIrqEnabled = TRUE;

#ifdef ABB_HP_DETECT
// Whether headphone out IRQ is enabled
PRIVATE BOOL g_pmdHpOutIrqEnabled = FALSE;

// Whether headphone out detect is started
PRIVATE BOOL g_pmdHpOutDetectStarted = FALSE;
#endif

//  RDA_ADDR_LDO_SETTINGS = 0x02
PRIVATE PMD_LDO_SETTINGS_T g_pmdLdoSettings = 
{{
    .fmEnable = FALSE,
    .btEnable = FALSE,
    .keyEnable = TRUE,
    .tscEnable = FALSE,//
    .vLcdEnable = TRUE,
    .vIO1P8Enable = TRUE,
    .vMicEnable = TRUE,//
    .vIbrEnable = TRUE,//
    .vAswEnable = TRUE,
    .vAbbEnable = TRUE,
    .vMmcEnable = TRUE,
    .vSim4Enable = FALSE,
    .vSim3Enable = FALSE,
    .vSim2Enable = FALSE,
    .vSim1Enable = FALSE,
}};

//  RDA_ADDR_LDO_ACTIVE_SETTING1 = 0x03
//  RDA_ADDR_LDO_LP_SETTING1 = 0x08
PRIVATE PMD_LDO_PROFILE_SETTING1_T g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_QTY] =
{
    {{  // PMD_PROFILE_MODE_NORMAL
        .normalMode = TRUE,
        .vSpiMemAct = TRUE,
        // The hw bit for vBlLedOff is useless.
        // Sw should set rgbLedOff bit in LDO setting2 register instead.
        .vBlLedAct = FALSE,// bit 2
        .vMicAct = TRUE,// bit 3
        // vMem is derived from vUsb, so vUsb should be always on
        .vUsbAct = TRUE,// bit 4
        .vIbrAct = TRUE,// bit 5
        .vMmcAct = TRUE,// bit 6
        .vLcdAct = TRUE,// bit 7
        .vIO1P8Act = TRUE,// bit 8
        .vAswAct = FALSE,// bit 9
        .vAnaAct = TRUE,// bit 10
        .vIO2P8Act = TRUE,// bit 11
        .vMemAct = TRUE,// bit 12
        .buck1LdoAct = TRUE,
        .buck1PfmOn = FALSE,
        .buck1On = TRUE,
    }},
    {{  // PMD_PROFILE_MODE_LOWPOWER
        .normalMode = FALSE,
        .vSpiMemAct = TRUE,
        // The hw bit for vBlLedOff is useless.
        // Sw should set rgbLedOff bit in LDO setting2 register instead.
        .vBlLedAct = FALSE,
        .vMicAct = FALSE,
        // vMem is derived from vUsb, so vUsb should be always on
        .vUsbAct = TRUE,
        .vIbrAct = FALSE,
        .vMmcAct = TRUE,
        .vLcdAct = FALSE,
        .vIO1P8Act = FALSE,
        .vAswAct = FALSE,
        .vAnaAct = FALSE,
        .vIO2P8Act = TRUE,
        .vMemAct = TRUE,
        .buck1LdoAct = TRUE,
        .buck1PfmOn = TRUE,
        .buck1On = TRUE,
    }},
};


//  RDA_ADDR_LDO_ACTIVE_SETTING2 = 0x04
//  RDA_ADDR_LDO_LP_SETTING2 = 0x09
PRIVATE PMD_LDO_PROFILE_SETTING2_T g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_QTY] =
{
    {{  // PMD_PROFILE_MODE_NORMAL
        .vResBit = 2,
        .buck2LdoOff = TRUE,
        .buck2PfmOn = FALSE,
        .buck2On = FALSE,
        .vBoost = FALSE,
        .vRf28 = TRUE,
        .vIbr_sel = FALSE,
        .vMmc_sel = TRUE,
        .vLcd_sel = TRUE,
        .vIo1p8_sel = TRUE,
        .vAsw_sel = FALSE,
        .vIo2p8_sel = FALSE,
        .vRtcVbit = 4,
    }},
    {{  // PMD_PROFILE_MODE_LOWPOWER
        .vResBit = 2,
        .buck2LdoOff = TRUE,
        .buck2PfmOn = FALSE,
        .buck2On = FALSE,
        .vBoost = FALSE,
        .vRf28 = TRUE,
        .vIbr_sel = TRUE,
        .vMmc_sel = TRUE,
        .vLcd_sel = TRUE,
        .vIo1p8_sel = TRUE,
        .vAsw_sel = FALSE,
        .vIo2p8_sel = FALSE,
        .vRtcVbit = 4,
    }},
};

//  RDA_ADDR_LDO_ACTIVE_SETTING3 = 0x05
//  RDA_ADDR_LDO_LP_SETTING3 = 0x0a
PRIVATE UINT16 g_pmdLdoProfileSetting3 =
                    RDA_PMU_VBACKUP_VBIT(4)|
                    RDA_PMU_VSPIMEM_IBIT(7)|
                    RDA_PMU_VMEM_IBIT(4)|
                    RDA_PMU_VIO2P8_IBIT(4)|
                    RDA_PMU_PU_RES_IBIT(4);//New added in 5855

//  RDA_ADDR_LDO_ACTIVE_SETTING4 = 0x06
PRIVATE UINT16 g_pmdLdoActiveSetting4 =
                    /*RDA_PMU_VRF_IBIT(4)|
                    RDA_PMU_VCAM_IBIT(4)|*/
                    RDA_PMU_VASW_IBIT(4)|//New added in 5855
                    RDA_PMU_VIO1P8_IBIT(4)|//New added in 5855
                    RDA_PMU_VLCD_IBIT(4)|
                    RDA_PMU_VMMC_IBIT(4)|
                    RDA_PMU_VIBR_IBIT(4);

//  RDA_ADDR_LDO_ACTIVE_SETTING5 = 0x07
//  RDA_ADDR_LDO_LP_SETTING5 = 0x0c
#if 0
PRIVATE PMD_LDO_PROFILE_SETTING5_T g_pmdLdoProfileSetting5 =
{{
    // NOTE:
    //   vRgbLed & vAbb bits are located in RDA_ADDR_LDO_ACTIVE_SETTING5
    //   only, and they are valid for all profiles
    .vRgbLedIbit = 4,
    .vRgbLedVsel = 4,
#ifdef ABB_HP_DETECT
    // Workaround:
    // Headphone detection will encounter errors if vAbbIbit>=3
    .vAbbIbit = 2,
#else
    .vAbbIbit = 4,
#endif
    // End of NOTE
    #ifdef Low_Power_Consumption
    .vUsbIbit = 0,//4,
    #else
    .vUsbIbit = 4,//4
    #endif
    .vACcDisable = TRUE,//New added in 5855
    .vUsbCcDisable = TRUE,//New added in 5855
    .vSim2Is1_8 = TRUE,
    .vSim1Is1_8 = TRUE,
}};
#endif
// VCORE values
PRIVATE UINT8 g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreValueLp;

PRIVATE UINT8 g_pmdDcdcVcoreActRegistry[PMD_CORE_VOLTAGE_USER_ID_QTY];
PRIVATE UINT8 g_pmdDcdcVcoreActCurIndex;

PRIVATE UINT16 g_pmdLdoUserBitmap[PMD_PROFILE_MODE_QTY][PMD_LDO_QTY];

// RDA_ADDR_CALIBRATION_SETTING2 = 0x17
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdCalibSetting2 = 
                    RDA_PMU_TS_I_CTRL_BATTERY(8);

// RDA_ADDR_MISC_CONTROL = 0x18
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdMiscControlActive = 
                    RDA_PMU_CLK2M_FTUN_BIT(4)|
                    RDA_PMU_LDO_AVDD3_BIT(4);

PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdMiscControlLp = 
                    RDA_PMU_PU_CLK_4M_DR|
                    RDA_PMU_PD_LDO_AVDD3_DR|
                    RDA_PMU_PD_LDO_AVDD3_REG|
                    RDA_PMU_CLK2M_FTUN_BIT(4)|
                    RDA_PMU_LDO_AVDD3_BIT(4);

// RDA_ADDR_LED_SETTING1 = 0x19
// Disable backlight PWM (0)
#if 0
PRIVATE UINT16 g_pmdLedSetting1 = 
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
// keypad light PWM controlled by PMU
                    RDA_PMU_PWM_RGB_PMU_MODE|
                    RDA_PMU_PWM_RGB_FREQ(8)|
#else
// keypad light PWM controlled by baseband
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
// keypad light always on (0)
                    RDA_PMU_DIM_LED_R_DR|
                    RDA_PMU_DIM_LED_G_DR|
                    RDA_PMU_DIM_LED_B_DR|
#endif // !ENABLE_KEYPAD_LIGHT_PWM
// direct LCD backlight control, LCD backlight on (0)
                    RDA_PMU_DIM_BL_DR;
#endif
// RDA_ADDR_LED_SETTING2 = 0x1a
PRIVATE UINT16 g_pmdLedSetting2 = 
                    RDA_PMU_BL_IBIT_PON(8)|
                    RDA_PMU_BL_IBIT_ACT(8)|
                    RDA_PMU_BL_IBIT_LP(8)|
                    RDA_PMU_BL_OFF_PON|
                    RDA_PMU_BL_OFF_ACT|
                    RDA_PMU_BL_OFF_LP;

// RDA_ADDR_LED_SETTING3 = 0x1b
//PRIVATE UINT16 g_pmdLedSetting3 = 
//                    RDA_PMU_PWM_BL_DUTY_CYCLE(16)|
//                    RDA_PMU_PWM_BL_FREQ(8);

// ABB SIM_INTERFACE (ABB 0x01)
//PRIVATE UINT16 g_pmdSimSelect = 0;

// RDA_ADDR_SPEAKER_PA_SETTING1 = 0x27
// CP (charge pump) control configuration
// CP direct-reg controls will be initialized in pmd_SpeakerPaRestoreMode
PRIVATE UINT16 g_pmdSpeakerPaSetting1 = 
                    RDA_PMU_DELAY_BIT_CP(3)|//????
                    RDA_PMU_VCP_OUT_BIT(0);

// RDA_ADDR_LDO_BUCK1 = 0x2d
PRIVATE CONST UINT16 gc_pmdLdoBuck1 =
                    RDA_PMU_VBUCK_LDO_BIT(4)|
                    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
                    RDA_PMU_PFM_THRESHOLD(2)|
                    RDA_PMU_OSC_FREQ(0)|
                    RDA_PMU_LOW_SENSE|
                    RDA_PMU_DISCHARGE_EN;

// RDA_ADDR_LDO_BUCK2 = 0x2e
PRIVATE CONST UINT16 gc_pmdLdoBuck2 =
                    #ifdef Low_Power_Consumption
                    RDA_PMU_VBUCK_LDO_BIT(0)|  // old 5
                    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
                    RDA_PMU_PFM_THRESHOLD(2)|
                    RDA_PMU_OSC_FREQ(0)|
                    RDA_PMU_LOW_SENSE|
                    RDA_PMU_DISCHARGE_EN|
                    RDA_PMU_HEAVY_LOAD;//????
                    #else
                    RDA_PMU_VBUCK_LDO_BIT(5)|  // old 5
                    RDA_PMU_PFM_CLK_PRD_BUCK(2)|
                    RDA_PMU_PFM_THRESHOLD(2)|
                    RDA_PMU_OSC_FREQ(0)|
                    RDA_PMU_LOW_SENSE|
                    RDA_PMU_DISCHARGE_EN|
                    RDA_PMU_HEAVY_LOAD;//???? 
                    #endif

// RDA_ADDR_DCDC_BUCK = 0x2f
// TODO: BT requires VBUCK1_NLP/LP=8,VBUCK2_NLP/LP=9 on U02 chips.
//       This might be skipped for U04 or later chips.
PRIVATE UINT16 PMD_DATA_INTERNAL g_pmdDcdcBuck =
                    RDA_PMU_VBUCK1_BIT_NLP(5)|
                    RDA_PMU_VBUCK1_BIT_LP(0)|
                    RDA_PMU_VBUCK2_BIT_NLP(10)|
                    RDA_PMU_VBUCK2_BIT_LP(0);

// RDA_ADDR_THERMAL_CALIBRATION = 0x36
PRIVATE UINT16 g_pmdThermalCalib = 
    RDA_PMU_CHR_CALMODE_EN_BG_REG|
    RDA_PMU_CHR_CALMODE_EN_NONCAL|
    RDA_PMU_CALMODE_EN_BG_CAL|
    RDA_PMU_THERMO_SEL(3)|
    RDA_PMU_PU_THERMO_PROTECT|
    RDA_PMU_PD_VD_LDO;

// RDA_ADDR_DEBUG3 = 0x37
PRIVATE UINT16 g_pmdDebug3 = 
                    RDA_PMU_OVER_TEMP_BYPASS|
                    RDA_PMU_RESETN_BT_TIME_SEL(2)|
                    RDA_PMU_RESETN_FM_TIME_SEL(2);

#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
// RDA_ADDR_LED_SETTING4 = 0x38
PRIVATE UINT16 g_pmdLedSetting4;
#endif
#endif

// RDA_ADDR_GPADC2 = 0x3b
PRIVATE UINT16 g_pmdGpadc2 = 
    RDA_PMU_GPADC_START_TIME(4)|
    RDA_PMU_GPADC_VREF_BIT(2);

// RDA_ADDR_LED_SETTING5 = 0x3e
PRIVATE UINT16 g_pmdLedSetting5 = 
                    RDA_PMU_GBIT_ABB_EN|
                    RDA_PMU_LED_R_OFF_ACT|
                    RDA_PMU_LED_G_OFF_ACT|
                    RDA_PMU_LED_B_OFF_ACT|
                    RDA_PMU_LED_R_OFF_LP|
                    RDA_PMU_LED_G_OFF_LP|
                    RDA_PMU_LED_B_OFF_LP|
                    RDA_PMU_LED_R_IBIT(0)|
                    RDA_PMU_LED_G_IBIT(0)|
                    RDA_PMU_LED_B_IBIT(0);

// RDA_ADDR_SPEAKER_PA_SETTING2 = 0x40
PRIVATE CONST UINT16 gc_pmdSpeakerPaSetting2 = 
                    RDA_PMU_VCLG_BIT(2)|
                    RDA_PMU_VNCN_BIT(2)|
                    RDA_PMU_I_BIT_CLG_REG(2);

// RDA_ADDR_SPEAKER_PA_SETTING3 = 0x41
PRIVATE UINT16 g_pmdSpeakerPaSetting3 = 
                    RDA_PMU_SETUP_TMCTRL_BIT(0)|
                    RDA_PMU_RESET_CLG_DELAY_NEG(1)|
                    RDA_PMU_RESET_CLG_DELAY_POS(1);

/// Configuration structure for TARGET
PROTECTED CONST PMD_CONFIG_T* g_pmdConfig;

/// Variable used to store the default values of the PMD calibration.
/// This is not used in the CT1010, be used by Opal.
//PRIVATE CONST CALIB_PMD_T g_pmdCalibDefault; // No use any more

/// To avoid SPI deactivation/activation during Open
PRIVATE BOOL g_pmdInitDone = FALSE;

/// To track activation status
PRIVATE VOLATILE UINT8 g_pmdActivated = 0;
PRIVATE VOLATILE UINT8 g_tsdActivated = 0;
//PRIVATE VOLATILE UINT8 g_SimActivated = 0;

// Workaround to disable PMU IRQ without blocking in PMU IRQ handler
 UINT32 g_pmdIrqSettingLatest = 0;

// Workaround to disable HP-Detect IRQ without blocking in PMU IRQ handler
PRIVATE UINT32 g_pmdHpDetectSettingLatest = 0;

// Clk4m and Avdd3 enabling registry in LP mode
PRIVATE BOOL g_pmdClk4mAvdd3EnRegistryLp[PMD_CLK4M_AVDD3_USER_ID_QTY];

PRIVATE BOOL g_pmdBacklightOn = TRUE;

PRIVATE BOOL g_pmdScreenAntiFlickerEnabled = FALSE;

#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
PRIVATE PMD_BL_LED_OUT_SETTING_T g_pmdBlLedOutDefault;

PRIVATE CONST PMD_BL_LED_OUT_SETTING_T gc_pmdBlLedOutAntiFlicker =
{
    .vRgbLedIbit = 0,
    .vRgbLedVsel = 5,
};
#endif

PRIVATE BOOL g_pmdAudExtPaExists = FALSE;

PRIVATE BOOL g_pmdSpeakerPaEnabled = FALSE;

PRIVATE BOOL g_pmdSpeakerPaCpLowOutput = FALSE;

#ifdef ABB_HP_DETECT
PRIVATE PMD_EAR_HANDLER_T ear_handler = NULL;

PRIVATE HAL_GPIO_GPIO_ID_T g_ear_gpio = HAL_GPIO_NONE;
#endif

PRIVATE BOOL g_pmdFmInLpModeEnabled = FALSE;

#ifdef FM_LDO_WORKAROUND
PRIVATE BOOL g_pmdLdoReqFlag = FALSE;

PRIVATE BOOL g_pmdHighVcoreReqFlag = FALSE;

PRIVATE BOOL g_pmdLdoModeEnabled = FALSE;
#endif

//PRIVATE UINT8 Pmd_IrqTimer = 0;
PRIVATE CONST TSD_CONFIG_T*  g_tsdConfig;
HAL_ERR_T error;
BOOL i2c_flag = 0;

// ============================================================================
// Forward declarations
// ============================================================================
PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData);
PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data);
//PRIVATE VOID pmd_EarpDetectGpioInit(HAL_GPIO_GPIO_ID_T gpioId);
#if !(defined(_FLASH_PROGRAMMER) || defined(_T_UPGRADE_PROGRAMMER))
PRIVATE VOID pmd_SpeakerPaSetCpLevel(UINT32 level);
#endif
PUBLIC VOID pmd_SetPadVoltage(UINT8 level);
VOID tsd_Debounce(VOID);
// ============================================================================
//  PRIVATE FUNCTIONS
// ============================================================================

//=============================================================================
// pmd_OpalCsBlockingActivate
//-----------------------------------------------------------------------------
/// Activate the SPI bus for Opal chip.
/// This function will poll until the CS is activated
//=============================================================================
PUBLIC VOID pmd_OpalCsBlockingActivate(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return;
    }

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalCsBlockingActivate);
    
    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    while(FALSE == status)
    {
        if (1 == g_pmdActivated || 1 == g_tsdActivated)
        {
            status = FALSE;
            if (1 == g_pmdActivated)
            {
                PMD_TRACE(PMD_INFO_TRC, 0, "PMU has been activated. Be care of infinite loop.");
            }
        }
        else
        {
            status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
            if (status)
            {
                g_pmdActivated = 1;
            }
        }
        
        if(FALSE == status)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, " pmd_OpalCsBlockingActivate, Sleeping...");
            COS_SleepByTick(HAL_TICK1S / 10);
        }
    }
    hal_SysExitCriticalSection(scStatus);
    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalCsBlockingActivate);   
}

//=============================================================================
// pmd_OpalCsDeactivate
//-----------------------------------------------------------------------------
/// Activate the SPI bus for Opal chip.
/// This function will poll until the CS is activated
//=============================================================================
PUBLIC VOID pmd_OpalCsDeactivate(BOOL singleWrite)
{
     // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return;
    }

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalCsDeactivate);
    UINT32 scStatus = hal_SysEnterCriticalSection();
    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    g_pmdActivated = 0;
    hal_SysExitCriticalSection(scStatus);
    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalCsDeactivate);
}


//=============================================================================
// pmd_OpalSpiActivateCs called  by TSD moudle
//-----------------------------------------------------------------------------
/// Activate Opal SPI. refer to hal_SpiActivateCs() for details.
/// @return \c TRUE if the cs has been successfully activated
/// \c FALSE when another cs is already active
// =============================================================================
PUBLIC BOOL pmd_OpalSpiActivateCs(VOID)  //called  by TSD moudle
{
  //  PMD_ASSERT(g_pmdConfig != NULL, "g_pmdConfig must be not NULL.");//deducted for 5855

    PMD_PROFILE_FUNCTION_ENTER(pmd_OpalSpiActivateCs);
    
    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated)
    {
        status = FALSE;
        if (1 == g_tsdActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU-TS has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_tsdActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    PMD_PROFILE_FUNCTION_EXIT(pmd_OpalSpiActivateCs);

    return status;
}

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PUBLIC VOID pmd_OpalSpiDeactivateCs(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    g_tsdActivated  = 0;
    hal_SysExitCriticalSection(scStatus);
    
}

#if 0
PROTECTED BOOL pmd_SIMSpiActivateCs(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    PMD_ASSERT(g_pmdConfig != NULL, "g_pmdConfig must be not NULL.");

    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated || 1 == g_SimActivated)
    {
        status = FALSE;
        if (1 == g_SimActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU-SIM has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_SimActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    return status;
}

//=============================================================================
// pmd_OpalSpiDeactivateCs
//-----------------------------------------------------------------------------
/// Deactivate Opal SPI. refer to hal_SpiDeActivateCs() for details.
//=============================================================================
PROTECTED BOOL pmd_SIMSpiDeactivateCs(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    UINT32 scStatus = hal_SysEnterCriticalSection();
    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);
    g_SimActivated = 0;
    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}

#endif
//=============================================================================
// pmd_OpalSpiRead
//-----------------------------------------------------------------------------
/// Read Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @return register value                 0 write 1 read
//=============================================================================
PUBLIC UINT16 pmd_OpalSpiRead(RDA_REG_MAP_T regIdx)
{
    UINT32 wrData, rdData;
    UINT32 count;
    UINT16 value;

    wrData = (1<<25) | ((regIdx & 0x1ff) << 16) | 0;
    
    while(hal_IspiTxFifoAvail(HAL_ISPI_CS_PMU) < 1 ||
            hal_IspiSendData(HAL_ISPI_CS_PMU, wrData, TRUE) == 0);

    //wait until any previous transfers have ended
    while(!hal_IspiTxFinished(HAL_ISPI_CS_PMU));
    count = hal_IspiGetData(HAL_ISPI_CS_PMU, &rdData);
    PMD_ASSERT(1 == count, "PMD expected 1 data got %d !", count);

    value = (UINT16)(rdData & 0xffff);
    //PMD_TRACE(PMD_INFO_TRC, 0, "REG 0x%x: 0x%x", regIdx, value);

    return value;
}

//=============================================================================
// pmd_OpalSpiWrite
//-----------------------------------------------------------------------------
/// Write Opal register on SPI, the CS must already be activated
/// @param regIdx register to read
/// @param value register value
//=============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_OpalSpiWrite(RDA_REG_MAP_T regIdx, UINT16 value)
{
    UINT32 wrData;

    //debug printf
	//hal_HstSendEvent(SYS_EVENT, 0xFF000000 | (regIdx << 16) | value);
    wrData = (0<<25) | ((regIdx & 0x1ff) << 16) | (value);

    while(hal_IspiTxFifoAvail(HAL_ISPI_CS_PMU) < 1 ||
            hal_IspiSendData(HAL_ISPI_CS_PMU, wrData, FALSE) == 0);

    //wait until any previous transfers have ended
    while(!hal_IspiTxFinished(HAL_ISPI_CS_PMU));    
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingSet()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
 	UINT32 reg_tmp = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);//add by sjj in 2016.2.29
    reg_tmp |= flags;

    g_pmdIrqSettingLatest = reg_tmp;//by add by sjj in 2016.2.29
//	hal_HstSendEvent(SYS_EVENT, 0x14041002);
//	hal_TimDelay(16);
//    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)reg_tmp);
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);
    // Never save the clear bits
    g_pmdIrqSettingLatest &= ~(RDA_PMU_INT_CHR_CLEAR|RDA_PMU_PENIRQ_CLEAR|
                               RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_IRQ_SETTINGS must go through
// pmd_RegIrqSettingSet() or pmd_RegIrqSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegIrqSettingClr(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdIrqSettingLatest &= ~flags;
	//hal_HstSendEvent(SYS_EVENT, 0x14041003);
	//hal_HstSendEvent(SYS_EVENT, flags);
	//hal_DbgAssert("assert in pmd");
    pmd_OpalSpiWrite(RDA_ADDR_IRQ_SETTINGS, (UINT16)g_pmdIrqSettingLatest);//pd_Mode

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_HP_DETECT_SETTING must go through
// pmd_RegHpDetectSettingSet() or pmd_RegHpDetectSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegHpDetectSettingSet(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdHpDetectSettingLatest |= flags;
    pmd_OpalSpiWrite(RDA_ADDR_HP_DETECT_SETTING, (UINT16)g_pmdHpDetectSettingLatest);
    // Never save the clear bits
    g_pmdHpDetectSettingLatest &= ~(RDA_PMU_HP_OUT_CLEAR|RDA_PMU_HP_IN_CLEAR);

    hal_SysExitCriticalSection(scStatus);
}

// -------------------------------------------------------------
// All the write access to RDA_ADDR_HP_DETECT_SETTING must go through
// pmd_RegHpDetectSettingSet() or pmd_RegHpDetectSettingClr()
// -------------------------------------------------------------
PUBLIC VOID pmd_RegHpDetectSettingClr(UINT32 flags)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdHpDetectSettingLatest &= ~flags;
    pmd_OpalSpiWrite(RDA_ADDR_HP_DETECT_SETTING, (UINT16)g_pmdHpDetectSettingLatest);

    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID PMD_FUNC_INTERNAL pmd_RDAWrite(RDA_REG_MAP_T regIdx,UINT16 data)
{
#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsBlockingActivate(TRUE);
    }
#endif

    pmd_OpalSpiWrite(regIdx, data);

#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(TRUE);
    }
#endif
}

#if 0//Never used in 5855
BOOL pmd_SimSPIWrite(RDA_REG_MAP_T regIdx,UINT16 data)
{
#if 0
    if (FALSE == pmd_SIMSpiActivateCs(TRUE))
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "spi SIM BUSY");   
        return FALSE;
    }
#endif

    pmd_OpalSpiWrite(regIdx, data);

#if 0
    pmd_SIMSpiDeactivateCs(TRUE);
#endif

    return TRUE;
}

#endif
BOOL pmd_TSDGetSPI(VOID)
{
    if (FALSE == pmd_OpalSpiActivateCs())
    {
        PMD_TRACE(PMD_INFO_TRC, 0, "spi TSD BUSY");   
        return FALSE;
    }
    return TRUE;
    
}
BOOL pmd_TSDReleaseSPI(VOID)
{
    pmd_OpalSpiDeactivateCs();
    return TRUE;
    
}


//=============================================================================
// pmd_RDARead
//-----------------------------------------------------------------------------
/// Read Opal register on SPI
/// the CS will be activated and deactivated automatically
/// @param regIdx register to read
/// @return register value
///
/// Pls don't call this function in interrupt!!! By ZM
//=============================================================================
PUBLIC UINT16 pmd_RDARead(RDA_REG_MAP_T regIdx)
{
    u16 readVal = 0;
    UINT32 scStatus;
    
    /* SPI shared by PMU, ABB, FM(to open close I2C), especially in tsd_Debounce()
     * in KEY interrupt context, it will interrupt the operation here when user
     * press key in quick succession. 
     */
    scStatus = hal_SysEnterCriticalSection();

    if (g_pmdInitDone)
    {
        pmd_OpalCsBlockingActivate(FALSE);
    }

    readVal = pmd_OpalSpiRead(regIdx);

    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(FALSE);
    }

    hal_SysExitCriticalSection(scStatus);
    
    return readVal;
}



// ============================================================================
// pmd_EnableOpalLdo
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param ldo the id of the ldo to switch from enum #PMD_LDO_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
/// @param profile the profile applied.
/// @param user the LDO user set by POWER_LDO_USER or LEVEL_LDO_USER macro.
// ============================================================================
PROTECTED VOID pmd_EnableOpalLdo(PMD_LDO_ID_T ldo, BOOL on,
                                 PMD_PROFILE_MODE_T profile, UINT32 user)
{
    //PMD_TRACE(PMD_DBG_TRC, 0, "pmd_EnableOpalLdo: profile=%d on=%d ldo=0x%x", profile, on, ldo);

    PMD_ASSERT(profile < PMD_PROFILE_MODE_QTY, "pmd_EnableOpalLdo: Invalid profile number (%d)", profile);
    PMD_ASSERT(ldo.type == PMD_LDO_ID_TYPE_OPAL, "pmd_EnableOpalLdo: using ID of non LDO resource (%08x) ", ldo);
    PMD_ASSERT(ldo.id < (1<<PMD_LDO_QTY),
        "pmd_EnableOpalLdo: invalid LDO Id 0x%08x", ldo);

    // VMEM is derived from VUSB, so VUSB should be always on
    // (VUSB is guaranteed to be turned on during initialization)
    if (ldo.id & PMD_LDO_USB)
    {
        ldo.id &= ~PMD_LDO_USB;
    }
    // VMIC is derived from VABB
    if (ldo.id & PMD_LDO_MIC)
    {
        ldo.opal |= PMD_LDO_ABB;
    }

    if(ldo.id & PMD_LDO_IO1P8)
    {
        if(!on)
        {
            ldo.id &= ~PMD_LDO_IO1P8;// Don't shut down IO1P8 for ext-RAM of 5855
        }
    }

    if (ldo.id == 0)
    {
        return;
    }

    UINT32 reg_offset = 0;
    UINT32 idx = 0;
    UINT32 ldoId = ldo.id;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    while (ldoId)
    {
        if (ldoId & 0x1)
        {
            if (on)
            {
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO has been enabled before, and we do not need to enable it again.
                    ldo.id &= ~(1<<idx);
                }
                g_pmdLdoUserBitmap[profile][idx] |= user;
            }
            else
            {
                g_pmdLdoUserBitmap[profile][idx] &= ~user;
                if (g_pmdLdoUserBitmap[profile][idx] != 0)
                {
                    // This LDO is still used by another user, and we can NOT disable it.
                    ldo.id &= ~(1<<idx);
                }
            }
        }
        ldoId >>= 1;
        idx++;
    }

    if (ldo.id == 0)
    {
        hal_SysExitCriticalSection(scStatus);
        return;
    }

    // NOTE:
    // The hw bit for PMD_LDO_BLLED is useless.
    // Sw should set rgbLedOff bit in LDO setting2 register instead.

    if (!on)// Different with 8809
    { 
        g_pmdLdoProfileSetting1[profile].reg &= ~(ldo.id & RDA_PMU_LDO_EN_MASK);
        if (ldo.id & PMD_LDO_BLLED)
        {
    //        g_pmdLdoProfileSetting2[profile].rgbLedOff = FALSE;
        }
    }
    else
    { 
        g_pmdLdoProfileSetting1[profile].reg |= ldo.id & RDA_PMU_LDO_EN_MASK;
        if (ldo.id & PMD_LDO_BLLED)
        {
    //        g_pmdLdoProfileSetting2[profile].rgbLedOff = TRUE;
        }
    }

    // ACT and LP LDO setting1 registers are in the same structure
    if (profile == PMD_PROFILE_MODE_LOWPOWER)
    {
        reg_offset = 5;
    }

    // during pmd_Open, don't write to Opal Profile reg for each LDO change
    // pmd_Open will write the final value before enabling the profile
    if (g_pmdInitDone)
    {
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1+reg_offset,g_pmdLdoProfileSetting1[profile].reg);
        if (ldo.id & PMD_LDO_BLLED)
        {
            pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2+reg_offset,g_pmdLdoProfileSetting2[profile].reg);
        }
    } 

    hal_SysExitCriticalSection(scStatus);
}



// =============================================================================
//  Special API with lower level SPI handling (for multiple register access)
// =============================================================================


//=============================================================================
// pmd_OpalSpiSendCommand
//-----------------------------------------------------------------------------
/// Send a burst of commands to Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results ensure the command has been effectively send).
/// @param cmd array of bytes containing the command, use #PMD_PMD_SPI_BUILD_WRITE_COMMAND and #PMD_OPAL_SPI_BUILD_READ_COMMAND.
/// @param length number of SPI command to send (cmd array must by 3 time that)
//=============================================================================
PROTECTED VOID pmd_OpalSpiSendCommand(CONST UINT32* cmd, UINT32 length)
{
    UINT32 count = 0;
    while (count < length)
    {
        count += hal_IspiSendDataBuffer(HAL_ISPI_CS_PMU,
                                        cmd + count,
                                        length - count);
    }
}


//=============================================================================
// pmd_OpalSpiGetData
//-----------------------------------------------------------------------------
/// Read a burst of data from Opal over SPI. SPI is configured in non DMA mode
/// caller must ensure the SPI FIFO will not overflow.
/// (reading results before sending too much new commands).
/// @param data array of bytes to receive the data, use #PMD_OPAL_SPI_EXTRACT_DATA
/// @param length number of SPI command recieve (data array must by 3 time that)
///
/// This function is blocking, it will pool to get all the expected data, 
/// use with care!
//=============================================================================
PROTECTED VOID pmd_OpalSpiGetData(UINT32* data, UINT32 length)
{
    UINT32 count;
    count = 0;
    while (count < length)
    {
        count += hal_IspiGetDataBuffer(HAL_ISPI_CS_PMU,
                                       data + count,
                                       length - count);
    }
}


// ============================================================================
//  pmd_EnableFmInLpMode
// ============================================================================
PRIVATE VOID pmd_EnableFmInLpMode(BOOL on)
{
    PMD_LDO_ID_T abbLdo = { .opal = PMD_LDO_ABB };
    pmd_EnableOpalLdo(abbLdo, on, PMD_PROFILE_MODE_LOWPOWER, POWER_LDO_USER(PMD_POWER_FM));

    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdFmInLpModeEnabled = on;

    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_FM, !on);

#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
    if (on)
    {
        // Prefer class AB mode for speaker
        pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_CLASS_AB,
                             PMD_SPEAKER_PA_MODE_CLASS_AB);
    }
    else
    {
        // Restore the original speaker mode
        pmd_SpeakerPaRestoreMode();
    }
#endif

#ifdef FM_LDO_WORKAROUND
    if (g_pmdLdoModeEnabled)
    {
        if (on)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = FALSE;
        }
        else
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = TRUE;
        }
    }
    else
#endif
    {
        if (on)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
        }
        else
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
        }
    }
    pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);	
    hal_SysExitCriticalSection(scStatus);
}


#ifdef FM_LDO_WORKAROUND
// ============================================================================
//  pmd_DcdcPower_Control
// ============================================================================
PRIVATE VOID pmd_DcdcPower_Control(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoModeEnabled = !on;
    if (on) // DCDC mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = TRUE;//enable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = TRUE;//disable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = TRUE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = TRUE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }
    else // LDO mode
    {
        // Active mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);		
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2LdoOff = FALSE;//enable LDO
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        //hal_TimDelay(10);

        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].buck1On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].buck2On = FALSE;//disable dcdc
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);

        // LP mode setting
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1LdoOff = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].buck1On = FALSE;
        pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

        if (g_pmdFmInLpModeEnabled)
        {
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2LdoOff = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
            g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].buck2On = FALSE;
            pmd_RDAWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
        }
    }

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
//  pmd_EnableDcdcPower
// ============================================================================
PUBLIC VOID pmd_EnableDcdcPower(BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdLdoReqFlag = !on;

    if (!on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, TRUE);
    }

    if(!g_pmdHighVcoreReqFlag)
    {
        pmd_DcdcPower_Control(on);
	}

    if (on)
    {
        rfd_XcvSetDcdcLdoMode(TRUE, FALSE);
    }

    hal_SysExitCriticalSection(scStatus);
}
#endif // FM_LDO_WORKAROUND


// ============================================================================
// pmd_AudExtPaExists
// ----------------------------------------------------------------------------
/// Tell if an external audio PA exists in the configuration
// ============================================================================
PUBLIC BOOL pmd_AudExtPaExists(VOID)
{
    return g_pmdAudExtPaExists;
}


// =============================================================================
// pmd_SetMmcVoltage
// -----------------------------------------------------------------------------
/// level :0-7
//  7-3.2V  3-3.09v  0-2.9v
// =============================================================================
PUBLIC VOID pmd_SetMmcVoltage(UINT8 level)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoActiveSetting4 = 
        PMU_SET_BITFIELD(g_pmdLdoActiveSetting4,
                         RDA_PMU_VMMC_IBIT,
                         level);
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING4, g_pmdLdoActiveSetting4);
    hal_SysExitCriticalSection(scStatus);
}


// =============================================================================
// pmd_Open
// -----------------------------------------------------------------------------
/// Open the PMD driver. This function configure the devices and set the 
/// global variables needed for proper operation of the PMD driver.
/// It must be called before any other function of the PMD driver. That's why 
/// this function is called in the end of #hal_Open. That way, the PMD driver
/// functions should always be usable by the user program, as soon as its main
/// function is launched by HAL.
///
/// @param pmdConfig Pointer to the PMD configuration structure as obtained
/// through the TGT API (#tgt_GetPmdConfig).
// -----------------------------------------------------------------------------
// array to store information for shared pins:
// UINT32 g_pmdMapAccess.powerInfo[PMD_POWER_ID_QTY];
// the first ID of a share has bit 31 = 0 and lower bits represent resource used
// the other IDs have bit 31 = 1 and lower bits are the ID of the first one
// note: if more that 31 IDs are needed
// change the bit usage: instead of indexing with the bits with ID use bit 0
// for first, then add the bit to the LINK information (need 5 bits)
// then we can use IDs as we want but a shared group is limited to 31 IDs for
// the same pin.
// =============================================================================
PUBLIC VOID pmd_Open(CONST PMD_CONFIG_T* pmdConfig)
{
    UINT32 i;

#ifdef SPI_REG_DEBUG
    extern VOID pmd_RegisterSpiRegCheckFunc(VOID);
    pmd_RegisterSpiRegCheckFunc();
#endif // SPI_REG_DEBUG

    //PMD_ASSERT(PMD_POWER_ID_QTY < 31,
    //    "Expected PMD_POWER_ID_QTY < 31, implementation of g_pmdMapAccess.powerInfo must be updated to support more IDs",
    //    PMD_POWER_ID_QTY);
    PMD_ASSERT(PMD_POWER_ID_QTY + PMD_LEVEL_ID_QTY <= 31,
        "Too many LDO users for current LDO user bitmap: powerIds=%d, levelIds=%d",
        PMD_POWER_ID_QTY, PMD_LEVEL_ID_QTY);

     PMD_ASSERT(PMD_POWER_ID_QTY == PMD_MAP_POWER_ID_QTY, "must be in sync with pmd_map.xmd PMD_MAP_POWER_ID_QTY");
      PMD_ASSERT(PMD_LEVEL_ID_QTY == PMD_MAP_LEVEL_ID_QTY, "must be in sync with pmd_map.xmd PMD_MAP_LEVEL_ID_QTY");
	  

    PMD_ASSERT(pmdConfig != NULL, "pmdConfig must be not NULL.");

    g_pmdConfig = pmdConfig;
    g_pmdInitDone = FALSE;

    // DCDC VCORE value->voltage:
    // 0->0.74, 1->0.81, 2->0.88, 3->0.95, 4->1.03, 5->1.09,
    // 6->1.16, 7->1.25, 8->1.33, 9->1.43, 10->1.49, 11->1.56,
    // 12->1.65, 13->1.69, 14->1.75, 15->1.80

    // Check chip version
    UINT32 metalId = hal_SysGetChipId(HAL_SYS_CHIP_METAL_ID);

    //312M PLL need >= 8;
    //the active mode min voltage(PMD_CORE_VOLTAGE_LOW) could be  4
    //the low power mode min voltage could be 0
    #ifdef Low_Power_Consumption    
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = 11;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 11;// old 12
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = 11;
    #else
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_HIGH] = 12;
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_MEDIUM] = 12;// old 12
    g_pmdDcdcVcoreValueActiveAry[PMD_CORE_VOLTAGE_LOW] = 12;
    #endif
    // TODO: BT requires VBUCK1_NLP/LP=8,VBUCK2_NLP/LP=9 on U02 chips.
    //       This might be skipped for U04 or later chips.
    if (metalId < HAL_SYS_CHIP_METAL_ID_U04)
    {
        g_pmdDcdcVcoreValueLp = 4;//8;
    }
    else
    {
        g_pmdDcdcVcoreValueLp = 4;//4;
    }

    // Set default LDO on, more will be updated by power[] and level[]
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg &=
                            ~(g_pmdConfig->ldoEnableNormal & RDA_PMU_LDO_EN_MASK);
    g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg &=
                            ~(g_pmdConfig->ldoEnableLowPower & RDA_PMU_LDO_EN_MASK);
    // NOTE:
    // The hw bit for vBlLedOff is useless.
    // Sw should set rgbLedOff bit in LDO setting2 register instead.
#if 0 //No rgbLedoff bit in 5855
    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].rgbLedOff = 
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].vBlLedOff;
    g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].rgbLedOff = 
        g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].vBlLedOff;
#endif
    // Init volatge selection
    if (metalId < HAL_SYS_CHIP_METAL_ID_U04)
    {
        // vPad should be lowered to 1.8V when in LP mode
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIo2p8_sel = TRUE;
    }

    if (g_pmdConfig->ldoIbrIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIbr_sel = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIbr_sel = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIbr_sel = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIbr_sel = TRUE;
    }
    if (g_pmdConfig->ldoMMCIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vMmc_sel = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vMmc_sel = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vMmc_sel = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vMmc_sel = TRUE;
    }
    if (g_pmdConfig->ldoLcdIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vLcd_sel = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vLcd_sel = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vLcd_sel = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vLcd_sel = TRUE;
    }
    if (g_pmdConfig->ldoIoIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIo1p8_sel = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIo1p8_sel = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vIo1p8_sel = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vIo1p8_sel = TRUE;
    }
    if (g_pmdConfig->ldoRfIs2_8)
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vAsw_sel = FALSE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vAsw_sel = FALSE;
    }
    else
    {
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vAsw_sel = TRUE;
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].vAsw_sel = TRUE;
    }

    // Init LDO user bitmap
    for (i=0; i<PMD_LDO_QTY; i++)
    {
        g_pmdLdoUserBitmap[PMD_PROFILE_MODE_NORMAL][i] = 0;
        g_pmdLdoUserBitmap[PMD_PROFILE_MODE_LOWPOWER][i] = 0;
    }

    // Init Clk4m and Avdd3 enabling registry in LP mode (default off)
    for (i=0; i<PMD_CLK4M_AVDD3_USER_ID_QTY; i++)
    {
        g_pmdClk4mAvdd3EnRegistryLp[i] = FALSE;
    }

#ifdef PMD_NO_ASSERT
    pmd_SpiCsNonblockingActivate(FALSE);
#else
    BOOL activateStatus = pmd_SpiCsNonblockingActivate(FALSE);
#endif
    PMD_ASSERT(activateStatus, "PMD: Activation failled, ISPI must be free for pmd_Open !");
#ifndef MEMD_EXTA_OPEN_EARLY
    if(hal_SysGetResetCause() == HAL_SYS_RESET_CAUSE_NORMAL)
	{
        // Reset PMU registers
        pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_SOFT_RESETN);
        pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_REGISTER_RESETN|RDA_PMU_SOFT_RESETN);
    }
#endif

	//Force efuse sense to zero to save power in 5856, XueguiChang
	// TODO: will be fix in 5856e
	UINT16 data;
	data = pmd_OpalSpiRead(RDA_ADDR_EFUSE_OPT_SETTING1);
	data |= RDA_PMU_EFUSE_SENSE_DR | RDA_PMU_EFFUSE_SENSE_EN_DR;
	data &= ~RDA_PMU_EFFUSE_SENSE_EN_REG;
	pmd_OpalSpiWrite(RDA_ADDR_EFUSE_OPT_SETTING1, data);

    UINT32 id;
    // power
    for (id = 0; id < PMD_POWER_ID_QTY; id++)
    {
        //g_pmdMapAccess.powerInfo[id] = 0;
        // initial state as described by powerOnState from config
        BOOL powerOn = g_pmdConfig->power[id].powerOnState;
#ifdef FASTPF_USE_USB
        if (id == PMD_POWER_USB)
        {
            powerOn = TRUE; // For USB flash programmer
        }
#endif // FASTPF_USE_USB

#if (CHIP_ASIC_ID != CHIP_ASIC_ID_5855) && (CHIP_ASIC_ID != CHIP_ASIC_ID_5856)     
        if (id == PMD_POWER_CAMERA)
        {
            powerOn = TRUE; //´ò¿ªCAMERAµçÔ´
        }
#endif        
        pmd_EnablePower(id, powerOn);
        // For GPIO, set it to output mode
        if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->power[id].ldo.pin.gpioId);
        }
    }
    hal_HstSendEvent(SYS_EVENT, 0x14071801);

    // level
    for (id = 0; id < PMD_LEVEL_ID_QTY; id++)
    {
        // initial state as described by powerOnState from config
        pmd_SetLevel(id, g_pmdConfig->level[id].powerOnState);
        // For GPIO, set it to output mode
        if (g_pmdConfig->level[id].type == PMD_LEVEL_TYPE_LDO &&
            g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_IO)
        {
            hal_GpioSetOut(g_pmdConfig->level[id].ldo.pin.gpioId);
        }
    }

    hal_HstSendEvent(SYS_EVENT, 0x14071802);
    hal_HstSendEvent(SYS_EVENT, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
    hal_HstSendEvent(SYS_EVENT, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);
    // Set LDO setting1
    
 //   03 reg: 0xBDFB from shiningzhu
 //   08 reg: 0xB853 from shiningzhu
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING1, g_pmdLdoProfileSetting1[PMD_PROFILE_MODE_LOWPOWER].reg);

    hal_HstSendEvent(SYS_EVENT, 0x14071803);
#if 1
    // Set LDO setting2
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_LP_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_LOWPOWER].reg);
#endif
    // Init LCD/LED setting
    //rgbLED(reg 0x19) is useless in 5855
//    pmd_OpalSpiWrite(RDA_ADDR_LED_SETTING1, g_pmdLedSetting1);
    pmd_OpalSpiWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
#if 0//rgb led is inactive in reg 07 in 5855
    // Init KP_LED_OUT voltage
#if (LCD_BACKLIGHT_CHARGE_PUMP == 0)
    g_pmdBlLedOutDefault.vRgbLedIbit = g_pmdLdoProfileSetting5.vRgbLedIbit;
    g_pmdBlLedOutDefault.vRgbLedVsel = g_pmdLdoProfileSetting5.vRgbLedVsel;
#endif
    pmd_OpalSpiWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
#endif
    // Init SIM setting (not to select any card)
    //pmd_SelectSimCard(0xff);

    // Set v_mmc to 3V
   // pmd_SetMmcVoltage(1);  //delete by huangyx 20160419

    // battery monitor
#ifndef _FLASH_PROGRAMMER // flash don't monitor battery
    hal_AnaGpadcOpen(g_pmdConfig->batteryGpadcChannel, HAL_ANA_GPADC_ATP_2S);
#endif

    pmd_InitRdaPmu();

    pmd_OpalCsDeactivate(FALSE);
    g_pmdInitDone = TRUE;

    // Check if external audio PA exists
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.type != HAL_GPIO_TYPE_NONE)
    {
        g_pmdAudExtPaExists = TRUE;
    }
	
    // Earpiece detection
    //pmd_EarpDetectGpioInit(g_pmdConfig->earpieceDetectGpio);
}

VOID pmd_ResetPmu(VOID)
{
    // Reset PMU registers
    pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_SOFT_RESETN);
    pmd_OpalSpiWrite(RDA_ADDR_PMU_RESET, RDA_PMU_REGISTER_RESETN|RDA_PMU_SOFT_RESETN);
}

// =============================================================================
// pmd_Close
// -----------------------------------------------------------------------------
/// Closes the PMD driver.
// =============================================================================
PUBLIC VOID pmd_Close(VOID)
{
    UINT32 scStatus;
    UINT16 reg;

    scStatus = hal_SysEnterCriticalSection();

    // Turn off LCD backlight
    pmd_SetLevel(PMD_LEVEL_LCD, 0);
    pmd_SetLevel(PMD_LEVEL_SUB_LCD, 0);
    // Turn off keypad lights
    pmd_SetLevel(PMD_LEVEL_KEYPAD, 0);
    pmd_SetLevel(PMD_LEVEL_LED0, 0);
    pmd_SetLevel(PMD_LEVEL_LED1, 0);
    pmd_SetLevel(PMD_LEVEL_LED2, 0);
    pmd_SetLevel(PMD_LEVEL_LED3, 0);
    // Power off ABB (The pop sound is smaller if ABB power is down at first)
    g_pmdLdoSettings.reg = 0;
    pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
    hal_TimDelay(1000 MS_WAITING);
    // Power off audio module
    hal_AudForcePowerDown();
    // Disable GPADC
    pmd_GpadcDisable();
    // Restore charger h/w setting
    pmd_RestoreChargerAtPowerOff();
    // Disable headphone detect IRQs
    pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);
    // 1) Disable EOM, PEN, Charger IRQs;
    // 2) Enter the power down mode (power controlled by PMU default profile).
    g_pmdIrqSettingLatest = 0;
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_MASK|RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_INT_CHR_MASK|
                         RDA_PMU_HOLD_PRECHARGER_EFUSE);


    //Switch to lpo 32K clock, and power off xtal totally, mean both active and lp mode
    reg = pmd_RDARead(RDA_ADDR_32K_CTRL);
    reg |= RDA_PMU_CK32K_LPO_CLK_EN;
    reg &= ~(RDA_PMU_CK32K_26MXTAL_CLK_EN | RDA_PMU_XEN_BT_ENABLE);
    pmd_RDAWrite(RDA_ADDR_32K_CTRL,reg);
    
    reg = pmd_RDARead(RDA_ADDR_XTAL_CTRL1);
    reg &= ~RDA_PMU_ENABLE_CLK_6P5M;
    pmd_RDAWrite(RDA_ADDR_XTAL_CTRL1,reg);

    //pmd_ResetPmu();
    //rfd_XcvPowerDown();//5855 had no xcv
    //hwp_tcu->LPS_PU_Ctrl &= ~(TCU_LPS_PU_ENABLE | TCU_LPS_PU_CLK_RF_ON | TCU_LPS_PU_MASK_CLK_RF_ENABLE);
    hal_TcuStop();
    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
// pmd_SetPowerMode
// ----------------------------------------------------------------------------
/// Set Power Mode to the desired mode
// ============================================================================
PUBLIC VOID PMD_FUNC_INTERNAL pmd_SetPowerMode(PMD_POWERMODE_T powerMode)
{
    // this function is in internal ram because it is called in critical conditions (ECO Mode) so NO TRACE IN HERE !
    // PMD_TRACE(PMD_INFO_TRC, 0, __func__);

    // This function is now called far before the critical point of entering 
    // the deepest low power mode and is responsible only for preparing
    // other stuff (like controlling V_ANA as bellow)

    UINT16 miscCtrl = 0;
    UINT32 scStatus = hal_SysEnterCriticalSection();
    UINT16 data;

#ifdef __PRJ_WITH_SPIFLSH__
    if (powerMode == PMD_IDLEPOWER)
    {
        memd_FlashWakeup();
#if (1)
#if (0)
// spi va_enable[10] 
pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, pmd_RDARead(RDA_ADDR_LDO_SETTINGS)|RDA_PMU_VA_ENABLE);

// spi xtal_doubler_en[0] 
data = pmd_RDARead(RDA_ADDR_XTAL_DOUBLER_SETTING);
data |= (RDA_PMU_XTAL_DOUBLER_SETTING_XTAL_DOUBLER_EN);
//pmd_RDAWrite(RDA_ADDR_XTAL_DOUBLER_SETTING, data);

// efuse_en_reg[0]
//pmd_RDAWrite(RDA_ADDR_EFUSE_OPT_SETTING1,pmd_RDARead(RDA_ADDR_EFUSE_OPT_SETTING1)|RDA_PMU_EFFUSE_SENSE_EN_REG);
#endif /* 0 */
// pu_vusb _pon[4]
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)|0x0010);
// pu_va[10]
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)|0x0400);
// Vio2p8[11] 
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)|0x0800);

//Open transciver  
data = pmd_RDARead(RDA_ADDR_POWER_TIMING2);// TODO: Is bt still work?
data &= ~(RDA_PMU_TRANSC_DIR);
pmd_RDAWrite(RDA_ADDR_POWER_TIMING2, data);


#if !defined(__CT_CHIP_PKG_RDA5856T__)
//USE BUCK, non pfm mode, for working mode
data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING2);
data |= RDA_PMU_BUCK2_ON;
data &= ~(RDA_PMU_BUCK2_LDO_OFF|RDA_PMU_BUCK2_PFM_ON);
pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING2, data);
#endif /* __CT_CHIP_PKG_RDA5856T__ */

data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING1);
data |= (RDA_PMU_NORMAL_MODE);
//pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING1, data);

//pu_vusb _pon[4]
data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING1);
data |= RDA_PMU_VUSB_ON;
//pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING1, data);

#endif /* 0 */

        
    }
#endif

	switch (powerMode)
    {
        case PMD_LOWPOWER:
            miscCtrl = g_pmdMiscControlLp;
            // Disable charge LDO to save power
            g_pmdCalibSetting2 &= ~RDA_PMU_PU_CHARGE_LDO;
            break;
        case PMD_IDLEPOWER:
            miscCtrl = g_pmdMiscControlActive;
            // Enable charge LDO to reduce vsim fluctuation in low battery
            g_pmdCalibSetting2 |= RDA_PMU_PU_CHARGE_LDO;
            // we are out of EcoMode, so we can do some checks using external
            // rams (data or functions)
            break;
        default:
            break;
    }
    pmd_RDAWrite(RDA_ADDR_MISC_CONTROL, miscCtrl);
    pmd_RDAWrite(RDA_ADDR_CALIBRATION_SETTING2, g_pmdCalibSetting2);

#ifdef __PRJ_WITH_SPIFLSH__
    if (powerMode == PMD_LOWPOWER)
    {
#if (0)
// spi va_enable[10]     //vIo1p8_sel
pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, pmd_RDARead(RDA_ADDR_LDO_SETTINGS)&(~RDA_PMU_VA_ENABLE));


// spi xtal_doubler_en[0] 
data = pmd_RDARead(RDA_ADDR_XTAL_DOUBLER_SETTING);
data &= ~(RDA_PMU_XTAL_DOUBLER_SETTING_XTAL_DOUBLER_EN);
//pmd_RDAWrite(RDA_ADDR_XTAL_DOUBLER_SETTING, data);


// efuse_en_reg[0]
//pmd_RDAWrite(RDA_ADDR_EFUSE_OPT_SETTING1,pmd_RDARead(RDA_ADDR_EFUSE_OPT_SETTING1)&(~RDA_PMU_EFFUSE_SENSE_EN_REG));
#endif /* 0 */
// pu_vusb _pon[4]
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)&0xffef);
// pu_va[10]
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)&0xfbff);
// Vio2p8[11] 
//pmd_RDAWrite(0x0d,pmd_RDARead(0x0d)&0xf7ff);
// key_enable
//pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, pmd_RDARead(RDA_ADDR_LDO_SETTINGS)|0x04);//key_enable

#if !(defined(__CT_CHIP_PKG_RDA5856T__ ) || defined(__CT_CHIP_PKG_RDA5856Q32__))
//VBUCK2V4 use BUCK, non-pfm(verified lower power than pfm mode) for low power
data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING2);
data |= (RDA_PMU_BUCK2_ON );
data &= ~(RDA_PMU_BUCK2_LDO_OFF |RDA_PMU_BUCK2_PFM_ON);
pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING2, data);
#endif /* __CT_CHIP_PKG_RDA5856T__ */

//3  //Close transciver
data = pmd_RDARead(RDA_ADDR_POWER_TIMING2);   // TODO: Is bt still work?
data |= RDA_PMU_TRANSC_DIR ;
data &= ~(RDA_PMU_TRANSC_REG );
pmd_RDAWrite(RDA_ADDR_POWER_TIMING2, data);

//3 //Lp mode
data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING1);
data &= ~(RDA_PMU_NORMAL_MODE);
//pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING1, data);

data = pmd_RDARead(RDA_ADDR_LDO_POWER_ON_SETTING1);
data &= ~RDA_PMU_VUSB_ON;
//pmd_RDAWrite(RDA_ADDR_LDO_POWER_ON_SETTING1, data);

    
        memd_FlashSleep();
    }
#endif

    hal_SysExitCriticalSection(scStatus);
}


// ============================================================================
//  pmd_EnableClk4mAvdd3InLpMode
// ============================================================================
PROTECTED VOID pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_ID_T user,
                                            BOOL on)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdClk4mAvdd3EnRegistryLp[user] = on;

    if (!on)
    {
        // Disable Clk4m and Avdd3 in LP mode only when all the users request so.
        for (UINT32 i=0; i<PMD_CLK4M_AVDD3_USER_ID_QTY; i++)
        {
            if (g_pmdClk4mAvdd3EnRegistryLp[i])
            {
                on = TRUE;
                break;
            }
        }
    }

    if (on)
    {
        // Not to direct-reg clock 4M and avdd3
        g_pmdMiscControlLp &= ~(RDA_PMU_PU_CLK_4M_DR|
                                RDA_PMU_PD_LDO_AVDD3_DR);
    }
    else
    {
        // Direct-reg to disable clock 4M and avdd3
        g_pmdMiscControlLp |= RDA_PMU_PU_CLK_4M_DR|
                              RDA_PMU_PD_LDO_AVDD3_DR|
                              RDA_PMU_PD_LDO_AVDD3_REG;
        g_pmdMiscControlLp &= ~RDA_PMU_PU_CLK_4M_REG;
    }

    hal_SysExitCriticalSection(scStatus);
}

/******************************************************************************
 * pmd_I2C_ExtEn: 
 * DESCRIPTION: - MUST disable EXT_I2C before switch to hst_rx function
 * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * 
 ******************************************************************************/
PUBLIC VOID pmd_OpenI2cTrace(void)
{
    UINT16 data;

    //Enable DBB I2C and disable EXT I2C
    data = pmd_RDARead(RDA_ADDR_SYS_CTRL);
    data &= (~RDA_PMU_SYS_CTRL_I2C_EXT_EN);
    data |= RDA_PMU_SYS_CTRL_I2C_HOST_EN;
    pmd_RDAWrite(RDA_ADDR_SYS_CTRL,data);

    hal_GpioSetPinMux(HAL_GPIO_3_7, HAL_GPIO_ALTFUNC_CFG_4);
    hal_GpioSetPinMux(HAL_GPIO_4_0, HAL_GPIO_ALTFUNC_CFG_4);
}


/******************************************************************************
 * pmd_I2C_ExtEn: 
 * DESCRIPTION: - MUST disable EXT_I2C before switch to hst_rx function
 * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * 
 ******************************************************************************/
PUBLIC VOID pmd_I2C_ExtEn(void)
{
    UINT16 data;

    //Enable DBB I2C and disable EXT I2C
    data = pmd_RDARead(RDA_ADDR_SYS_CTRL);
    data &= (~RDA_PMU_SYS_CTRL_I2C_EXT_EN);
    data |= RDA_PMU_SYS_CTRL_I2C_HOST_EN;
    pmd_RDAWrite(RDA_ADDR_SYS_CTRL,data);
}

// ============================================================================
// pmd_ConfigureRfLowPower
// ----------------------------------------------------------------------------
/// Configure the Low Power state for the RF
///
/// @param xcvUseVRF \c TRUE if the transciever use V_RF from PMD.
/// @param xcvLowPowerKeepVRFOn \c TRUE if the power on V_RF must stay on 
///         during low power.
/// @return configuration for LPS (only fieds controlling LPSCO_0 and LPSCO_1)
///         actual type is #HAL_LPS_POWERUP_CTRL_T.
// ============================================================================
PUBLIC UINT32 pmd_ConfigureRfLowPower(BOOL xcvUseVRF, BOOL xcvLowPowerKeepVRFOn)
{
    union
    {
        UINT32 reg;
        HAL_LPS_POWERUP_CTRL_T bitfield;
    } puctrl;
    
    puctrl.reg = 0;

    // If called before pmd_Open (hal_BoardSetup) we don't configure the LDOs
    if (g_pmdInitDone)
    {
        // let LPSCO_0 be controlled by LPS FSM
        puctrl.bitfield.puCO0ForceOn = 0;
        // LP_MODE is active (low power) High
        // LPSCO_0 is active during normal mode, so invert the pin polarity
        puctrl.bitfield.puCO0Pol = 1;
    }
    else
    {
        // force off
        puctrl.bitfield.puCO0ForceOn = 1;
        puctrl.bitfield.puCO0Pol = 1;
    }
    // LPSCO_1 is not used by Opal, should be set as gpio in hal tgt
    // force off
    puctrl.bitfield.puCO1ForceOn = 1;
    puctrl.bitfield.puCO1Pol = 1;

    return puctrl.reg;
}

#if 0 // no sim support

// ============================================================================
// pmd_SimSetVoltageClass
// ----------------------------------------------------------------------------
/// Set the voltage class
///     
/// The #pmd_SimVoltageOpen function controlling the level shifter must have 
/// previously called. \n
/// Voltage class must \b _ALWAYS_ be set to zero before re-programming 
/// voltage
/// @param voltClass Voltage Class to set
// ============================================================================
PRIVATE VOID pmd_SimSetVoltageClass(PMD_SIM_VOLTAGE_CLASS_T voltClass)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "sim voltage: %d", voltClass);    

    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (voltClass == PMD_SIM_VOLTAGE_0V0)
    {
        g_pmdLdoSettings.vSim1Enable = FALSE;
        g_pmdLdoSettings.vSim2Enable = FALSE;
        g_pmdLdoSettings.vSim3Enable = FALSE;
        g_pmdLdoSettings.vSim4Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim1Enable = TRUE;
        g_pmdLdoSettings.vSim2Enable = TRUE;
        g_pmdLdoSettings.vSim3Enable = TRUE;
        g_pmdLdoSettings.vSim4Enable = TRUE;
    }

    switch (voltClass)
    {
        case PMD_SIM_VOLTAGE_0V0:
            break;

        case PMD_SIM_VOLTAGE_1V8:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = TRUE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = TRUE;
            break;

        case PMD_SIM_VOLTAGE_3V0:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        case PMD_SIM_VOLTAGE_5V0:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim voltage: invalid voltage class %d", voltClass);                    
        break;
    }

    if (voltClass == PMD_SIM_VOLTAGE_0V0)
    {
        hal_SimDisableSci();
    }

    pmd_SimSPIWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_LP_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    if (voltClass != PMD_SIM_VOLTAGE_0V0 &&
        (g_pmdSimSelect & ABB_SIM_SELECT_MASK) != 0)
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);
}

DUALSIM_ERR_T pmd_FourSimSetVoltageClass(DUALSIM_SIM_VOLTAGE_T voltClass0,
                                         DUALSIM_SIM_VOLTAGE_T voltClass1, 
                                         DUALSIM_SIM_VOLTAGE_T voltClass2,
                                         DUALSIM_SIM_VOLTAGE_T voltClass3)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_FourSimSetVoltageClass: class1 = %d , class2 = %d  class3 = %d , class4 = %d ",
		voltClass0,voltClass1, voltClass2,voltClass3);
    UINT16 simSelMask = 0;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    if (voltClass0 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim1Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim1Enable = TRUE;
        simSelMask |= ABB_SIM_SELECT_SIM1;
    }

    switch (voltClass0)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim1Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim0 voltage: invalid voltage class %d", voltClass0);                    
        break;
    }

    if (voltClass1 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim2Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim2Enable = TRUE;
        simSelMask |= ABB_SIM_SELECT_SIM2;
    }

    switch (voltClass1)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim2Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim1 voltage: invalid voltage class %d", voltClass0);                    
        break;
    }

    if (voltClass2 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim3Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim3Enable = TRUE;
        simSelMask |= ABB_SIM_SELECT_SIM3;
    }

    switch (voltClass2)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim3Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim2 voltage: invalid voltage class %d", voltClass0);                    
        break;
    }

    if (voltClass3 == DUALSIM_SIM_VOLTAGE_NULL)
    {
        g_pmdLdoSettings.vSim4Enable = FALSE;
    }
    else
    {
        g_pmdLdoSettings.vSim4Enable = TRUE;
        simSelMask |= ABB_SIM_SELECT_SIM4;
    }

    switch (voltClass3)
    {
        case DUALSIM_SIM_VOLTAGE_NULL:
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_C:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = TRUE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_B:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        case DUALSIM_SIM_VOLTAGE_CLASS_A:
            g_pmdLdoProfileSetting5.vSim4Is1_8 = FALSE;
            break;

        default:
            PMD_ASSERT(FALSE, "sim3 voltage: invalid voltage class %d", voltClass0);                    
        break;
    }

    if ((simSelMask & g_pmdSimSelect) == 0)
    {
        hal_SimDisableSci();
    }
    pmd_SimSPIWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_LP_SETTING5, g_pmdLdoProfileSetting5.reg);
    pmd_SimSPIWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    if ((simSelMask & g_pmdSimSelect) != 0)
    {
        hal_SimEnableSci();
    }
    hal_SysExitCriticalSection(scStatus);

    return DUALSIM_ERR_NO;
}


BOOL pmd_SelectSimCard(UINT8 sim_card)
{
    PMD_LDO_SETTINGS_T simEnMask;
    simEnMask.reg = 0;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    g_pmdSimSelect = g_pmdSimSelect & (~ABB_SIM_SELECT_MASK);

    if (sim_card == 0)
    {
        g_pmdSimSelect |= ABB_SIM_SELECT_SIM1;
        simEnMask.vSim1Enable = TRUE;
    }
    else if (sim_card == 1)
    {
        g_pmdSimSelect |= ABB_SIM_SELECT_SIM2;
        simEnMask.vSim2Enable = TRUE;
    }
    else if (sim_card == 2)
    {
        g_pmdSimSelect |= ABB_SIM_SELECT_SIM3;
        simEnMask.vSim3Enable = TRUE;
    }
    else if (sim_card == 3)
    {
        g_pmdSimSelect |= ABB_SIM_SELECT_SIM4;
        simEnMask.vSim4Enable = TRUE;
    }

    if ((simEnMask.reg & g_pmdLdoSettings.reg) == 0)
    {
        hal_SimDisableSci();
    }

    hal_AbbRegWrite(CODEC_SIM_INTERFACE, g_pmdSimSelect);

    if ((simEnMask.reg & g_pmdLdoSettings.reg) != 0)
    {
        hal_SimEnableSci();
    }

    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}
#endif

#if (CONFIG_CLASS_K_SPEAKER_PA)
VOID pmd_EnableClassKLoudSpeaker(VOID)
{
    HAL_APO_ID_T apo = g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].ldo.pin;

    VOID (*gpioActive)(HAL_APO_ID_T apo);
    VOID (*gpioInactive)(HAL_APO_ID_T apo);
    if (g_pmdConfig->power[PMD_POWER_LOUD_SPEAKER].polarity)
    {
        gpioActive = &hal_GpioSet;
        gpioInactive = &hal_GpioClr;
    }
    else
    {
        gpioActive = &hal_GpioClr;
        gpioInactive = &hal_GpioSet;
    }

    // Generate a pulse sequence to configure class K audio PA

    // TODO:
    //     Change the following codes to meet your PA specification.
    // CAUSION:
    //     There is no INT-LOCK in hal_SysWaitMicrosecond(). Please
    // by yourselves determine whether INT-LOCK is needed when
    // generating the pulse sequence.

    //UINT32 scStatus = hal_SysEnterCriticalSection();

    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioInactive)(apo);
    hal_SysWaitMicrosecond(10);
    (*gpioActive)(apo);

    //hal_SysExitCriticalSection(scStatus);
}
#endif // CONFIG_CLASS_K_SPEAKER_PA


// ============================================================================
// pmd_EnablePower 
// ----------------------------------------------------------------------------
/// Set Power Control to desired state
///
/// @param id the id of the power to switch from enum #PMD_POWER_ID_T
/// @param on the desired state \c TRUE is on, \c FALSE is off.
// ============================================================================
PUBLIC VOID pmd_EnablePower(PMD_POWER_ID_T id, BOOL on)
{
    BOOL newState;
    UINT32 callerAddr = 0;
    UINT32 scStatus;

    SXS_GET_RA(&callerAddr);
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_EnablePower id=%i on=%i from 0x%x", id, on, callerAddr);

    newState = on;
    //g_pmdMapAccess.powerInfo[id] = newState;

    if (id == PMD_POWER_USB)
    {
		// Power on/off USB PHY
		// If v_usb is shared by other modules, it might NOT be powered down when
		// USB is requested to be turned off. To avoid impacting USB reset, USB PHY
		// must be directly controlled (not by baseband USB module).
    }
    else if (id == PMD_POWER_FM)
    {
        // Allow FM playing in LP mode
        pmd_EnableFmInLpMode(on);
        if (on)
        {
            // Power on FM
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.fmEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
            // Enable FM IO
            hal_ConfigFmIo(on);
        }
        else
        {
            // Disable FM IO
            hal_ConfigFmIo(on);
            // Power down FM
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.fmEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
        }
    }
    else if (id == PMD_POWER_BT)
    {
        if (on)
        {
            // Power on BT
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.btEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
            // Enable BT IO
            hal_ConfigBtIo(on);
        }
        else
        {
            // Disable BT IO
            hal_ConfigBtIo(on);
            // Power down BT
            scStatus = hal_SysEnterCriticalSection();
            g_pmdLdoSettings.btEnable = on;
            pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
            hal_SysExitCriticalSection(scStatus);
        }
    }
    else if(id == PMD_POWER_AUDIO)
    {
        scStatus = hal_SysEnterCriticalSection();
        hal_AudEnableCodecAuxPll(on);
        //g_pmdLdoSettings.vAbbEnable = on;
        //pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);
        hal_SysExitCriticalSection(scStatus);
    }
    else if(id == PMD_POWER_SDMMC)
    {
        scStatus = hal_SysEnterCriticalSection();
		//on select 3.2v for sdmmc, 0ff select 2.8v for sdmmc
        g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].vMmc_sel = on;
        pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING2, g_pmdLdoProfileSetting2[PMD_PROFILE_MODE_NORMAL].reg);
        hal_SysExitCriticalSection(scStatus);
    }
    else if(id == PMD_POWER_I2C)
    {
        UINT32 regVal = 0;
        UINT8 times = 0;
        while(!(pmd_RegRead(RDA_ADDR_SYS_CTRL, &regVal)))
        {
            if(times == 5)
            {
                break;
            }
            times++;
            COS_SleepByTick(2);
        }

        if(times < 5)
        {
            scStatus = hal_SysEnterCriticalSection();
            if(on)// enable i2c
            {
                pmd_RDAWrite(RDA_ADDR_SYS_CTRL,(regVal|RDA_PMU_SYS_CTRL_PMU_I2C_EN));
            }
            else// disable i2c
            {
                pmd_RDAWrite(RDA_ADDR_SYS_CTRL,regVal&(~RDA_PMU_SYS_CTRL_PMU_I2C_EN));
            }
            hal_SysExitCriticalSection(scStatus);
        }
        else
        {
            hal_HstSendEvent(SYS_EVENT, 0x14122100);
        }
    }

    // actual update according to newState.
    if (g_pmdConfig->power[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
    {
        pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                          PMD_PROFILE_MODE_NORMAL, POWER_LDO_USER(id));

        if ( id == PMD_POWER_EARPIECE &&
             (g_pmdConfig->power[id].ldo.id&PMD_LDO_MIC) != 0 )
        {
            // VMIC needs 15 ms to be stable, so it can NOT always be off during low power
            pmd_EnableOpalLdo(g_pmdConfig->power[id].ldo, newState,
                              PMD_PROFILE_MODE_LOWPOWER, POWER_LDO_USER(id));
#ifdef VMIC_POWER_ON_WORKAROUND
            if (g_pmdInitDone)
            {
                aud_EnableEarpieceSetting(on);
            }
            else
            {
                // OS is NOT ready yet. Audio module should be initialize directly.
                if (on)
                {
                    aud_InitEarpieceSetting();
                }
            }
#endif // VMIC_POWER_ON_WORKAROUND
        }
    }
    else
    {   // Pin
        // For pin-controlled power items, e.g., external loud speaker
#if (CONFIG_CLASS_K_SPEAKER_PA)
        if (id == PMD_POWER_LOUD_SPEAKER && newState)
        {
            pmd_EnableClassKLoudSpeaker();
        }
        else
#endif // CONFIG_CLASS_K_SPEAKER_PA
        {
            if ( (newState && !g_pmdConfig->power[id].polarity) ||
                  (!newState && g_pmdConfig->power[id].polarity) )
            {
                hal_GpioClr(g_pmdConfig->power[id].ldo.pin);
            }
            else
            {
                hal_GpioSet(g_pmdConfig->power[id].ldo.pin);
            }
        }
    }
}


// ============================================================================
// pmd_SetLevel(PMD_LEVEL_LOUD_SPEAKER)
// ----------------------------------------------------------------------------
/// Set a Level to desired value. PMD_LEVEL_LCD
///
/// @param id the id of the level to set from enum #PMD_LEVEL_ID_T
/// @param level the desired value, its acutal type depends on the \p id
// ============================================================================
#define PMD_LEVEL_KEYPAD_DUTY_CYCLE_MASK RDA_PMU_PWM_G_DUTY_CYCLE_MASK|RDA_PMU_PWM_B_DUTY_CYCLE_MASK
#define PMD_LEVEL_KEYPAD_DUTY_CYCLE(n) RDA_PMU_PWM_G_DUTY_CYCLE(n)|RDA_PMU_PWM_B_DUTY_CYCLE(n)
#define PMD_LEVEL_KEYPAD_MASK RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_B_OFF_ACT


#define PMD_LEVEL_LED3_DUTY_CYCLE_MASK RDA_PMU_PWM_R_DUTY_CYCLE_MASK
#define PMD_LEVEL_LED3_DUTY_CYCLE(n) RDA_PMU_PWM_R_DUTY_CYCLE(n)
#define PMD_LEVEL_LED3_MASK RDA_PMU_LED_R_OFF_ACT


PUBLIC VOID pmd_SetLevel(PMD_LEVEL_ID_T id, UINT32 level)
{
    PMD_TRACE(PMD_INFO_TRC, 0, "pmd_SetLevel id=%i level=%i", id, level); 

    UINT32 status;
    UINT32 value;

    switch (g_pmdConfig->level[id].type)
    {
        case PMD_LEVEL_TYPE_NONE:
            break;
        case PMD_LEVEL_TYPE_PWL0:
            if (level > 7)
            {
                level = 7;
            }
            hal_PwlSelLevel(HAL_PWL_0,g_pmdConfig->lightLevelToPwm0[level]);
            break;
        case PMD_LEVEL_TYPE_PWL1:
            if (level > 7)
            {
                level = 7;
            }
            hal_PwlSelLevel(HAL_PWL_1,g_pmdConfig->lightLevelToPwm1[level]);
            break;
        case PMD_LEVEL_TYPE_LPG:
            PMD_ASSERT(FALSE, "TODO impelment LPG", id);
            break;
        case PMD_LEVEL_TYPE_LDO:
            PMD_ASSERT(g_pmdConfig->level[id].ldo.type != PMD_LDO_ID_TYPE_INVALID,
                                    "Invalid LDO type: id=%d, type=%d",
                                    id, g_pmdConfig->level[id].ldo.type);
            if (g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_OPAL)
            {   // Opal LDO
                pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (0 == level)?FALSE:TRUE,
                                  PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));

                if (id == PMD_LEVEL_VIBRATOR)
                {
                    // Allow vibrator to work in LP mode
                    pmd_EnableOpalLdo(g_pmdConfig->level[id].ldo, (0 == level)?FALSE:TRUE,
                                      PMD_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(id));
                }
            }
            else
            {   // Pin
                switch (id)
                {
                    case PMD_LEVEL_LCD:
                    case PMD_LEVEL_KEYPAD:
                    case PMD_LEVEL_LED0:
                    case PMD_LEVEL_LED1:
                    case PMD_LEVEL_LED2:
                    case PMD_LEVEL_LED3:
                        if (0 == level)
                        {
                            hal_GpioClr(g_pmdConfig->level[id].ldo.pin);
                        }
                        else
                        {
                            hal_GpioSet(g_pmdConfig->level[id].ldo.pin);
                        }
                        break;
                    default:
                        PMD_ASSERT(g_pmdConfig->level[id].ldo.type == PMD_LDO_ID_TYPE_NONE,
                                                "Invalid level LDO config: id=%d, type=%d",
                                                id, g_pmdConfig->level[id].ldo.type);
                        break;
                }
            }
            break;
        case PMD_LEVEL_TYPE_OPAL:
            switch (id)
            {
                case PMD_LEVEL_KEYPAD:
                case PMD_LEVEL_LED0:
                case PMD_LEVEL_LED1:
                case PMD_LEVEL_LED2:
                case PMD_LEVEL_LED3:
                    {
                        // Keypad LED backlight control is driven by PMU directly instead of HAL PWL0.
                        if (level > 7)
                        {
                            level = 7;
                        }
                        // Get a value from the PWM0 config in the target
                        value = g_pmdConfig->lightLevelToPwm0[level];
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
                        // Change value range from [0, 255] (for HAL PWM) to [0, 31] (for PMU PWM)
                        value = (value & 0xff) >> 3;
                        UINT32 dutyCycleMask, dutyCycle;
                        if (id == PMD_LEVEL_LED0)
                        {
                            dutyCycleMask = RDA_PMU_PWM_R_DUTY_CYCLE_MASK;
                            dutyCycle = RDA_PMU_PWM_R_DUTY_CYCLE(value);
                        }
                        else if (id == PMD_LEVEL_LED1)
                        {
                            dutyCycleMask = RDA_PMU_PWM_G_DUTY_CYCLE_MASK;
                            dutyCycle = RDA_PMU_PWM_G_DUTY_CYCLE(value);
                        }
                        else if (id == PMD_LEVEL_LED2)
                        {
                            dutyCycleMask = RDA_PMU_PWM_B_DUTY_CYCLE_MASK;
                            dutyCycle = RDA_PMU_PWM_B_DUTY_CYCLE(value);
                        }
                        else if (id == PMD_LEVEL_LED3)
                        {
                            dutyCycleMask = PMD_LEVEL_LED3_DUTY_CYCLE_MASK;
                            dutyCycle = PMD_LEVEL_LED3_DUTY_CYCLE(value);
                        }
                        else // PMD_LEVEL_LED3 or PMD_LEVEL_KEYPAD
                        {
                           dutyCycleMask = PMD_LEVEL_KEYPAD_DUTY_CYCLE_MASK;
                           dutyCycle = PMD_LEVEL_KEYPAD_DUTY_CYCLE(value);
                        }
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
                        // Change value range from [0, 255] (for HAL PWM) to [0, 7] (for PMU DIM)
                        value = (value & 0xff) >> 5;
                        UINT32 ibitMask, ibit;
                        if (id == PMD_LEVEL_LED0)
                        {
                            ibitMask = RDA_PMU_LED_R_IBIT_MASK;
                            ibit = RDA_PMU_LED_R_IBIT(value);
                        }
                        else if (id == PMD_LEVEL_LED1)
                        {
                            ibitMask = RDA_PMU_LED_G_IBIT_MASK;
                            ibit = RDA_PMU_LED_G_IBIT(value);
                        }
                        else if (id == PMD_LEVEL_LED2)
                        {
                            ibitMask = RDA_PMU_LED_B_IBIT_MASK;
                            ibit = RDA_PMU_LED_B_IBIT(value);
                        }
                        else if (id == PMD_LEVEL_LED3)
                        {
                            ibitMask = PMD_LEVEL_LED3_MASK;
                            ibit = RDA_PMU_LED_B_IBIT(value);
                        }
                        else // PMD_LEVEL_KEYPAD
                        {
                            ibitMask = PMD_LEVEL_KEYPAD_MASK;
                            ibit = RDA_PMU_LED_R_IBIT(value)|
                                   RDA_PMU_LED_G_IBIT(value)|
                                   RDA_PMU_LED_B_IBIT(value);
                        }
#endif // !ENABLE_KEYPAD_LIGHT_PWM
                        UINT32 mask;
                        PMD_BG_LP_MODE_USER_ID_T bgUser;
                        PMD_CLK4M_AVDD3_USER_ID_T clk4mUser;
                        if (id == PMD_LEVEL_LED0)
                        {
                            mask = RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_LED0;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED0;
                        }
                        else if (id == PMD_LEVEL_LED1)
                        {
                            mask = RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_G_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_LED1;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED1;
                        }
                        else if (id == PMD_LEVEL_LED2)
                        {
                            mask = RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_LED2;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED2;
                        }
                        else if (id == PMD_LEVEL_LED3)
                        {
                            mask = PMD_LEVEL_LED3_MASK;
                            bgUser = PMD_BG_LP_MODE_USER_LED3;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_LED3;
                        }
                        else // PMD_LEVEL_KEYPAD
                        {
                            mask = RDA_PMU_LED_R_OFF_ACT|RDA_PMU_LED_R_OFF_LP|
                                   RDA_PMU_LED_G_OFF_ACT|RDA_PMU_LED_G_OFF_LP|
                                   RDA_PMU_LED_B_OFF_ACT|RDA_PMU_LED_B_OFF_LP;
                            bgUser = PMD_BG_LP_MODE_USER_KEYPAD_LED;
                            clk4mUser= PMD_CLK4M_AVDD3_USER_KEYPAD_LED;
                        }
                        // Keypad/led power dependency setting in LP mode
                        PMD_LDO_ID_T ldo;
                        ldo.opal = PMD_LDO_BLLED;
                        pmd_EnableOpalLdo(ldo, (level != 0),
                            PMD_PROFILE_MODE_LOWPOWER, LEVEL_LDO_USER(id));
                        pmd_EnableOpalLdo(ldo, (level != 0),
                            PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));

                        pmd_ChargerSetBgLpMode(bgUser, (level == 0));
                        pmd_EnableClk4mAvdd3InLpMode(clk4mUser, (level != 0));
                        // Keypad/led power setting
                        status = hal_SysEnterCriticalSection();
                        if (0 == level)
                        {
                            g_pmdLedSetting5 |= mask;
                        }
                        else
                        {
                            g_pmdLedSetting5 &= ~mask;
#if (ENABLE_KEYPAD_LIGHT_PWM)
#if (KEYPAD_LIGHT_PWM_BY_PMU)
                            g_pmdLedSetting4 = (g_pmdLedSetting4 & (~dutyCycleMask)) |
                                               dutyCycle;
                            pmd_RDAWrite(RDA_ADDR_LED_SETTING4, g_pmdLedSetting4);
#else
                            hal_PwlSelLevel(HAL_PWL_0, value);
#endif
#else // !ENABLE_KEYPAD_LIGHT_PWM
                            g_pmdLedSetting5 = (g_pmdLedSetting5 & (~ibitMask)) |
                                               ibit;
#endif// !ENABLE_KEYPAD_LIGHT_PWM
                        }
                        hal_SysExitCriticalSection(status);

                        pmd_RDAWrite(RDA_ADDR_LED_SETTING5, g_pmdLedSetting5);

                    }
                    break;
                case PMD_LEVEL_LOUD_SPEAKER:
                    // Speaker level is controlled by ABB rather than PMU on Gallite
                default:
                    PMD_ASSERT(FALSE, "Trying to use an invalid id for type PMD_LEVEL_TYPE_OPAL", id);
                    break;
            }
            break;
        case PMD_LEVEL_TYPE_BACKLIGHT:
            {
                if (level > 7)
                {
                    level = 7;
                }
                //PMD_TRACE(PMD_DBG_TRC|TSTDOUT, 0,  "PMD_LEVEL_TYPE_BACKLIGHT level=%d",level);

                value = g_pmdConfig->lightLevelToBacklight[level];
                // Change value range from [0, 255] (for HAL PWM) to [0, 15] (for PMU DIM)
                value = (value & 0xff) >> 4;

                status = hal_SysEnterCriticalSection();
                if (0 == level)
                {
                    g_pmdLedSetting2 |= RDA_PMU_BL_OFF_ACT;
                    if (g_pmdScreenAntiFlickerEnabled)
                    {
#if (LCD_BACKLIGHT_CHARGE_PUMP)
                        // Not to direct-reg charge pump
                        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_PU_CP_DR|RDA_PMU_BYPASS_CP_DR);
#else
                        g_pmdLdoProfileSetting5.vRgbLedIbit = g_pmdBlLedOutDefault.vRgbLedIbit;
                        g_pmdLdoProfileSetting5.vRgbLedVsel = g_pmdBlLedOutDefault.vRgbLedVsel;
#endif
                    }
                }
                else
                {
                    g_pmdLedSetting2 &= ~RDA_PMU_BL_OFF_ACT;
                    g_pmdLedSetting2 = PMU_SET_BITFIELD(g_pmdLedSetting2,
                                            RDA_PMU_BL_IBIT_ACT,
                                            value);
                    if (g_pmdScreenAntiFlickerEnabled)
                    {
#if (LCD_BACKLIGHT_CHARGE_PUMP)
                        // Direct-reg to enable charge pump
                        //different from 8809
                        g_pmdSpeakerPaSetting1 |= RDA_PMU_PU_CP_DR|RDA_PMU_BYPASS_CP_DR;
                        g_pmdSpeakerPaSetting1 |= RDA_PMU_PU_CP_REG;
                        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_REG);
#else
                        g_pmdLdoProfileSetting5.vRgbLedIbit = gc_pmdBlLedOutAntiFlicker.vRgbLedIbit;
                        g_pmdLdoProfileSetting5.vRgbLedVsel = gc_pmdBlLedOutAntiFlicker.vRgbLedVsel;
#endif
                    }
                }
			{				
				PMD_LDO_ID_T ldo;
				ldo.opal = PMD_LDO_BLLED;	
				pmd_EnableOpalLdo(ldo, (level != 0),
				PMD_PROFILE_MODE_NORMAL, LEVEL_LDO_USER(id));	
			}
                pmd_RDAWrite(RDA_ADDR_LED_SETTING2, g_pmdLedSetting2);
                if (g_pmdScreenAntiFlickerEnabled)
                {
#if (LCD_BACKLIGHT_CHARGE_PUMP)
                    //hal_AudForceDcdc4ClockOn((level != 0));
                    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);
#else
                    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);
#endif
                }
                hal_SysExitCriticalSection(status);
            }
            break;
        default:
            PMD_ASSERT(FALSE, "Trying to use an invalid type of level", g_pmdConfig->level[id].type);
            break;
    }

    if (id == PMD_LEVEL_LCD)
    {
        if (0 == level)
        {
            g_pmdBacklightOn = FALSE;
            pmd_SetHighActivity(FALSE);
        }
        else
        {
            g_pmdBacklightOn = TRUE;
            pmd_SetHighActivity(TRUE);
        }
    }

    //g_pmdMapAccess.levelInfo[id]=level;
}

#if 0
PUBLIC UINT32 pmd_CalibStartCustom(UINT32 command,
                                   CALIB_PMD_T* pmdCalibStruct)
{
  return 1;
}


// ============================================================================
// pmd_CalibGetDefault
// ----------------------------------------------------------------------------
/// Returns the default values of the PMD calibration.
/// Used by the Calib module to get the default of each PMD implementation.
///
/// @return Default values for the PMD calib parameters.
// ============================================================================
PUBLIC CONST CALIB_PMD_T* pmd_CalibGetDefault(VOID)
{
    return &g_pmdCalibDefault;
}
#endif

// ============================================================================
// pmd_EnableMemoryPower
// ----------------------------------------------------------------------------
/// Power up/down memory
// ============================================================================
PUBLIC VOID pmd_EnableMemoryPower(BOOL on)
{
    PMD_LDO_ID_T ldo;
    ldo.opal = PMD_LDO_MEM;
    pmd_EnableOpalLdo(ldo, on, PMD_PROFILE_MODE_NORMAL, DEFAULT_LDO_USER);
}


// ============================================================================
// pmd_InitCoreVoltage
// ----------------------------------------------------------------------------
/// Init the core voltage level in active/idle mode
// ============================================================================
PUBLIC VOID pmd_InitCoreVoltage(VOID)//same as 8809
{
    // Init DCDC frequency
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK1, gc_pmdLdoBuck1);
    pmd_OpalSpiWrite(RDA_ADDR_LDO_BUCK2, gc_pmdLdoBuck2);

    // Set DCDC VCORE
    UINT32 vcore;
    vcore = g_pmdDcdcVcoreValueLp;
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                      RDA_PMU_VBUCK1_BIT_LP,
                                      vcore);
    g_pmdDcdcVcoreActCurIndex = PMD_CORE_VOLTAGE_LOW;
    vcore = g_pmdDcdcVcoreValueActiveAry[g_pmdDcdcVcoreActCurIndex];
    g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                      RDA_PMU_VBUCK1_BIT_NLP,
                                      vcore);
    pmd_OpalSpiWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);
}


// ============================================================================
// pmd_SetCoreVoltage
// ----------------------------------------------------------------------------
/// Set the core voltage level in active mode
// ============================================================================
PUBLIC VOID pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_ID_T user,
                               PMD_CORE_VOLTAGE_T coreVolt)
{
#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0)
    // It has risks to dynamically change DCDC VCORE, as
    // DCDC VCORE voltage stable time is about 100 us in a voltage change.
    if (g_pmdInitDone) return;
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 0

    g_pmdDcdcVcoreActRegistry[user] = coreVolt;

    UINT32 scStatus = hal_SysEnterCriticalSection();

    UINT32 vcoreIndex = 0;
    for (UINT32 i=0; i<PMD_CORE_VOLTAGE_USER_ID_QTY; i++)
    {
        if (vcoreIndex < g_pmdDcdcVcoreActRegistry[i])
        {
            vcoreIndex = g_pmdDcdcVcoreActRegistry[i];
        }
    }

    if (g_pmdDcdcVcoreActCurIndex != vcoreIndex)
    {
#ifdef FM_LDO_WORKAROUND
        if (g_pmdDcdcVcoreActCurIndex < PMD_CORE_VOLTAGE_HIGH &&
            vcoreIndex == PMD_CORE_VOLTAGE_HIGH)
        {
        	g_pmdHighVcoreReqFlag = TRUE;
        	if(g_pmdLdoReqFlag)
        	{
            	pmd_DcdcPower_Control(TRUE);
        	}
        }
#endif

        // Set DCDC VCORE
        UINT32 vcore = g_pmdDcdcVcoreValueActiveAry[vcoreIndex];

    	g_pmdDcdcBuck = PMU_SET_BITFIELD(g_pmdDcdcBuck,
                                      	RDA_PMU_VBUCK1_BIT_NLP,
                                      	vcore);
        pmd_RDAWrite(RDA_ADDR_DCDC_BUCK, g_pmdDcdcBuck);

#ifdef FM_LDO_WORKAROUND
        if (g_pmdDcdcVcoreActCurIndex == PMD_CORE_VOLTAGE_HIGH &&
            vcoreIndex < PMD_CORE_VOLTAGE_HIGH)
        {
        	g_pmdHighVcoreReqFlag = FALSE;
        	if(g_pmdLdoReqFlag)
        	{
            	pmd_DcdcPower_Control(FALSE);
        	}
        }
#endif

        g_pmdDcdcVcoreActCurIndex = vcoreIndex;
    }

    hal_SysExitCriticalSection(scStatus);

#if (DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1)
#if 0
    // Wait until the voltage becomes stable
    if (g_pmdInitDone)
    {
        // DCDC VCORE voltage stable time is about 100 us in a voltage change
        // Freq might be changed in IRQ context, e.g., PAL
        hal_TimDelay(2);
    }
#endif
#endif // DYNAMIC_DCDC_VCORE_ADJUSTMENT == 1
}

PUBLIC VOID pmd_SetMemVoltage(UINT8 level)
{
	UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoProfileSetting3 = 
        PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                         RDA_PMU_VMEM_IBIT,
                         level);
    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_RegWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);
    hal_SysExitCriticalSection(scStatus);
}

//vio2p8 in 5855 equals pad in 8809
PUBLIC VOID pmd_SetVIO2P8Voltage(UINT8 level)
{
	UINT32 scStatus = hal_SysEnterCriticalSection();
    g_pmdLdoProfileSetting3 = 
        PMU_SET_BITFIELD(g_pmdLdoProfileSetting3,
                         RDA_PMU_VIO2P8_IBIT,
                         level);
    pmd_RegWrite(RDA_ADDR_LDO_ACTIVE_SETTING3, g_pmdLdoProfileSetting3);
    pmd_RegWrite(RDA_ADDR_LDO_LP_SETTING3, g_pmdLdoProfileSetting3);
    hal_SysExitCriticalSection(scStatus);
}

VOID pmd_InitTouchScreen(VOID)
{
    UINT16 reg;
    // Set sar_vref_bit
    reg = pmd_OpalSpiRead(RDA_ADDR_TOUCH_SCREEN_CONTROL);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_SAR_VERF_BIT, 2); 
    pmd_OpalSpiWrite(RDA_ADDR_TOUCH_SCREEN_CONTROL, reg);

    // Set delay_before_samp and timer_penirq_stable
    reg = pmd_OpalSpiRead(RDA_ADDR_AUDIO_CONTROL);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_DELAY_BEFORE_SAMP, 3);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_TIMER_PEN_IRQ_STABLE, 3);
#ifdef _USED_TSC_KEY_
    // Increase the sample interval for TSC key
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_TIMER_INT_TO_X_SEL, 7);
#endif
    pmd_OpalSpiWrite(RDA_ADDR_AUDIO_CONTROL, reg);
}

VOID pmd_InitSpeakerPa(VOID)
{
    // For U03/U04 chips, L_CLG and R_CLG should be reset at boot
    // time, in order to reduce current leak.
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING2, 
        gc_pmdSpeakerPaSetting2|RDA_PMU_INTERFACE_MODE_EN);
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING4, 
        RDA_PMU_RESET_R_CLG_DR|RDA_PMU_RESET_L_CLG_DR);
    hal_TimDelay(1 MS_WAITING);
    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING4, 0);

    pmd_OpalSpiWrite(RDA_ADDR_SPEAKER_PA_SETTING2, gc_pmdSpeakerPaSetting2);
    // Init PA setting 1 & 3
    pmd_SpeakerPaRestoreMode();
}

/******************************************************************************
 * pmd_InitXtal32K: 
 * DESCRIPTION: - 
 * 
 * Input: 
 * Output: 
 * Returns: 
 * 
 * 
 ******************************************************************************/
VOID pmd_InitXtal32K(VOID)
{
    UINT16 reg;

    // Should pu 6p5m & 32.768k before power on FM, BT.
    reg = pmd_RDARead(RDA_ADDR_XTAL_CTRL1);
    reg |= RDA_PMU_ENABLE_CLK_6P5M;
    reg &= ~RDA_PMU_LP_MODE_EN_DR;
    pmd_RDAWrite(RDA_ADDR_XTAL_CTRL1,reg);

    //g_CdacDelta
    reg = 0xa410; // TODO: Get from calib data
    pmd_RDAWrite(RDA_ADDR_XTAL_SET_NOR,reg);

    reg = pmd_RDARead(RDA_ADDR_REG68);
    reg = PMU_SET_BITFIELD(reg, RDA_PMU_XDRV_AUX_STEP_EN, 0XC);
    pmd_RDAWrite(RDA_ADDR_REG68,reg);

    //Switch to precision 32K clock, and disable Xtal bt enable bit
    reg = pmd_RDARead(RDA_ADDR_32K_CTRL);
    reg |= RDA_PMU_CK32K_26MXTAL_CLK_EN;
    reg &= ~(RDA_PMU_CK32K_LPO_CLK_EN | RDA_PMU_XEN_BT_ENABLE);
    pmd_RDAWrite(RDA_ADDR_32K_CTRL,reg);

}

PUBLIC VOID pmd_SetDoubler(BOOL on)
{
#if 0
    UINT16 reg = 0;
    BOOL status = FALSE;
    UINT32 scStatus;
    
    scStatus = hal_SysEnterCriticalSection();

    if(hal_IspiCsActivate(HAL_ISPI_CS_PMU))
    {
        reg = pmd_OpalSpiRead(RDA_ADDR_XTAL_DOUBLER_SETTING);

        if(on)
        {
            reg |= RDA_PMU_XTAL_DOUBLER_SETTING_XTAL_DOUBLER_EN;
        }
        else
        {
            reg &= ~RDA_PMU_XTAL_DOUBLER_SETTING_XTAL_DOUBLER_EN;
        }

        pmd_OpalSpiWrite(RDA_ADDR_XTAL_DOUBLER_SETTING,reg);
    }
    else
    {
        PMD_ASSERT(0, "pmd_SetDoubler Failed!")
    }

    hal_SysExitCriticalSection(scStatus);
#endif
}

VOID pmd_InitRdaPmu(VOID)
{
	// Init Vcore in active & lp mode
    pmd_InitCoreVoltage();

	// Set Vmem voltage in active & lp mode
	//pmd_SetMemVoltage(7); //delete by huangyx 20160419

	// Set Vpad voltage in active & lp mode
	pmd_SetVIO2P8Voltage(7);

    // Disable touch screen irqs (they will be enabled later if needed)
    // Disable key irq
    // Enable charger irq
    // Enter active mode
    g_pmdIrqSettingLatest = 0;

#ifdef _USED_TSC_KEY_                  
    g_pmdLdoSettings.keyEnable = TRUE;//add by sjj for enable key_enable
#else
	g_pmdLdoSettings.tscEnable = TRUE;
#endif

	g_tsdConfig = tgt_GetTsdConfig();//add by sjj 2016.3.3

	
#if 1//add by sjj for simulator
    pmd_RegIrqSettingSet(/*RDA_PMU_EOMIRQ_MASK|*/RDA_PMU_PENIRQ_MASK|
                         RDA_PMU_KEYIRQ_MASK|RDA_PMU_HOLD_PRECHARGER_EFUSE/*|
                         RDA_PMU_PD_MODE_SEL*/);
#endif
    // Disable headphone detection irqs (they will be enabled later if needed)
    // Enter active mode
    g_pmdHpDetectSettingLatest = 0;
    //pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);//delete by huangyx 20160419

    // Change the LDO settings in active mode
    //pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);//delete by huangyx 20160419

    // Init sar_vref_bit for GPADC and TSD
    pmd_InitTouchScreen();
    // Init speaker PA
    pmd_InitSpeakerPa();
    // Init charger
    pmd_InitCharger();

    pmd_InitXtal32K();
}

PUBLIC UINT32 pmd_GetBatteryGpadcChannel(VOID)
{
    return g_pmdConfig->batteryGpadcChannel;
}

PUBLIC UINT16 pmd_GetScreenAntiFlickerVolt(VOID)
{
    UINT16 volt = g_pmdConfig->batteryMVScreenAntiFlicker;
    if (volt == 0)
    {
        // Return a large value to assume always on
        volt = 10000;
    }
    return volt;
}

PUBLIC VOID pmd_EnableScreenAntiFlicker(BOOL enable)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdScreenAntiFlickerEnabled = enable;

#if (LCD_BACKLIGHT_CHARGE_PUMP)

    if (enable)
    {
        // Direct-reg to enable charge pump
        // different from 8809
        g_pmdSpeakerPaSetting1 |= RDA_PMU_PU_CP_DR|RDA_PMU_BYPASS_CP_DR;
        g_pmdSpeakerPaSetting1 |= RDA_PMU_PU_CP_REG;
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_REG);
    }
    else
    {
        // Not to direct-reg charge pump
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_PU_CP_DR|RDA_PMU_BYPASS_CP_DR);
    }
    hal_AudForceDcdc4ClockOn(enable);
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

#else // !LCD_BACKLIGHT_CHARGE_PUMP

    if (enable)
    {
        g_pmdLdoProfileSetting5.vRgbLedIbit = gc_pmdBlLedOutAntiFlicker.vRgbLedIbit;
        g_pmdLdoProfileSetting5.vRgbLedVsel = gc_pmdBlLedOutAntiFlicker.vRgbLedVsel;
    }
    else
    {
        g_pmdLdoProfileSetting5.vRgbLedIbit = g_pmdBlLedOutDefault.vRgbLedIbit;
        g_pmdLdoProfileSetting5.vRgbLedVsel = g_pmdBlLedOutDefault.vRgbLedVsel;
    }
    pmd_RDAWrite(RDA_ADDR_LDO_ACTIVE_SETTING5, g_pmdLdoProfileSetting5.reg);

#endif // !LCD_BACKLIGHT_CHARGE_PUMP

    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID pmd_SpeakerPaCpLowOutput(BOOL low)
{
    g_pmdSpeakerPaCpLowOutput = low;
}

PRIVATE VOID pmd_SpeakerPaSetCpLevel(UINT32 level)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    g_pmdSpeakerPaSetting1 = 
        PMU_SET_BITFIELD(g_pmdSpeakerPaSetting1, 
                         RDA_PMU_VCP_OUT_BIT,
                         level);
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);
    hal_SysExitCriticalSection(scStatus);
}

PUBLIC VOID pmd_SpeakerPaPreEnable(BOOL enable)
{
    // Class K mode might need a higher vcore voltage and a higher charge pump
    if (g_pmdSpeakerPaEnabled != enable)
    {
        if (enable)
        {
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_CP, PMD_CORE_VOLTAGE_MEDIUM);
            hal_TimDelay(4);
            if (!g_pmdSpeakerPaCpLowOutput)
            {
#if (AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT)
                // Allow CP to output 5V - 6V
                g_pmdThermalCalib |= RDA_PMU_LED_MODE_CP;
                pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
                hal_TimDelay(4);
#endif
                for (int i=1; i<=15; i++)
                {
                    pmd_SpeakerPaSetCpLevel(i);
                    hal_TimDelay(4);
                }
            }
#if (AUD_SPK_LINE_L_R_DOUBLE)
            if (g_pmdSpeakerPaSetting3 & RDA_PMU_R_ENABLE_CLG)
            {
                // Disable R line output
                pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, 
                             g_pmdSpeakerPaSetting3 & ~RDA_PMU_R_ENABLE_CLG);
            }
#endif
        }
        else
        {
        }
    }
}

PUBLIC VOID pmd_SpeakerPaPostEnable(BOOL enable)
{
    // Class K mode might need a higher vcore voltage and a higher charge pump
    // output voltage.
    // However, we do not know the actual mode of the speaker to be used, so
    // we always treat it as in class K mode.
    if (g_pmdSpeakerPaEnabled != enable)
    {
        if (enable)
        {
#if (AUD_SPK_LINE_L_R_DOUBLE)
            if (!g_pmdSpeakerPaCpLowOutput)
            {
                if (g_pmdSpeakerPaSetting3 & RDA_PMU_R_ENABLE_CLG)
                {
                    hal_TimDelay(4);
                    // Re-enable R line output
                    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, 
                                 g_pmdSpeakerPaSetting3);
                }
            }
#endif
        }
        else
        {
            if (!g_pmdSpeakerPaCpLowOutput)
            {
                for (int i=14; i>=0; i--)
                {
                    hal_TimDelay(4);
                    pmd_SpeakerPaSetCpLevel(i);
                }
#if (AUD_SPK_CHARGE_PUMP_HIGH_OUTPUT)
                hal_TimDelay(4);
                // Restrict CP to output 4.5V - 5.6V
                g_pmdThermalCalib &= ~RDA_PMU_LED_MODE_CP;
                pmd_RDAWrite(RDA_ADDR_THERMAL_CALIBRATION, g_pmdThermalCalib);
#endif
            }
            hal_TimDelay(4);
            pmd_SetCoreVoltage(PMD_CORE_VOLTAGE_USER_CP, PMD_CORE_VOLTAGE_LOW);
        }
        g_pmdSpeakerPaEnabled = enable;
    }
}

PUBLIC VOID pmd_SpeakerPaSelectChannel(BOOL leftChanEn, BOOL rightChanEn)
{
    UINT16 chanSel = 0;
    chanSel |= (RDA_PMU_PU_O_R_CLG_REG|RDA_PMU_PU_O_R_CLG_DR|
                RDA_PMU_PU_R_CLG_REG|RDA_PMU_PU_R_CLG_DR|
                RDA_PMU_PU_O_L_CLG_REG|RDA_PMU_PU_O_L_CLG_DR|
                RDA_PMU_PU_L_CLG_REG|RDA_PMU_PU_L_CLG_DR);
    if (!leftChanEn)
    {
        chanSel &= ~(RDA_PMU_PU_L_CLG_DR|RDA_PMU_PU_L_CLG_REG|
                RDA_PMU_PU_O_L_CLG_DR|RDA_PMU_PU_O_L_CLG_REG);
    }
    if (!rightChanEn)
    {
        chanSel &= (RDA_PMU_PU_R_CLG_DR|RDA_PMU_PU_R_CLG_REG|
                RDA_PMU_PU_O_R_CLG_DR|RDA_PMU_PU_O_R_CLG_REG);
    }
    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING4, chanSel);
}

//no change in 5855
PUBLIC VOID pmd_SpeakerPaSetMode(PMD_SPEAKER_PA_MODE_T leftChanMode,
                                 PMD_SPEAKER_PA_MODE_T rightChanMode)
{
#if (LCD_BACKLIGHT_CHARGE_PUMP)

    PMD_ASSERT( (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_K &&
                 rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_K),
                "PA class K mode expected. %d/%d",
                leftChanMode, rightChanMode);

    // The PA modes of both channels have been initialized
    // to class K, and there is no need to change them here.

#else // !LCD_BACKLIGHT_CHARGE_PUMP

    UINT32 scStatus = hal_SysEnterCriticalSection();

    // Set speaker modes
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_DMODE_EN_CLG_MASK;

    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
    {
        if (rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLAB_L_CLAB;
        }
        else
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLD_L_CLAB;
        }
    }
    else
    {
        if (rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLAB_L_CLD;
        }
        else
        {
            g_pmdSpeakerPaSetting3 |= RDA_PMU_DMODE_R_CLD_L_CLD;
        }
    }

#if (AUD_SPK_LINE_L_R_DOUBLE)
    PMD_ASSERT(leftChanMode == rightChanMode, 
        "Speaker L&R mode different when L-R-double mode enabled");
    // Disable speaker right line if L-R-doulbe mode is enabled,
    // and both left and right speaker lines are in class AB mode.
    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB &&
        rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_AB)
    {
        g_pmdSpeakerPaSetting3 &= ~RDA_PMU_R_ENABLE_CLG;
    }
    else
    {
        g_pmdSpeakerPaSetting3 |= RDA_PMU_R_ENABLE_CLG;
    }
#endif

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);

    // Set CP direct-reg controls
    if (leftChanMode == PMD_SPEAKER_PA_MODE_CLASS_K &&
        rightChanMode == PMD_SPEAKER_PA_MODE_CLASS_K)
    {
        // Not to direct-reg bypass CP
        g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG);
    }
    else
    {
        // Direct-reg to bypass CP
        g_pmdSpeakerPaSetting1 |= RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG;
    }

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

    hal_SysExitCriticalSection(scStatus);

#endif // !LCD_BACKLIGHT_CHARGE_PUMP
}

PUBLIC VOID pmd_SpeakerPaRestoreMode(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // Restore CP direct-reg controls
#if (AUD_SPK_LINE_NO_CLASS_K) || \
    (AUD_SPK_LINE_L_CLASS_AB) || (AUD_SPK_LINE_R_CLASS_AB)
    // Direct-reg to bypass CP
    g_pmdSpeakerPaSetting1 |= RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG;
#else
    // Not to direct-reg bypass CP
    g_pmdSpeakerPaSetting1 &= ~(RDA_PMU_BYPASS_CP_DR|RDA_PMU_BYPASS_CP_REG);
#endif

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING1, g_pmdSpeakerPaSetting1);

    // Restore speaker modes
    g_pmdSpeakerPaSetting3 &= ~RDA_PMU_DMODE_EN_CLG_MASK;
    g_pmdSpeakerPaSetting3 |= 
#if (AUD_SPK_LINE_L_CLASS_AB) && (AUD_SPK_LINE_R_CLASS_AB)
                    RDA_PMU_DMODE_R_CLAB_L_CLAB
#elif (AUD_SPK_LINE_L_CLASS_AB)
                    RDA_PMU_DMODE_R_CLD_L_CLAB
#elif (AUD_SPK_LINE_R_CLASS_AB)
                    RDA_PMU_DMODE_R_CLAB_L_CLD
#else // Both L and R lines are in class D mode
                    RDA_PMU_DMODE_R_CLD_L_CLD
#endif
                    ;

    // Disable speaker right line if L-R-doulbe mode is enabled,
    // and both left and right speaker lines are in class AB mode.
    g_pmdSpeakerPaSetting3 &= ~(RDA_PMU_R_ENABLE_CLG|RDA_PMU_L_ENABLE_CLG);
    g_pmdSpeakerPaSetting3 |= 
#if (AUD_SPK_LINE_L_R_DOUBLE == 0) || \
    ((AUD_SPK_LINE_L_CLASS_AB == 0) && (AUD_SPK_LINE_R_CLASS_AB == 0))
                    RDA_PMU_R_ENABLE_CLG|
#endif
                    RDA_PMU_L_ENABLE_CLG;

    pmd_RDAWrite(RDA_ADDR_SPEAKER_PA_SETTING3, g_pmdSpeakerPaSetting3);

    hal_SysExitCriticalSection(scStatus);
}

VOID pmd_TsdClearEomIrqInternal(VOID)
{
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR);
}

BOOL pmd_ResetTouch(VOID)
{
    UINT32 scStatus;
    scStatus= hal_SysEnterCriticalSection();

    g_pmdDebug3 |= RDA_PMU_RESETN_TSC_DR;
    g_pmdDebug3 &= ~RDA_PMU_RESETN_TSC_REG;
    pmd_RDAWrite(RDA_ADDR_DEBUG3, g_pmdDebug3);

    g_pmdDebug3 &= ~RDA_PMU_RESETN_TSC_DR;
    g_pmdDebug3 |= RDA_PMU_RESETN_TSC_REG;
    pmd_RDAWrite(RDA_ADDR_DEBUG3, g_pmdDebug3);

    hal_SysExitCriticalSection(scStatus);
    return TRUE;
}

VOID pmd_TsdClearTouchIrq(VOID)
{   
#ifdef _USED_TSC_KEY_	
	pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);
#else
    pmd_RegIrqSettingSet(RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_PENIRQ_CLEAR);
#endif
}

BOOL pmd_TsdTscEnabled(VOID)
{
#ifdef _USED_TSC_KEY_
	return g_pmdLdoSettings.keyEnable;
#else
    return g_pmdLdoSettings.tscEnable;
#endif
}

VOID pmd_TsdEnableTsc(BOOL enable)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

#ifdef _USED_TSC_KEY_
    g_pmdLdoSettings.keyEnable = enable;
#else
	g_pmdLdoSettings.tscEnable = enable;
#endif
    pmd_OpalSpiWrite(RDA_ADDR_LDO_SETTINGS, g_pmdLdoSettings.reg);

    if (!enable)
    {
        pmd_ResetTouch();
    }

    hal_SysExitCriticalSection(scStatus);
}

BOOL pmd_TsdEnableIrq(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();
    UINT32 reg_data = 0;

    //make gpio_p04 dedicated to key
    reg_data = pmd_RDARead(RDA_ADDR_SYS_CTRL);
    pmd_RDAWrite(RDA_ADDR_SYS_CTRL,reg_data&(~RDA_PMU_SYS_CTRL_IO_KEYSENSE_ENB));

    // enable touch screen
    pmd_TsdEnableTsc(TRUE);
#if 1
    // clear previous pen irq
    pmd_TsdClearTouchIrq();

#ifdef _USED_TSC_KEY_
    // unmask TSC KEY
    pmd_RegIrqSettingClr(RDA_PMU_KEYIRQ_MASK);

#ifdef _USED_TSC_KEY_IN_LP_MODE_
    // Disable bandgap LP mode
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_TSC_KEY, FALSE);
    // Enable clock-4M and avdd3 in LP mode
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_TSC_KEY, TRUE);
#endif

#else
    // unmask pen irq
    pmd_RegIrqSettingClr(RDA_PMU_PENIRQ_MASK);
#endif
#endif
    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}

BOOL pmd_TsdDisableIrq(VOID)
{
    UINT32 scStatus = hal_SysEnterCriticalSection();

    // disable touch screen
    pmd_TsdEnableTsc(FALSE);

#ifdef _USED_TSC_KEY_
	// mask TSC KEY
	pmd_RegIrqSettingSet(RDA_PMU_KEYIRQ_MASK);

#ifdef _USED_TSC_KEY_IN_LP_MODE_
    // Enable bandgap LP mode
    pmd_ChargerSetBgLpMode(PMD_BG_LP_MODE_USER_TSC_KEY, TRUE);
    // Disable clock-4M and avdd3 in LP mode
    pmd_EnableClk4mAvdd3InLpMode(PMD_CLK4M_AVDD3_USER_TSC_KEY, FALSE);
#endif

#else
    // mask pen irq
    pmd_RegIrqSettingSet(RDA_PMU_PENIRQ_MASK);
#endif
    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}

// NOTE:
// Both the Charger detection and the headphone detection are now
// run in TIMER IRQ: pmd_IrqHanderInternal, pmd_ChargerManager,
// and pmd_EarDetectDelay.
// They will be synchronized in nature and 
// there is no need to lock interrupt.
#ifdef ABB_HP_DETECT
PRIVATE VOID pmd_IrqHanderInternal(UINT32 reqType)
{
    UINT32 nextAction = 0;
    UINT16 irq = 0;
    UINT16 chrStatus = 0;
#ifdef ABB_HP_DETECT
    UINT16 hpDetect= 0;
#endif

	hal_HstSendEvent(SYS_EVENT, 0x14041006);

    if (FALSE == pmd_SpiCsNonblockingActivate(FALSE))
    {
        // Never stop PMD IRQ function timers, as they have different jobs.
        sxr_StartFunctionTimer(PMD_TIME_IRQ_ACT_CS_RETRY, pmd_IrqHanderInternal, reqType, 0);
        return;
    }

    // Read charger IRQ status
    if (reqType & PMD_IRQ_DEBOUNCE_REQ)
    {
        irq = pmd_OpalSpiRead(RDA_ADDR_IRQ_SETTINGS);
    }
    // Read charger status
    if (reqType & PMD_IRQ_CHECK_CHARGER)
    {
        chrStatus = pmd_OpalSpiRead(RDA_ADDR_CHARGER_STATUS);
    }
#ifdef ABB_HP_DETECT
    // Read headphone IRQ status
    hpDetect = pmd_OpalSpiRead(RDA_ADDR_HP_DETECT_SETTING);
#endif

    pmd_OpalCsDeactivate(FALSE);

#if 0
    PMD_TRACE(PMD_INFO_TRC, 0,
        "[%08d] PMD-INT: reqType=%d, irq(%d)=0x%04x, chr=0x%04x",
        hal_TimGetUpTime()*1000/16384, reqType,
        g_pmdChargerInIrqEnabled, irq, chrStatus);
#ifdef ABB_HP_DETECT
    PMD_TRACE(PMD_INFO_TRC, 0,
        "-- hp(%d)=0x%04x, hpStart=%d",
        g_pmdHpOutIrqEnabled, hpDetect, g_pmdHpOutDetectStarted);
#endif
#endif

    if (reqType & PMD_IRQ_DEBOUNCE_REQ)
    {
        // Check IRQ source
        if (irq & RDA_PMU_INT_CHR_ON)
        {
            pmd_RegIrqSettingSet(RDA_PMU_INT_CHR_CLEAR);
            if (g_pmdChargerInIrqEnabled)
            {
                nextAction |= PMD_IRQ_CHECK_CHARGER;
            }
        }
#ifdef ABB_HP_DETECT
        if (hpDetect & RDA_PMU_HP_OUT)
        {
            pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_CLEAR);
            // Check current headphone status
            if ((hpDetect & RDA_PMU_HP_DETECT) == 0)
            {
                if (g_pmdHpOutIrqEnabled)
                {
                    nextAction |= PMD_IRQ_CHECK_HP;
                    if (ear_handler != NULL)
                    {
                        (*ear_handler)(PMD_EAR_STATUS_UNKNOWN);
                    }
                }
            }
        }
#endif
        if (nextAction)
        {
            // Never stop PMD IRQ function timers, as they have different jobs.
            sxr_StartFunctionTimer(PMD_TIME_IRQ_DEBOUNCE, pmd_IrqHanderInternal,
                                   nextAction, 0);
        }
    }

    if (reqType & PMD_IRQ_CHECK_CHARGER)
    {
        if (chrStatus & RDA_PMU_CHR_AC_ON)
        {
            g_pmdChargerInIrqEnabled = FALSE;
            // A timer will be started, and it will read the acutal charger status in non-blocking mode
            pmd_DcOnHandler(TRUE);
        }
        else
        {
            g_pmdChargerInIrqEnabled = TRUE;
        }
    }

#ifdef ABB_HP_DETECT
    if (g_pmdHpOutDetectStarted)
    {
        if (reqType & PMD_IRQ_CHECK_HP)
        {
            // Check current headphone status
            if ((hpDetect & RDA_PMU_HP_DETECT) == 0)
            {
                g_pmdHpOutIrqEnabled = FALSE;
                if (ear_handler != NULL)
                {
                    (*ear_handler)(PMD_EAR_STATUS_OUT);
                }
                // Enable GPIO detection for headphone plug-in
                HAL_GPIO_CFG_T eraCfg;
                eraCfg.irqMask.rising = FALSE;
                eraCfg.irqMask.falling = TRUE;
                eraCfg.irqMask.debounce = TRUE;
                eraCfg.irqMask.level = FALSE;
                hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
                hal_GpioResetIrq(g_ear_gpio);
            }
            else
            {
                g_pmdHpOutIrqEnabled = TRUE;
                // Clear previous PMD_EAR_STATUS_UNKNOWN
                if (ear_handler != NULL)
                {
                    (*ear_handler)(PMD_EAR_STATUS_IN);
                }
            }
        }
    }
#endif

    if (
         // This IRQ does NOT expect to check charger status
         ( (reqType & PMD_IRQ_DEBOUNCE_REQ) &&
           (nextAction & PMD_IRQ_CHECK_CHARGER) == 0 ) ||
         // The debounced charger status has been checked
         (reqType & PMD_IRQ_CHECK_CHARGER)
       )
    {
        if (g_pmdChargerInIrqEnabled)
        {
            pmd_RegIrqSettingClr(RDA_PMU_INT_CHR_MASK);
        }
    }
#ifdef ABB_HP_DETECT
    if (
         // This IRQ does NOT expect to check HP status
         ( (reqType & PMD_IRQ_DEBOUNCE_REQ) &&
           (nextAction & PMD_IRQ_CHECK_HP) == 0 ) ||
         // The debounced HP status has been checked
         (reqType & PMD_IRQ_CHECK_HP)
       )
    {
        if (g_pmdHpOutIrqEnabled)
        {
            pmd_RegHpDetectSettingClr(RDA_PMU_HP_OUT_MASK);
        }
    }
#endif
}
#endif
#if 1

PRIVATE UINT32 pmd_poweronKeyValue;
PUBLIC VOID pmd_SoftStart(VOID)
{
#if 1
	hal_HstSendEvent(BOOT_EVENT, 0x16042701);
	UINT32 key_Data,pData;
	
	//pmd_RDAWrite(RDA_ADDR_SYS_CTRL, pmd_RDARead(RDA_ADDR_SYS_CTRL)|RDA_PMU_SYS_CTRL_I2C_HOST_EN);//i2c_host_en
	//pmd_RDAWrite(RDA_ADDR_LDO_SETTINGS, pmd_RDARead(RDA_ADDR_LDO_SETTINGS)|0x04);//key_enable
	pmd_TsdEnableTsc(TRUE);
	hal_TimDelay(5);

	pmd_RegRead(RDA_ADDR_IRQ_SETTINGS, &pData);
	hal_HstSendEvent(BOOT_EVENT, pData);
	pmd_poweronKeyValue = 0xFFFFFFFF;
#ifndef CODEC_SUPPORT
	if(pData&RDA_PMU_EOMIRQ)
	{
		pmd_RegRead(RDA_ADDR_TOUCH_SCREEN_RESULTS1,&key_Data);

		hal_HstSendEvent(BOOT_EVENT, 0x00099999);
		hal_HstSendEvent(BOOT_EVENT, key_Data);
		pmd_poweronKeyValue = key_Data;
		
		//HAL_ANA_GPADC_MV_T mv = hal_AnaGpadcGpadc2Volt(pmd_poweronKeyValue&0x3ff);
		//hal_HstSendEvent(BOOT_EVENT, 0x0009999a);
	}
#endif
#endif
}

PUBLIC BOOL tsd_KeyOnOffStateAtPowerOn(VOID)
{
    if(0xFFFFFFFF != pmd_poweronKeyValue)
    {
    	UINT16 key_index;
	    CONST TSD_CONFIG_T* tsd_cfg_ptr = tgt_GetTsdConfig();

		hal_HstSendEvent(BOOT_EVENT,0x88338820);
		hal_HstSendEvent(BOOT_EVENT,0x88338821);

        HAL_ANA_GPADC_MV_T mv = hal_AnaGpadcGpadc2Volt(pmd_poweronKeyValue&0x3ff);

		hal_HstSendEvent(BOOT_EVENT, 0x00099998);
    	hal_HstSendEvent(BOOT_EVENT, mv);

        if(tsd_cfg_ptr->keyCount > 1)
        {
            float step = (tsd_cfg_ptr->maxVolt-tsd_cfg_ptr->minVolt)/(tsd_cfg_ptr->keyCount-1);
            float value = ((float)mv-tsd_cfg_ptr->minVolt)/step;
        
            key_index = (UINT16)(value+0.5);
        }
        else
        {
            key_index = 0;
        }

        hal_HstSendEvent(SYS_EVENT, key_index);

        //Skip gpio key
        key_index += tgt_GetTscKeyIndex();  
        
        return (tgt_GetKeyCode(0) == tgt_GetKeyCode(key_index));
    }
	
    return TRUE;
}

PUBLIC BOOL tsd_IsKeyPowerOn(VOID)
{
    return (0xFFFFFFFF != pmd_poweronKeyValue);
}

#endif

PUBLIC VOID pmd_IrqHandler(UINT8 interruptId)
{
#ifndef _USED_TSC_KEY_SPI_  //use spi   
    UINT16 pData;
    UINT16 key_Data;
    UINT8 Pmd_FirstIrqTimer;

    COS_EVENT ev;
#endif

    //ISPI is using
    if(!hal_IspiCsActivate(HAL_ISPI_CS_PMU))
    {
        hal_HstSendEvent(SYS_EVENT, 0X20161218);
        return;
    }


    // Mask all available PMU IRQ sources.
    // IRQ handler cannot be blocked, so we write to PMU directly here.
    // (NOTE: PEN IRQ and KEY IRQ is driven by another interrupt line, 
    // which is connected to GPIO_0 at baseband side.)
#ifdef _USED_TSC_KEY_SPI_  //use spi   
	key_enterInterrupt = 1;
    pmd_RegIrqSettingSet(RDA_PMU_KEYIRQ_MASK|RDA_PMU_EOMIRQ_MASK|RDA_PMU_EOMIRQ_CLEAR|RDA_PMU_KEYIRQ_CLEAR);
    //pmd_RegHpDetectSettingSet(RDA_PMU_HP_IN_MASK|RDA_PMU_HP_OUT_MASK);
	//hal_HstSendEvent(SYS_EVENT, 0x14041046);
#ifndef ROTARY_SWITCH_USED
	sxr_StartFunctionTimer(g_tsdConfig->debounceTime,tsd_Debounce,(VOID*)NULL,0x03);
	//Pmd_IrqTimer = COS_SetTimer(DEBOUNCE_TIME,Pmd_IrqTimerTimeOut ,key_Data, COS_TIMER_MODE_SINGLE);
#endif
    // 1. Never directly invoke pmd_IrqHanderInternal in PMU IRQ.
    //    Invoke it in TIMER IRQ instead for the purpose of synchronization.
    // 2. Never stop PMD IRQ function timers, as they have different jobs.
#if 0
    sxr_StartFunctionTimer(PMD_TIME_IRQ_ACT_CS_RETRY, pmd_IrqHanderInternal,
                           PMD_IRQ_DEBOUNCE_REQ, 0);
#endif

    PMD_TRACE(PMD_INFO_TRC, 0, "[%08d] pmd_IrqHandler",
        hal_TimGetUpTime()*1000/16384);
#else //use i2c
	hal_HstSendEvent(SYS_EVENT, 0x14041047);
	if(i2c_flag == 0)
	{
		error = hal_I2cOpen(HAL_I2C_BUS_ID_2);
		if(error != HAL_ERR_NO)
			hal_HstSendEvent(SYS_EVENT, error);
		i2c_flag = 1;
	}
#if 1	
	
	//error = hal_I2cGetData(HAL_I2C_BUS_ID_2, 0x5f,   0x01, &pData, 2);

	pData = pmd_OpalSpiRead(0x01);
	pData = (pData<<8)|(pData>>8);
	
	hal_HstSendEvent(SYS_EVENT, pData);
	
	pData = pData|0x401a;
	error = hal_I2cSendData(HAL_I2C_BUS_ID_2, 0x5f,   0x01,  &pData, 2);
	if(error != HAL_ERR_NO)
			hal_HstSendEvent(SYS_EVENT, error);
	
	//hal_I2cGetData(HAL_I2C_BUS_ID_2, 0x5f,   0x01, &pData, 2);
	hal_HstSendEvent(SYS_EVENT, pData);
	hal_HstSendEvent(SYS_EVENT, 0x20160310);
	Pmd_FirstIrqTimer = COS_SetTimer(DEBOUNCE_TIME,Pmd_IrqTimerTimeOut ,key_Data, COS_TIMER_MODE_SINGLE);
	
#endif
#endif

    hal_IspiCsDeactivate(HAL_ISPI_CS_PMU);

    return;
}

BOOL pmd_TsdReadCoordinatesInternal(UINT16* gpadc_x, UINT16* gpadc_y)
{
#ifdef _USED_TSC_KEY_
    UINT16 x;

    x = hal_AnaGpadcGetTscX();

    if (x == HAL_ANA_GPADC_BAD_VALUE)
    {
        return FALSE;
    }

    *gpadc_x = x;

    return TRUE;
#else
    UINT16 x, y;

    x = hal_AnaGpadcGetTscX();
    y = hal_AnaGpadcGetTscY();

    if (x == HAL_ANA_GPADC_BAD_VALUE || y == HAL_ANA_GPADC_BAD_VALUE)
    {
        return FALSE;
    }

    *gpadc_x = x;
    *gpadc_y = y;

    return TRUE;
#endif	
}

PUBLIC BOOL pmd_SpiCsNonblockingActivate(BOOL singleWrite)
{
    // No need to activate CS for ISPI single data frame write-only operation
    if(singleWrite)
    {
        return TRUE;
    }

    BOOL status = FALSE;
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    if (1 == g_pmdActivated || 1 == g_tsdActivated)
    {
        status = FALSE;
        if (1 == g_pmdActivated)
        {
            PMD_TRACE(PMD_INFO_TRC, 0, "PMU has been activated. Be care of infinite loop.");
        }
    }
    else
    {
        status = hal_IspiCsActivate(HAL_ISPI_CS_PMU);
        if (status)
        {
            g_pmdActivated = 1;
        }
    }
    hal_SysExitCriticalSection(scStatus);

    return status;
}

PUBLIC BOOL pmd_RegWrite(RDA_REG_MAP_T regIdx, UINT32 data)
{
#if 0
    if (g_pmdInitDone)
    {
        if (!pmd_SpiCsNonblockingActivate(TRUE))
        {
            return FALSE;
        }
    }
#endif

    pmd_OpalSpiWrite(regIdx, (UINT16)data);

#if 0
    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(TRUE);
    }
#endif

    return TRUE;
}

PUBLIC BOOL pmd_RegRead(RDA_REG_MAP_T regIdx, UINT32 *pData)
{
    /* SPI shared by PMU, ABB, FM(to open close I2C), especially in tsd_Debounce()
     * in KEY interrupt context, it will interrupt the operation here when user
     * press key in quick succession. 
     */
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();
    
    if (g_pmdInitDone)
    {
        if (!pmd_SpiCsNonblockingActivate(FALSE))
        {
            hal_SysExitCriticalSection(scStatus);
            return FALSE;
        }
    }

    *pData = pmd_OpalSpiRead(regIdx);

    if (g_pmdInitDone)
    {
        pmd_OpalCsDeactivate(FALSE);
    }

    hal_SysExitCriticalSection(scStatus);

    return TRUE;
}


#ifdef SPI_REG_DEBUG
// ======================================================
// Debug: Read and Write Registers via SPI

#define PMD_SPI_REG_VALUE_LOCATION __attribute__((section(".pmu_reg_value")))

#define SPI_REG_WRITE_FLAG (1<<31)
#define SPI_REG_READ_FLAG (1<<30)
#define SPI_REG_FORCE_WRITE_FLAG (1<<29)
#define SPI_REG_FORCE_READ_FLAG (1<<28)
#define SPI_REG_DATA_MASK (0xffff)

#define PMU_REG_NUM (0x47)
UINT32 PMD_SPI_REG_VALUE_LOCATION g_pmdSpiRegValue[PMU_REG_NUM];

PROTECTED VOID pmd_SpiRegValueInit(VOID)
{
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        g_pmdSpiRegValue[i] = 0;
    }
}

PUBLIC VOID pmd_SpiRegCheck(VOID)
{
    static BOOL initDone = FALSE;
    if (!initDone)
    {
        pmd_SpiRegValueInit();
        initDone = TRUE;
    }
    
    for (int i=0; i<PMU_REG_NUM; i++)
    {
        if (g_pmdSpiRegValue[i] & SPI_REG_WRITE_FLAG)
        {
            if (!pmd_RegWrite(i, g_pmdSpiRegValue[i]&SPI_REG_DATA_MASK))
            {
                continue;
            }
            g_pmdSpiRegValue[i] &= ~SPI_REG_WRITE_FLAG;
        }
        if (g_pmdSpiRegValue[i] & SPI_REG_READ_FLAG)
        {
             UINT32 data;
             if (!pmd_RegRead(i, &data))
             {
                continue;
             }
            g_pmdSpiRegValue[i] &= ~(SPI_REG_READ_FLAG|SPI_REG_DATA_MASK);
            g_pmdSpiRegValue[i] |= data&SPI_REG_DATA_MASK;
        }
    }
}

PUBLIC VOID pmd_RegisterSpiRegCheckFunc(VOID)
{
    static BOOL registerIdleHook = FALSE;
    if (!registerIdleHook)
    {
        registerIdleHook = TRUE;
        extern bool sxs_RegisterDebugIdleHookFunc(void (*funcPtr)(void));
        sxs_RegisterDebugIdleHookFunc(&pmd_SpiRegCheck);
    }
}
#endif // SPI_REG_DEBUG

// =============================================================================
//  Earphone detection stuff
// =============================================================================

#ifdef ABB_HP_DETECT
PRIVATE VOID pmd_StartHpOutDetectInternal(VOID)
{
    UINT16 hpDetect;

	hal_HstSendEvent(SYS_EVENT, 0x14041005);

    if (g_pmdHpOutDetectStarted)
    {
        // HP detection already started
        return;
    }

    if (FALSE == pmd_SpiCsNonblockingActivate(FALSE))
    {
        sxr_StopFunctionTimer(pmd_StartHpOutDetectInternal);
        sxr_StartFunctionTimer(PMD_TIME_IRQ_ACT_CS_RETRY, pmd_StartHpOutDetectInternal, 0, 0);
        return;
    }

    // Read headphone IRQ status
    hpDetect = pmd_OpalSpiRead(RDA_ADDR_HP_DETECT_SETTING);

    pmd_OpalCsDeactivate(FALSE);

#if 0
    PMD_TRACE(PMD_INFO_TRC, 0,
        "[%08d] PMD-StartHpDetInter: hp(%d)=0x%04x",
        hal_TimGetUpTime()*1000/16384, g_pmdHpOutIrqEnabled, hpDetect);
#endif

    g_pmdHpOutDetectStarted = TRUE;

    if (hpDetect & RDA_PMU_HP_OUT)
    {
        pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_CLEAR);
        // PMU needs two 32K clock cycles to clear the cause,
        // whereas IRQ mask change will take effect immediately.
        hal_TimDelay(2);
    }

    if (hpDetect & RDA_PMU_HP_DETECT)
    {
        // Headphone is plugged-in. Enable headphone out IRQ.
        g_pmdHpOutIrqEnabled = TRUE;
        pmd_RegHpDetectSettingClr(RDA_PMU_HP_OUT_MASK);
    }
    else
    {
        // Headphone might be plugged-out. Check the status again later.

        // Never stop PMD IRQ function timers, as they have different jobs.
        sxr_StartFunctionTimer(PMD_TIME_IRQ_DEBOUNCE, pmd_IrqHanderInternal,
                               PMD_IRQ_CHECK_HP, 0);
    }
}

PUBLIC VOID pmd_StartHpOutDetect(BOOL start)
{
#if 0
    PMD_TRACE(PMD_INFO_TRC, 0, "[%08d] PMD-StartHpDet: %d",
        hal_TimGetUpTime()*1000/16384, start);
#endif

    // Invoked at task level
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (start)
    {
        // Respond to HP-OUT INT IRQ after HP state is stable
        sxr_StartFunctionTimer(PMD_TIME_HP_DETECT_STABLE, pmd_StartHpOutDetectInternal, NULL, 0);
    }
    else
    {
        sxr_StopFunctionTimer(pmd_StartHpOutDetectInternal);

        if (g_pmdHpOutDetectStarted)
        {
            pmd_RegHpDetectSettingSet(RDA_PMU_HP_OUT_MASK|RDA_PMU_HP_OUT_CLEAR);
            g_pmdHpOutIrqEnabled = FALSE;
            g_pmdHpOutDetectStarted = FALSE;
        }
    }
    hal_SysExitCriticalSection(scStatus);
}
#endif // ABB_HP_DETECT

#if 0 // move to edrv/gpio/gpio_detect.c
VOID pmd_EarDetectDelay(VOID)
{
    HAL_GPIO_CFG_T eraCfg;

    if (ear_handler != NULL)
    {
        if (hal_GpioGet(g_ear_gpio))
        {
#ifdef ABB_HP_DETECT
            pmd_StartHpOutDetect(FALSE);
#endif
            (*ear_handler)(PMD_EAR_STATUS_OUT);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear Delay gpio = 1");
        }
        else
        {
            (*ear_handler)(PMD_EAR_STATUS_IN);
            PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear Delay gpio = 0");
        }
    }

    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
}

VOID pmd_EarDetectHandler(VOID)
{
    PMD_TRACE(PMD_WARN_TRC, 0, "pmd:ear EarDetectHandler gpio");

    if (ear_handler != NULL)
    {
        (*ear_handler)(PMD_EAR_STATUS_UNKNOWN);
    }

    HAL_GPIO_CFG_T eraCfg;
    eraCfg.irqMask.rising = FALSE;
    eraCfg.irqMask.falling = FALSE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioIrqSetMask(g_ear_gpio, &eraCfg.irqMask);
    hal_GpioResetIrq(g_ear_gpio);
    sxr_StopFunctionTimer(pmd_EarDetectDelay);
    sxr_StartFunctionTimer(HAL_TICK1S, pmd_EarDetectDelay, NULL, 0);
}

VOID pmd_EarpDetectGpioInit(HAL_GPIO_GPIO_ID_T gpioId)
{
    if ( gpioId != HAL_GPIO_NONE &&
         (gpioId < HAL_GPIO_1 || gpioId > HAL_GPIO_7) )
    {
        PMD_ASSERT(FALSE, "Earpice detect GPIO should be none or from 1 to 7: 0x%08x", gpioId);
    }

    g_ear_gpio = gpioId;
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return;
    }

    HAL_GPIO_CFG_T eraCfg;
    eraCfg.direction = HAL_GPIO_DIRECTION_INPUT;
    eraCfg.irqHandler = pmd_EarDetectHandler;
    eraCfg.irqMask.rising = TRUE;
    eraCfg.irqMask.falling = TRUE;
    eraCfg.irqMask.debounce = TRUE;
    eraCfg.irqMask.level = FALSE;
    hal_GpioOpen(g_ear_gpio,&eraCfg);
}

PUBLIC VOID pmd_SetEarDetectHandler(PMD_EAR_HANDLER_T handler)
{
    ear_handler = handler;

    // Avoid reentrance of ear_handler coming from TIMER IRQ
    UINT32 scStatus = hal_SysEnterCriticalSection();
    if (ear_handler != NULL && pmd_GetEarModeStatus() == 1)
    {
        // Debouce the GPIO status
        pmd_EarDetectHandler();
    }
    hal_SysExitCriticalSection(scStatus);
}

UINT8 pmd_GetEarModeStatus(VOID)
{
    if (g_ear_gpio == HAL_GPIO_NONE)
    {
        return PMD_EAR_DETECT_POLLING;
    }
    else
    {
        return (hal_GpioGet(g_ear_gpio) == 0);
    }
}
#endif

PUBLIC UINT32 pmd_GetEarpieceGpadcChannel(VOID)
{
    return g_pmdConfig->earpieceGpadcChannel;
}

// =============================================================================
//  GPADC stuff
// =============================================================================

PUBLIC VOID pmd_GpadcSetInterval(UINT32 interval)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 =  PMU_SET_BITFIELD(g_pmdGpadc2, RDA_PMU_GPADC_START_TIME, interval);
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}

PUBLIC VOID pmd_GpadcEnableChannel(UINT32 channel)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 =  PMU_SET_BITFIELD(g_pmdGpadc2, RDA_PMU_GPADC_ENABLE_CHANNEL, channel) |
            RDA_PMU_GPADC_ENABLE | RDA_PMU_GPADC_ENABLE_DR;
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}
PUBLIC VOID pmd_GpadcDisable(VOID)
{
    UINT32 scStatus;
    scStatus = hal_SysEnterCriticalSection();

    g_pmdGpadc2 = g_pmdGpadc2 & (~RDA_PMU_GPADC_ENABLE);
    pmd_RDAWrite(RDA_ADDR_GPADC2, g_pmdGpadc2);

    hal_SysExitCriticalSection(scStatus);
    return;
}

PUBLIC BOOL pmd_GpadcChannelEnabled(UINT32 channel)
{
    UINT16 reg;
#ifdef GPADC_CONFIG_READ_FROM_PMU
    // CAUTION: Cannot be invoked at interrupt level
    reg = pmd_RDARead(RDA_ADDR_GPADC2);
#else
    reg = g_pmdGpadc2;
#endif

    if( (reg&RDA_PMU_GPADC_ENABLE_CHANNEL_MASK) == RDA_PMU_GPADC_ENABLE_CHANNEL(channel) &&
        (reg&RDA_PMU_GPADC_ENABLE) )
    {
        return TRUE;
    }

    return FALSE;
}

