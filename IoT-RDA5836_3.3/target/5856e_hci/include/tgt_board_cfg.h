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
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file tgt_board_config.h
/// That file describes the configuration of the board drivers for the specific
/// gallite g33 target.
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _TGT_BOARD_CFG_H_
#define _TGT_BOARD_CFG_H_

#include "hfp.h"

// #############################################################################
// #                                                                           #
// #                      CHIP AND MANDATORY DRIVERS                           #
// #                                                                           #
// #############################################################################

extern VOID gpio_CardDetectHandler(VOID);


// =============================================================================
// TGT_HAL_CONFIG
// =============================================================================
#ifndef TGT_HAL_CONFIG



#define   CHIP_5856E_16PIN    0
#define   CHIP_5856E_24PIN_HEADSET  1
#define   CHIP_5856E_40PIN  2

#define  CHIP_PIN_NUMBER    CHIP_5856E_24PIN_HEADSET
// -----------------------------------------------------------------------------

// =============================================================================

// =============================================================================
// Chip pin num
// =============================================================================
#if(CHIP_PIN_NUMBER == CHIP_5856E_16PIN)
#define TGT_HAL_PINNUM    5     //pin16 version 
#elif(CHIP_PIN_NUMBER == CHIP_5856E_24PIN_HEADSET)
#define TGT_HAL_PINNUM    10     //pin24 version 
#elif(CHIP_PIN_NUMBER == CHIP_5856E_40PIN)
#define TGT_HAL_PINNUM    14     //pin40 version 
#endif 
// =============================================================================
// Chip version
// =============================================================================
#define TGT_HAL_CHIP_VERSION 1


// =============================================================================
// RF CLK FREQUENCY 
// =============================================================================
#define TGT_HAL_RF_CLK_FREQ     HAL_SYS_FREQ_26M

// =============================================================================
// TGT_GPIO_CONFIG_CFG 
// -----------------------------------------------------------------------------
// This fills the structure GPIO_CONFIG_CFG_T
// =============================================================================
#define TGT_GPIO_CONFIG_CFG                                         \
{                                                                   \
    .gpio_irc                     = HAL_GPIO_NONE,                   \
    .gpio_NFC                   = HAL_GPIO_NONE,                    \
    .led_count                   = 4,                               \
    .ledHighLight               = TRUE,                            \
    .leds      = {                                                  \
        {.gpioId =HAL_GPIO_1_1},     /* MUSIC */                             \
        {.gpioId =HAL_GPIO_1_2},     /* FM */                                \
        {.gpioId =HAL_GPIO_1_3},     /* Bluetooth */                         \
        {.gpioId =HAL_GPIO_1_0}},                                           \
    .key_count                   = 0,                               \
    .key_interrupt_mode     = TRUE,                            \
    .high_keydown             = FALSE,                           \
    .gpio_keys                   = { HAL_GPIO_NONE, HAL_GPIO_NONE, HAL_GPIO_NONE, HAL_GPIO_NONE, HAL_GPIO_NONE, HAL_GPIO_NONE},         \
    .gpio_keyirqnum        = { 15, 13, 12, 11, 8, 6},         \
    .gpio_lineIn                  = HAL_GPIO_NONE,                   \
    .lineinPlugHigh              = FALSE,                           \
    .cardDetectGpio            = HAL_GPIO_0_5,                    \
    .gpioCardDetectHigh      = FALSE,                           \
    .earpieceDetectGpio      = HAL_GPIO_NONE, 			        \
    .earpiecePlugHigh          = FALSE,                           \
    .earpiecePlugin             = FALSE,                           \
    .gpioMute                     = { .gpioId = HAL_GPIO_NONE },     \
    .gpioMuteEnable            = FALSE,                           \
    .gpioMuteHigh               = FALSE,                           \
    .uart2_wake                 = HAL_GPIO_NONE, 	                \
    .gpioUSBDetect             = HAL_GPIO_NONE, 			        \
    .gpioUSBPlugHigh          = FALSE,                           \
    .gpioUSBDetectEnable    = FALSE,                           \
}

#if(CHIP_PIN_NUMBER == CHIP_5856E_16PIN)
#define TGT_GPIO_PINMUX_CFG   \
{                                                              \
     /*    id             in or out                   cfg0-cfg9         up or down      drv strength enint  gpio intnum     irqmask           irqhandler  */  \
    { HAL_GPIO_0_0,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  0, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dn */  \
    { HAL_GPIO_0_1,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  1, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dp */  \
    { HAL_GPIO_0_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   2, {FALSE, TRUE, FALSE, FALSE}, NULL},/* keysense */   \
    { HAL_GPIO_1_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP, NONEED, FALSE,  10, {FALSE, FALSE, FALSE, FALSE}, NULL},/* i2s di */\
    { HAL_GPIO_1_3,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP, NONEED, FALSE,  11, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2s do */\
}
#elif(CHIP_PIN_NUMBER == CHIP_5856E_24PIN_HEADSET)    // pin24 version
#define TGT_GPIO_PINMUX_CFG   \
{                                                              \
     /*    id             in or out                   cfg0-cfg9         up or down      drv strength enint  gpio intnum     irqmask           irqhandler  */  \
    { HAL_GPIO_0_0,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  0, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dn */  \
    { HAL_GPIO_0_1,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  1, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dp */  \
    { HAL_GPIO_0_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   2, {FALSE, TRUE, FALSE, FALSE}, NULL},/* keysense */   \
    { HAL_GPIO_0_4,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_4, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   4,{FALSE,  FALSE,  FALSE,  FALSE}, NULL},/* D5 */ \
    { HAL_GPIO_0_5,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_11, HAL_GPIO_UP, NONEED, TRUE,  5, {TRUE, TRUE, TRUE, FALSE}, gpio_CardDetectHandler}, /* D6*/\
    { HAL_GPIO_0_6,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_4, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  6, {FALSE, FALSE, FALSE, FALSE}, NULL},/* D7 */ \
    { HAL_GPIO_1_0,  HAL_GPIO_DIRECTION_OUTPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  8, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2c sda */\
    { HAL_GPIO_1_1,  HAL_GPIO_DIRECTION_OUTPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  9, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2c clk*/\
    { HAL_GPIO_1_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP, NONEED, FALSE,  10, {FALSE, FALSE, FALSE, FALSE}, NULL},/* i2s di */\
    { HAL_GPIO_1_3,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP, NONEED, FALSE,  11, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2s do */\
}
#elif(CHIP_PIN_NUMBER == CHIP_5856E_40PIN)
#define TGT_GPIO_PINMUX_CFG   \
{                                                              \
     /*    id             in or out                   cfg0-cfg9         up or down      drv strength enint  gpio intnum     irqmask           irqhandler  */  \
    { HAL_GPIO_0_0,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_8, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  0, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dn */  \
    { HAL_GPIO_0_1,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  1, {FALSE, FALSE, FALSE, FALSE}, NULL},/* usb_dp */  \
    { HAL_GPIO_0_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_3, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   2, {FALSE, TRUE, FALSE, FALSE}, NULL},/* keysense */   \
    { HAL_GPIO_0_3,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_8, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   3,{FALSE, FALSE, FALSE, FALSE}, NULL},/* D3 */ \
    { HAL_GPIO_0_4,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_8, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,   4,          {TRUE,  TRUE,  TRUE,  FALSE}, NULL},/* D5 */ \
    { HAL_GPIO_0_5,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_2, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  5, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* D6*/\
    { HAL_GPIO_0_6,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_2, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  6,          {FALSE, FALSE, FALSE, FALSE}, NULL},/* D7 */ \
    { HAL_GPIO_0_7,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_4, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  7, {FALSE, FALSE, FALSE, FALSE}, NULL}, \
    { HAL_GPIO_1_0,  HAL_GPIO_DIRECTION_OUTPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  8, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2c sda */\
    { HAL_GPIO_1_1,  HAL_GPIO_DIRECTION_OUTPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  9, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2c clk*/\
    { HAL_GPIO_1_2,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  10, {FALSE, FALSE, FALSE, FALSE}, NULL},/* i2s di */\
    { HAL_GPIO_1_3,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_0, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  11, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2s do */\
    { HAL_GPIO_1_4,  HAL_GPIO_DIRECTION_INPUT, HAL_GPIO_ALTFUNC_CFG_11, HAL_GPIO_UP, NONEED, TRUE,  12,   {TRUE,  TRUE,  TRUE,  FALSE}, gpio_CardDetectHandler}, /* i2s lrck */\
    { HAL_GPIO_1_5,  HAL_GPIO_DIRECTION_OUTPUT, HAL_GPIO_ALTFUNC_CFG_4, HAL_GPIO_UP_DOWN_QTY, NONEED, FALSE,  13, {FALSE, FALSE, FALSE, FALSE}, NULL}, /* i2s bclk */\
}
#endif


// =============================================================================
// TGT_HAL_CAM_CFG
// -----------------------------------------------------------------------------
// This fills the structure HAL_CFG_CAM_T
// =============================================================================
#define TGT_HAL_CAM_CFG                                                 \
{                                                                       \
    .camUsed            = FALSE,                                        \
    .camMpUsed          = FALSE,                                        \
    .camRstActiveH      = FALSE,                                        \
    .camPdnActiveH      = TRUE,                                         \
    .camPdnRemap        = { .gpioId  = HAL_GPIO_NONE},                  \
    .camRstRemap        = { .gpioId  = HAL_GPIO_NONE},                  \
    .cam1Used           = FALSE,                                        \
    .cam1RstActiveH     = FALSE,                                        \
    .cam1PdnActiveH     = TRUE,                                         \
    .cam1PdnRemap       = { .gpioId  = HAL_GPIO_NONE},                  \
    .cam1RstRemap       = { .gpioId  = HAL_GPIO_NONE},                  \
    .camSerialIfMode    = FALSE,                                        \
    .camGpio1En         = FALSE,                                        \
    .camGpio2En         = FALSE,                                        \
    .camGpio3En         = FALSE,                                        \
}

// =============================================================================
// TGT_HAL_PWM_CFG
// -----------------------------------------------------------------------------
/// This structure describes the PWM configuration for a given target. 
/// The first field identify which PWL is used for GLOW (if any).
/// The lasts fields tell wether the pin corresponding to PWM output 
/// is actually used as PWM output and not as something else (for 
/// instance as a GPIO). 
// =============================================================================
#define TGT_HAL_PWM_CFG                                                 \
{                                                                       \
    .pwlGlowPosition    = HAL_PWL_NONE,                                 \
    .pwl0Used           = TRUE,                                         \
    .pwl1Used           = FALSE,                                        \
    .pwtUsed            = FALSE,                                        \
    .lpgUsed            = FALSE                                         \
}

// =============================================================================
// HAL_CFG_I2C_T
// -----------------------------------------------------------------------------
/// This structure describes the I2C configuration for a given target. The
/// fields tell wether the corresponding I2C pins are actually used
/// for I2C and not as something else (for instance as a GPIO). 
// =============================================================================
#define TGT_HAL_I2C_CFG                                                 \
{                                                                       \
    .i2cUsed             = TRUE,                                       \
    .i2cBps              = HAL_I2C_BPS_100K,                            \
    .i2c2Used            = TRUE,                                        \
    .i2c2Bps             = HAL_I2C_BPS_100K,                            \
    .i2c3Used            = TRUE,                                       \
    .i2c3Bps             = HAL_I2C_BPS_100K,                            \
}

// =============================================================================
// TGT_HAL_I2S_CFG
// -----------------------------------------------------------------------------
/// This structure describes the I2S configuration for a given target. The
/// fields tell wether the corresponding I2S pin is actually used
/// for I2S and not as something else (for instance as a GPIO). 
// =============================================================================
#define TGT_HAL_I2S_CFG                                                 \
{                                                                       \
    .i2sFunc            = TRUE,                                         \
    .doUsed             = TRUE,                                         \
    .di0Used            = TRUE,                                         \
    .di1Used            = TRUE,                                         \
    .di2Used            = FALSE,                                        \
}

// =============================================================================
// TGT_HAL_UART_CFG
// -----------------------------------------------------------------------------
/// Used to describes a configuration for used pin by an UART for a given target.
// =============================================================================
#ifndef UART2_SUPPORT
#define TGT_HAL_UART_CFG                                                \
{                                                                       \
    HAL_UART_CONFIG_NONE												\
}
#else
#define TGT_HAL_UART_CFG                                                \
{                                                                       \
    HAL_UART_CONFIG_DATA                                                \
}
#endif

// =============================================================================
// TGT_HAL_HST_UART_CFG
// -----------------------------------------------------------------------------
/// Used to describe a configuration for used pin by Hst UART for a given target.
// =============================================================================
#define TGT_HAL_HST_UART_CFG    HAL_UART_CONFIG_NONE

// =============================================================================
// TGT_HAL_SPI_CFG
// -----------------------------------------------------------------------------
/// This structure describes the SPI configuration for a given target. The first
/// fields tell wether the pin corresponding to chip select is actually used
/// as a chip select and not as something else (for instance as a GPIO). 
/// Then, the polarity of the Chip Select is given. It is only relevant
/// if the corresponding Chip Select is used as a Chip Select.
/// Finally which pin is used as input, Can be none, one or the other.
/// On most chip configuration the input 0 (di0) is on the output pin: SPI_DIO
// =============================================================================
#define TGT_HAL_SPI_CFG                                                 \
    {                                                                   \
        {                                                               \
        .cs0Used        = FALSE,                                        \
        .cs1Used        = FALSE,                                        \
        .cs2Used        = FALSE,                                        \
        .cs3Used        = FALSE,                                        \
        .cs0ActiveLow   = FALSE,                                        \
        .cs1ActiveLow   = FALSE,                                        \
        .cs2ActiveLow   = FALSE,                                        \
        .cs3ActiveLow   = FALSE,                                        \
        .di0Used        = FALSE,                                        \
        .di1Used        = FALSE,                                        \
        .usePinConfig0  = FALSE,                                        \
        .usePinConfig1  = FALSE                                         \
    },                                                                  \
    {                                                                   \
        .cs0Used        = FALSE,                                        \
        .cs1Used        = FALSE,                                        \
        .cs2Used        = FALSE,                                        \
        .cs3Used        = FALSE,                                        \
        .cs0ActiveLow   = FALSE,                                        \
        .cs1ActiveLow   = FALSE,                                        \
        .cs2ActiveLow   = FALSE,                                        \
        .cs3ActiveLow   = FALSE,                                        \
        .di0Used        = FALSE,                                        \
        .di1Used        = FALSE,                                        \
        .usePinConfig0  = FALSE,                                        \
        .usePinConfig1  = FALSE                                         \
        }                                                               \
    }

// =============================================================================
// TGT_HAL_GOUDA_CFG
// -----------------------------------------------------------------------------
/// This structure describes the GOUDA configuration for a given target. 
/// The first fields tell wether the pin corresponding to chip select is 
/// actually used as a chip select and not as something else (for instance
/// as a GPIO). If none are used, the GOUDA is considered unused.
// =============================================================================
#define TGT_HAL_GOUDA_CFG                                               \
{                                                                       \
    .cs0Used            = TRUE,                                         \
    .cs1Used            = FALSE,                                        \
    .lcdData16Bit       = FALSE,                                        \
    .lcdResetActiveLow  = TRUE,                                         \
  /*  .lcdResetPin        = { .gpoId = HAL_GPO_7 },  */                     \
}

// =============================================================================
// TGT_HAL_IO_DRIVE
// -----------------------------------------------------------------------------
/// This structure describes the IO Drive configuration for a given target. 
// =============================================================================
#define TGT_HAL_IO_DRIVE                                                \
{                                                                       \
    {                                                                   \
        .vMem0Domain    = 0,                                            \
        .vMem1Domain    = 0,                                            \
        .vMemSpiDomain  = 0,                                            \
        .vLcdDomain     = 3,                                            \
        .vCamDomain     = 3,                                            \
        .vPadDomain     = 3,                                            \
        .keyInKeyOut    = 3,                                            \
        .vSdmmcDomain   = 0,                                            \
    }                                                                   \
}


// =============================================================================
// TGT_HAL_CONFIG
// =============================================================================
#define TGT_HAL_CONFIG                                                  \
{                                                                       \
    .chipVersion        = TGT_HAL_CHIP_VERSION,                         \
    .rfClkFreq          = TGT_HAL_RF_CLK_FREQ,                          \
    .useLpsCo1          = FALSE,                                        \
    .keyInMask          = 0x00,                                         \
    .keyOutMask         = 0x00,                                         \
    .ledUsedMask        = 0x03,                                         \
    .gpadcUsedMask      = 0x0,                                          \
    .uart2PinConfig     = 1,                                            \
    .lpg_pwl_pwt_pwm_mask = 0x0,                                        \
    .pwmCfg             = TGT_HAL_PWM_CFG,                              \
    .useUsbBackup       = FALSE,                                        \
    .useClk32k          = TRUE,                                         \
    .i2cCfg             = TGT_HAL_I2C_CFG,                              \
    .i2c1UseCamPins     = FALSE,                                        \
    .usePmuAbbSpi       = FALSE,                                        \
    .i2sCfg             = TGT_HAL_I2S_CFG,                              \
    .uartCfg            = TGT_HAL_UART_CFG,                             \
    .hstUartCfg         = TGT_HAL_HST_UART_CFG,                         \
    .spiCfg             = TGT_HAL_SPI_CFG,                              \
    .useIRC             = TRUE,   /* New added in 5855 */               \
    .useSdmmc           = TRUE,                                         \
    .useSdmmc2          = FALSE,                                        \
    .uartKeyinoutSel    = {                                             \
         .uart1UseTco = 0,                                              \
         .uart2UseTco = 0,                                              \
         .keyin0GpioEn = 0,  /* GPIO_20 */                              \
         .keyin4GpioEn = 1,  /* GPIO_21 */                              \
         .keyout1GpioEn = 0, /* GPIO_17 */                              \
         .keyout2GpioEn = 0, /* GPIO_11 */                              \
         .keyout3GpioEn = 1, /* GPIO_7 */                               \
         .keyout4GpioEn = 0, /* GPIO_8 */                               \
     },                                                                 \
    .spi2UseCamPins     = FALSE,                                        \
    .camCfg             = TGT_HAL_CAM_CFG,                              \
    .goudaCfg           = TGT_HAL_GOUDA_CFG,                            \
    .useSpiLcd          = FALSE,                                        \
    .noConnectGpio      = {0},                                          \
    .usedGpio           = {0,0},                                        \
    .usedTco            = 0,                                            \
 /*   .ioDrive            = TGT_HAL_IO_DRIVE                   */       \
}

#endif // TGT_HAL_CONFIG

#ifndef SX_NB_CFG

#define SX_NB_CFG                                                           \
{                                                                           \
    .sxr_NB_MAX_TASK                                = 6,                    \
    .sxr_STACK_SIZE_MULTIPLIER                      = 1,                    \
    .sxr_TK_STCK_HEAP                               = SXR_CSP_TK_STCK_HEAP, \
    .sxr_JB_STCK_HEAP                               = SXR_CSP_JB_STCK_HEAP, \
    .sxr_JB_CTX_HEAP                                = SXR_CSP_JB_CTX_HEAP,  \
    .sxr_NB_MAX_MBX_ENV                             = 150,                  \
    .sxr_EVT_MBX_SIZE                               = 2,                    \
    .sxr_NB_MAX_MBX                                 = 20,                   \
    .sxr_NB_MAX_SEM                                 = 6,                    \
    .sxr_NB_MAX_MUTEX                               = 1,                    \
    .sxr_TIMER_NB_INSTANCE                          = 1,                    \
    .sxr_NB_MAX_TIMER_ENVELOPPE                     = 30,                   \
    .sxr_TIMER_CTX_SIZE                             = 2,                    \
    .sxr_TIMER_LIST_SIZE                            = 8,                    \
    .sxr_REGULAR_TIMER                              = 0,/*PAL_REGULAR_TIMER,*/    \
    .sxr_FRAMED_TIMER                               = 1,/*PAL_FRAMED_TIMER,*/     \
    .sxr_NB_MAX_POOL                                = 7,                    \
    .sxr_NB_HEAP_USER                               = 5,                    \
    .sxr_NB_MAX_QUEUE                               = 120,                  \
    .sxr_NB_MAX_USR_QUEUE                           = 2,                    \
    .sxr_NB_QUEUE_ENVELOPPE                         = 300,                  \
    .sxr_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_START     = 100,                  \
    .sxr_NB_FREE_QUEUE_ENVELOPPE_OVERLOAD_END       = 250,                  \
    .sxr_NB_MAX_TRACEID                             = 30,                   \
    .sxs_FLH_NB_FILES_MAX                           = 16,                   \
    .sxs_SPY_NB_MAX                                 = 8,                    \
}

#endif// SX_NB_CFG


// =============================================================================
// KEY Mapping
// =============================================================================
#ifndef KEY_MAP
#define AUTO_POWERON  TRUE

#ifdef _USED_TSC_KEY_
#define KEY_MAP                                                           \
{                                                                         \
 /* power key */    KEY_POWER,                                            \
 /* gpio keys */    KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
 /* TSC keys  */    KEY_MODE,       KEY_VOL_UP,	    KEY_REC,	    KEY_PREV,	    KEY_PLAY,	    KEY_NEXT,	    KEY_EQ,	        KEY_VOL_DOWN, \
                    KEY_MUTE,		KEY_PLAY,		KEY_NEXT,		KEY_PREV,	    KEY_MODE,   	KEY_REC,    	KEY_INVALID,    KEY_EQ,\
                    KEY_VOL_UP, 	KEY_VOL_DOWN,	KEY_INVALID,    KEY_INVALID,	KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
}
#else
#define KEY_MAP                                                           \
{                                                                         \
 /* power key */    KEY_POWER,                                            \
 /* gpio keys */    KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
 /* row 0  */       KEY_MODE,		KEY_VOL_UP, 	KEY_REC,		KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
 /* row 1  */       KEY_PREV,		KEY_PLAY,		KEY_NEXT,		KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
 /* row 2  */       KEY_EQ,         KEY_VOL_DOWN,	KEY_LOOP,		KEY_INVALID,	KEY_INVALID,    KEY_INVALID,	KEY_INVALID,	KEY_INVALID,  \
}
#endif // KEY_MAP
#endif // _USED_TSC_KEY_

#ifndef KEY_CONFIG

#define KEY_CONFIG                                                        \
{                                                                         \
    .repeat_timeout      = 1000,                                          \
    .repeat_interval     = 200,                                           \
    .keymap              = g_tgtKeyMap                                    \
}

#endif // KEY_CONFIG


#ifndef KEY_BOOT_DOWNLOAD

#define KEY_BOOT_DOWNLOAD { KP_0, KP_OK, }

#endif // KEY_BOOT_DOWNLOAD


// =============================================================================
// IRC Mapping
// =============================================================================
#ifndef IRC_MAP

#define IRC_CODE_MAP_SIZE		    22
#define IRC_MAX_STYLE               4


#define IRC_STYLE_MAP                                                     \
{                                                                         \
    0xbf00, 0x7f80, 0xff00, 0xfd02                                        \
}

#define IRC_MAP                                                       \
{                                                                     \
    {0x10, 0x0a, 0x0c, 0x08}, /* KEY_1   */                           \
    {0x11, 0x1b, 0x18, 0x88}, /* KEY_2   */                           \
    {0x12, 0x1f, 0x5e, 0x48}, /* KEY_3   */                           \
    {0x14, 0x0c, 0x08, 0x28}, /* KEY_4   */                           \
    {0x15, 0x0d, 0x1c, 0xa8}, /* KEY_5   */                           \
    {0x16, 0x0e, 0x5a, 0x68}, /* KEY_6   */                           \
    {0x18, 0x00, 0x42, 0x18}, /* KEY_7   */                           \
    {0x19, 0x0f, 0x52, 0x98}, /* KEY_8   */                           \
    {0x1a, 0x19, 0x4a, 0x58}, /* KEY_9   */                           \
    {0x0c, 0x07, 0x16, 0x30}, /* KEY_0   */                           \
    {0x01, 0x1a, 0x46, 0x60}, /* KEY_MODE   */                        \
    {0x02, 0x01, 0x44, 0x40}, /* KEY_PLAY   */                        \
    {0xff, 0xff, 0xff, 0xff}, /* KEY_REC   */                         \
    {0x0d, 0x02, 0x40, 0xb0}, /* KEY_PREV   */                        \
    {0x0e, 0x03, 0x43, 0x70}, /* KEY_NEXT   */                        \
    {0x08, 0x04, 0x07, 0x10}, /* KEY_EQ   */                          \
    {0x04, 0x08, 0x19, 0x20}, /* KEY_LOOP   */                        \
    {0xff, 0xff, 0xff, 0xff}, /* KEY_MUTE   */                        \
    {0xff, 0xff, 0xff, 0xff}, /* KEY_HALT   */                        \
    {0x00, 0x12, 0x45, 0x00}, /* KEY_POWER   */                       \
    {0x0a, 0x06, 0x09, 0x50}, /* KEY_VOL_UP   */                      \
    {0x09, 0x05, 0x15, 0x90}, /* KEY_VOL_DOWN   */                    \
}

#define IRC_CONFIG                                                    \
{                                                                     \
    .irc_styles          = IRC_MAX_STYLE,                             \
    .map_size            = IRC_CODE_MAP_SIZE,                         \
    .timeout             = 200,                                       \
    .style_list          = &g_tgtIRCStyle[0],                         \
    .code_map            = &g_tgtIRCMap[0][0]                         \
}

#endif // IRC_MAP

// =============================================================================
// RFD config
// =============================================================================
#ifndef TGT_RFD_CONFIG
#define TGT_RFD_CONFIG

#include "hal_tcu.h"
#define TGT_XCV_CONFIG    {.RST = TCO_UNUSED, .PDN  = TCO(11)              }
#define TGT_PA_CONFIG     {.ENA = TCO_UNUSED, .TXEN = TCO_UNUSED, .BS  = TCO_UNUSED }
#define TGT_SW_CONFIG     {.SW1 = TCO(2), .SW2  = TCO(3), .SW3 = TCO(4) }

// Note: Some XCV maybe have different control pin names, so someone who develop
//       the target configuration should explain the pin usage as below.
//       
// FIXME Fix that with proper knowledge !
// PA->ENA is VLOGIC pin for SKY77518, MODEN for TQM4002, MOD for RDA6216
// PA-TXEN is BIAS for RDA6216 ?
//
#endif // TGT_RFD_CONFIG


// =============================================================================
// PMD config
// -----------------------------------------------------------------------------
/// This fills the structure PMD_CONFIG_T
// =============================================================================
#ifndef TGT_PMD_CONFIG

#define TGT_PMD_CONFIG                                                  \
    {                                                                   \
        .power =                                                        \
        {                                                               \
            { /* PMD_POWER_MIC          : Micro bias enable */          \
                .ldo            = {  .opal = PMD_LDO_MIC},              \
                .polarity       = TRUE,                                 \
                .shared         = TRUE, /* with PMD_POWER_EARPIECE */   \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_CAMERA       : Camera LDO enable */          \
                .ldo            = { .opal = PMD_LDO_IO1P8|PMD_LDO_ASW},    \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_AUDIO        : Audio LDO enable */           \
                .ldo            = { .opal = PMD_LDO_ABB},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_STEREO_DAC   : Stereo DAC LDO enable */      \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_LOUD_SPEAKER : Loud Speaker enable */        \
                .ldo            = /* { .opal = PMD_LDO_NONE},     */     \
                              { .pin = { .gpioId = HAL_GPIO_NONE } },     \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_PA           : RF Power Amplifier */         \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_USB          : USB LDOs enable */            \
                .ldo            = { .opal = PMD_LDO_USB},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_SDMMC        : SD/MMC LDO enable */          \
                .ldo            = { .opal = PMD_LDO_MMC},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_FM           : FM LDO enable */              \
                .ldo            = { .opal = PMD_LDO_ABB},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_EARPIECE     : Ear Piece Micro bias enable */\
                .ldo            = { .opal = PMD_LDO_MIC},               \
                .polarity       = TRUE,                                 \
                .shared         = TRUE, /* with PMD_POWER_MIC */        \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_BT           : BlueTooth LDOs enable */      \
                .ldo            = { .pin ={ .gpoId= HAL_GPIO_NONE}},         \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_CAMERA_FLASH : Camera Flash Light enable */  \
                .ldo            = { .pin = {.gpoId = HAL_GPIO_NONE}},       \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_LCD          : (main) LCD LDO enable */      \
                .ldo            = { .opal = PMD_LDO_LCD},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_I2C          : I2C LDO enable */             \
                 /* Inside Gallite chip, PMD_LDO_CAM also supplies  */  \
                 /*   power to I2C1 I/O. It must be specified here  */  \
                 /*   if I2C1 is used on this board.                */  \
                 /* On this board, PMD_LDO_LCD also supplies power  */  \
                 /*   to I2C2 pull-up resistor.                     */  \
                 /* In 5855, PMD_LDO_CAM is replaced by PMD_LDO_IO1P8*/ \
                .ldo            = { .opal = PMD_LDO_LCD|PMD_LDO_IO1P8},               \
                .polarity       = TRUE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },                                                          \
            { /* PMD_POWER_BT2           : BlueTooth LDOs enable */      \
                .ldo            = { .pin ={ .gpoId= HAL_GPIO_NONE}},         \
                .polarity       = FALSE,                                 \
                .shared         = FALSE,                                \
                .powerOnState   = FALSE,                                \
            },										\
        },                                                              \
        .level =                                                        \
        {                                                               \
            { /* PMD_LEVEL_KEYPAD       : KeyPad Back Light level */    \
                .type           = PMD_LEVEL_TYPE_OPAL,                  \
                .ldo            = { .pin = { .gpoId  = HAL_GPIO_NONE}},  \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LCD          : (main) LCD Back Light level*/ \
                .type           = PMD_LEVEL_TYPE_BACKLIGHT,             \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_SUB_LCD      : Sub LCD Back Light level */   \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED0         : LED0 Light level */           \
                .type           = PMD_LEVEL_TYPE_LDO,                 \
                .ldo            = { .pin = {.gpoId = HAL_GPIO_NONE}},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED1         : LED1 Light level */           \
                .type           = PMD_LEVEL_TYPE_LDO,                 \
                .ldo            = { .pin = {.gpoId = HAL_GPIO_NONE}},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED2         : LED2 Light level */           \
                .type           = PMD_LEVEL_TYPE_LDO,                 \
                .ldo            = { .pin = {.gpoId = HAL_GPIO_NONE}},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LED3         : LED3 Light level */           \
                .type           = PMD_LEVEL_TYPE_LDO,                   \
                .ldo            = { .pin = {.gpoId = HAL_GPIO_NONE}},    \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_VIBRATOR     : Vibrator control level */     \
                .type           = PMD_LEVEL_TYPE_LDO,                   \
                .ldo            = { .opal = PMD_LDO_VIBR},              \
                .powerOnState   = 0,                                    \
            },                                                          \
            { /* PMD_LEVEL_LOUD_SPEAKER : loudspeaker gain */           \
                .type           = PMD_LEVEL_TYPE_NONE,                  \
                .ldo            = { .opal = PMD_LDO_NONE},              \
                .powerOnState   = 0,                                    \
            },                                                          \
        },                                                              \
        .lightLevelToBacklight = {   0,  8,  12,  16,  32,  40,  48, 64},  \
        .lightLevelToPwm0 = {   0,  32,  32,  32,  32,  32,  32, 32},   \
        .lightLevelToPwm1 = {   0,   0,   0,   0,   0, 224, 224, 224},  \
        /* Inside Gallite chip, PMD_LDO_MMC also supplies power to  */  \
        /*   TCU0-2, so if any of TCU0-2 is used, MMC LDO should be */  \
        /*   always on in active mode and cannot be controlled as a */  \
        /*   POWER or LEVEL LDO.                                    */  \
        .ldoEnableNormal = 0,                                 \
        .ldoEnableLowPower = 0,                                         \
        .ldoIoIs2_8 = TRUE,                                            \
        .ldoLcdIs2_8 = TRUE,                                            \
        .ldoMMCIs2_8 = TRUE,                                            \
        .ldoIbrIs2_8 = FALSE,                                           \
        .ldoRfIs2_8  = FALSE,                                           \
        .batteryGpadcChannel = 7 /*HAL_ANA_GPADC_CHAN_7*/,                    \
        /*.batteryLevelChargeTermMV    = 4200,  */                          \
        /*.batteryLevelRechargeMV      = 4150,  */                          \
        .batteryLevelFullMV          = 4200,                            \
        .batteryLevelPrechargeMV     = 3200,                            \
        .batteryChargeTimeout        = 4 HOURS,                         \
        .batteryPulseChargeEndRatio  = 4, /* 400mA / 50mA */            \
        .batteryOffsetHighActivityMV = 30,                              \
        .powerOnVoltageMV            = 3200,                            \
        .powerDownVoltageMV          = 3200,                            \
        .batteryChargeCurrent        = PMD_CHARGER_700MA,               \
        .chargerGpadcChannel         = 6/*HAL_ANA_GPADC_CHAN_7*/,            \
        .chargerLevelUpperLimit      = 6000,                            \
        /*.chargerOffsetBackToNormal   = -200,  */                          \
        .batteryMVScreenAntiFlicker  = 3600,                            \
        /*.batteryOffsetScreenNormal   = 100,   */                          \
        .earpieceGpadcChannel        = HAL_ANA_GPADC_CHAN_1,            \
        .holdPowerOn                 = FALSE,                           \
        .keyPowerOnInterval          = PMD_KEY_POWERON_INTERVAL2       \
    }

#endif // TGT_PMD_CONFIG



// =============================================================================
// MEMD config
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// note note note note note note note note note note note note note note note
// note note note note note note note note note note note note note note note
// mcp flash :not modify, still use memd/flsh_xxxx/include/memd_tgt_params_gallite.h 
// spi flash :delete memd/flsh_xxxx/include/memd_tgt_params_gallite.h 
// add memd/include/memd_tgt_params_8806.h for 8806
// add memd/include/memd_tgt_params_8808.h for 8808
// add memd/include/memd_tgt_params_8809.h for 8809
// note note note note note note note note note note note note note note note
// note note note note note note note note note note note note note note note
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// =============================================================================
#include "memd_tgt_params_8809.h"



// =============================================================================
// TGT_AUD_CONFIG
// -----------------------------------------------------------------------------
/// Audio interface configuration
// =============================================================================
#ifndef TGT_AUD_CONFIG
#define TGT_AUD_CONFIG

#define TGT_AUD_CONFIG_RECEIVER_DRIVER             CodecGallite
#define TGT_AUD_CONFIG_RECEIVER_PARAM              0
#define TGT_AUD_CONFIG_RECEIVER_OUTPUT_PATH        AUD_SPK_RECEIVER
#define TGT_AUD_CONFIG_RECEIVER_OUTPUT_TYPE        AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_RECEIVER_INPUT_PATH         AUD_MIC_RECEIVER
#define TGT_AUD_CONFIG_RECEIVER_MIXAUDIO           TRUE

#define TGT_AUD_CONFIG_EAR_PIECE_DRIVER            CodecGallite
#define TGT_AUD_CONFIG_EAR_PIECE_PARAM             0
#define TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_PATH       AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_EAR_PIECE_OUTPUT_TYPE       AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_EAR_PIECE_INPUT_PATH        AUD_MIC_EAR_PIECE
#define TGT_AUD_CONFIG_EAR_PIECE_MIXAUDIO          FALSE

#define TGT_AUD_CONFIG_LOUD_SPEAKER_DRIVER         CodecGallite
#define TGT_AUD_CONFIG_LOUD_SPEAKER_PARAM          0
#define TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_PATH    AUD_SPK_LOUD_SPEAKER //AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_LOUD_SPEAKER_OUTPUT_TYPE    AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_LOUD_SPEAKER_INPUT_PATH     AUD_MIC_LOUD_SPEAKER
#define TGT_AUD_CONFIG_LOUD_SPEAKER_MIXAUDIO       TRUE

#define TGT_AUD_CONFIG_BT_DRIVER                   Bt
#define TGT_AUD_CONFIG_BT_PARAM                    0
#define TGT_AUD_CONFIG_BT_OUTPUT_PATH              AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_BT_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_BT_INPUT_PATH               AUD_MIC_LOUD_SPEAKER
#define TGT_AUD_CONFIG_BT_MIXAUDIO                 FALSE

#define TGT_AUD_CONFIG_BT_EP_DRIVER                CodecGallite
#define TGT_AUD_CONFIG_BT_EP_PARAM                 0
#define TGT_AUD_CONFIG_BT_EP_OUTPUT_PATH           AUD_SPK_EAR_PIECE
#define TGT_AUD_CONFIG_BT_EP_OUTPUT_TYPE           AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_BT_EP_INPUT_PATH            AUD_MIC_LOUD_SPEAKER
#define TGT_AUD_CONFIG_BT_EP_MIXAUDIO              FALSE

#define TGT_AUD_CONFIG_BT_SP_DRIVER                CodecGallite
#define TGT_AUD_CONFIG_BT_SP_PARAM                 0
#define TGT_AUD_CONFIG_BT_SP_OUTPUT_PATH           AUD_SPK_LOUD_SPEAKER
#define TGT_AUD_CONFIG_BT_SP_OUTPUT_TYPE           AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_BT_SP_INPUT_PATH            AUD_MIC_LOUD_SPEAKER
#define TGT_AUD_CONFIG_BT_SP_MIXAUDIO              TRUE

#define TGT_AUD_CONFIG_FM_DRIVER                   Fm
#define TGT_AUD_CONFIG_FM_PARAM                    0
#define TGT_AUD_CONFIG_FM_OUTPUT_PATH              AUD_SPK_LOUD_SPEAKER
#define TGT_AUD_CONFIG_FM_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_FM_INPUT_PATH               AUD_MIC_RECEIVER
#define TGT_AUD_CONFIG_FM_MIXAUDIO                 TRUE

#define TGT_AUD_CONFIG_LINEIN_DRIVER                   Linein
#define TGT_AUD_CONFIG_LINEIN_PARAM                    0
#define TGT_AUD_CONFIG_LINEIN_OUTPUT_PATH              AUD_SPK_LOUD_SPEAKER
#define TGT_AUD_CONFIG_LINEIN_OUTPUT_TYPE              AUD_SPEAKER_STEREO
#define TGT_AUD_CONFIG_LINEIN_INPUT_PATH               AUD_MIC_LOUD_SPEAKER
#define TGT_AUD_CONFIG_LINEIN_MIXAUDIO                 TRUE

#endif // TGT_AUD_CONFIG



// #############################################################################
// #                                                                           #
// #                           OPTIONNAL DRIVERS                               #
// #                                                                           #
// #############################################################################



#ifdef TGT_WITH_MCD_IF_spi
// =============================================================================
// MCD config
// -----------------------------------------------------------------------------
/// @Elem HAL_SPI_CS0
/// @Elem HAL_GPIO_13 GPIO over CS0 used for MMC init process step to drive CS
/// to 1 during at least 74 clocks.
// =============================================================================
#ifndef TGT_MCD_CONFIG
#define TGT_MCD_CONFIG                                                  \
    {                                                                   \
        HAL_SPI_CS1,                                                    \
        HAL_GPIO_0_7                                                      \
    }
#endif // TGT_MCD_CONFIG
#endif
#ifdef TGT_WITH_MCD_IF_sdmmc
// =============================================================================
// MCD config
// -----------------------------------------------------------------------------
/// @elem cardDetectGpio
/// Gpio Connected to socket to detect card insertion/removal .
/// (set to #HAL_GPIO_NONE) if not available.
/// @elem gpioCardDetectHigh
/// Define the polarity of the above GPIO: \c TRUE when active high,
/// \c FALSE when active low.
/// @elem dat3HasPullDown
/// Wether or not the DAT3/CD line has a pull-down and can be used for 
/// minimalist new card detection (no insertion/removal interrupt.)
// =============================================================================
#ifndef TGT_MCD_CONFIG
#define TGT_MCD_CONFIG                                                  \
    {                                                                   \
        .dat3HasPullDown    = FALSE                                     \
    }
#endif // TGT_MCD_CONFIG
#endif

#ifdef TGT_WITH_TS_MODEL_tsd_5856
// =============================================================================
// TSD config
// -----------------------------------------------------------------------------
/// This fills the TSD_CONFIG_T structure
// =============================================================================
#ifndef TGT_TSD_CONFIG
#define TGT_TSD_CONFIG                                                  \
    {                                                                   \
        .penGpio        = HAL_GPIO_0_4,                                   \
        .debounceTime   = 5*HAL_TICK1S/1000,                            \
        .downPeriod     = 6,                                            \
        .upPeriod       = 6,                                            \
        .maxError       = 0x50,                                         \
        .keyCount       = 6,                                            \
        .minVolt        =  0,                                          \
        .maxVolt        = 1820,                                         \
        .resistance     = {0,2000,5100,11000,22000,68000},					\
    }
#endif // TGT_TSD_CONFIG

#ifndef TGT_TSD_KEY_ADC_VALUE_ARRAY
#define TGT_TSD_KEY_ADC_VALUE_ARRAY                                     \
    {                                                                   \
        0x07C, 0x0CE, 0x120, 0x171, 0x1C3,                              \
        0x215, 0x266, 0x2B8, 0x30A, 0x35B,                              \
    }
#endif

#ifndef TGT_TSD_KEY2_ADC_VALUE_ARRAY
#define TGT_TSD_KEY2_ADC_VALUE_ARRAY                                    \
    {                                                                   \
        0x06B, 0x0B1, 0x0F7, 0x13D, 0x183,                              \
        0x1C9, 0x20F, 0x255, 0x29B, 0x2E1,                              \
        0x327, 0x36D,                                                   \
    }
#endif
#endif // TGT_WITH_TS_MODEL_rda1203_gallite

#ifdef TGT_WITH_FM_MODEL_rda5870 
// =============================================================================
// FMD config
// -----------------------------------------------------------------------------
/// This fills the FMD_CONFIG_T structure for RDA5802e_gallite
/// @Elem HAL_SPI Either SPI1 or SPI2
/// @Elem HAL_SPI_CS1 Chip select on the selected SPI ID
// =============================================================================
#ifndef TGT_FMD_CONFIG
#define TGT_FMD_CONFIG                                                  \
    {                                                                   \
        .powerOnTime        = 16384,                                    \
        .channelSpacing     = FMD_CHANNEL_SPACE_100,                    \
        .seekRSSIThreshold  = 0x10                                      \
    }

#endif // TGT_FMD_CONFIG
#endif // TGT_WITH_FM_MODEL_rda5870
#ifdef TGT_WITH_FM_MODEL_rda5802na
// =============================================================================
#ifndef TGT_FMD_CONFIG
#define TGT_FMD_CONFIG                                                  \
{                                                                   \
	 .i2cBusId	= HAL_I2C_BUS_ID_2,		\
        .i2cBps             = HAL_I2C_BPS_100K,                         \
        .powerOnTime        = 1600,                                     \
        .volumeVal          = {0,0x2,0x04,0x06,0x08,0x0a,0x0c,0x0f},    \
        .channelSpacing     = FMD_CHANNEL_SPACE_100,                    \
        .seekRSSIThreshold  = 0x05,                                     \
}
#endif // TGT_FMD_CONFIG 
#endif 
#ifdef TGT_WITH_FM_MODEL_rdafm_5856
// =============================================================================
#ifndef TGT_FMD_CONFIG
#define TGT_FMD_CONFIG                                                  \
{                                                                   \
	 .i2cBusId	= HAL_I2C_BUS_ID_2,		\
        .i2cBps             = HAL_I2C_BPS_100K,                         \
        .powerOnTime        = 1600,                                     \
        .volumeVal          = {0,0x2,0x04,0x06,0x08,0x0a,0x0c,0x0f},    \
        .channelSpacing     = FMD_CHANNEL_SPACE_100,                    \
        .seekRSSIThreshold  = 0x05,                                     \
}
#endif // TGT_FMD_CONFIG 
#endif 
#ifdef TGT_WITH_FM_MODEL_rda5876
// =============================================================================
#ifndef TGT_FMD_CONFIG
#define TGT_FMD_CONFIG                                                  \
{                                                                   \
	 .i2cBusId	= HAL_I2C_BUS_ID_2,		\
        .i2cBps             = HAL_I2C_BPS_100K,                         \
        .powerOnTime        = 1600,                                     \
        .volumeVal          = {0,0x2,0x04,0x06,0x08,0x0a,0x0c,0x0f},    \
        .channelSpacing     = FMD_CHANNEL_SPACE_100,                    \
        .seekRSSIThreshold  = 0x05,                                     \
}
#endif // TGT_FMD_CONFIG 
#endif //TGT_WITH_FM_MODEL_rda5876

#if 0
#ifdef TGT_WITH_GSENSOR_MODEL_mma7660fc
// =============================================================================
//  Gsensor config
// -----------------------------------------------------------------------------
/// This fills the Gsensor CONFIG_T structure for MMA7660FC
// =============================================================================
#ifndef TGT_GSENSOR_CONFIG
#define TGT_GSENSOR_CONFIG                                              \
    {                                                                   \
        .i2cBusId           = HAL_I2C_BUS_ID_2,                         \
        .gsensorGpio        = HAL_GPIO_3                                \
    }
#endif // TGT_GSENSOR_CONFIG
#endif // TGT_WITH_GSENSOR_MODEL_mma7660fc

#ifdef TGT_WITH_GSENSOR_MODEL_mma7660fc_gpio
// =============================================================================
//  GsensorGpio config
// -----------------------------------------------------------------------------
/// This fills the Gsensor CONFIG_T structure for MMA7660FC GPIO
// =============================================================================
#ifndef TGT_GSENSOR_CONFIG
#define TGT_GSENSOR_CONFIG                                              \
    {                                                                   \
        .i2cBusId           = HAL_I2C_BUS_ID_2,                         \
        .gsensorGpio        = HAL_GPIO_5,                               \
        .scl_i2c_gpio       = HAL_GPIO_NONE ,                           \
        .scl_i2c_gpo        = HAL_GPO_4 ,                               \
        .sda_i2c            = HAL_GPIO_3                                \
}
#endif // TGT_GSENSOR_CONFIG 
#endif // TGT_WITH_GSENSOR_MODEL_mma7660fc_gpio

#endif

#ifdef TGT_WITH_GPIOI2C_MODEL_i2c_gpio
// =============================================================================
//  GPIO I2C config
// -----------------------------------------------------------------------------
/// This fills the GPIOI2C CONFIG_T structure
// =============================================================================
#ifndef TGT_GPIOI2C_CONFIG
#define TGT_GPIOI2C_CONFIG                                              \
    {                                                                   \
        .i2c_gpio_Bps       = GPIO_I2C_BPS_80K ,                        \
        .scl_i2c_gpio       = HAL_GPIO_6_3 ,                             \
        .scl_i2c_gpo        = HAL_GPO_NONE ,                            \
        .sda_i2c            = HAL_GPIO_6_2                               \
}
#endif // TGT_GPIOI2C_CONFIG 
#endif // TGT_WITH_GPIOI2C_MODEL_i2c_gpio


// =============================================================================
// BTD config
// -----------------------------------------------------------------------------
/// This fills the BTD_CONFIG_T structure for RDA5868
// =============================================================================
#ifndef TGT_BTD_CONFIG
#define TGT_BTD_CONFIG                                                  \
    {                                                                   \
        .i2cBusId  = HAL_I2C_BUS_ID_2,                                  \
        .pinReset  = { .gpoId  = HAL_GPO_NONE },                        \
        .pinWakeUp = { .gpioId = HAL_GPIO_NONE },                       \
        .pinSleep  = { .gpioId = HAL_GPIO_NONE }                           \
    }
#endif // TGT_BTD_CONFIG


// =============================================================================
// TGT_SIMD_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for SIMD
// =============================================================================
#ifndef TGT_SIMD_CONFIG
#define TGT_SIMD_CONFIG                                                 \
{                                                                       \
}
#endif // TGT_SIMD_CONFIG


// =============================================================================
// DSIM config
// -----------------------------------------------------------------------------
/// This fills the dual sim configuration
// =============================================================================
#ifndef TGT_DUALSIM_CONFIG
#define TGT_DUALSIM_CONFIG                                              \
    {                                                                   \
        HAL_SPI,                                                        \
        HAL_SPI_CS1                                                     \
    }
#endif // TGT_DUALSIM_CONFIG


// =============================================================================
// TGT_CAMD_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for CAMD
// =============================================================================
#ifndef TGT_CAMD_CONFIG
#define TGT_CAMD_CONFIG                                                 \
{                                                                       \
    .i2cBusId          = HAL_I2C_BUS_ID_2,                              \
}
#endif // TGT_CAMD_CONFIG


// =============================================================================
// TGT_CAMS_CONFIG
// -----------------------------------------------------------------------------
/// Configuration for CAMS
// =============================================================================
#ifndef TGT_CAMS_CONFIG
#define TGT_CAMS_CONFIG                                                 \
{                                                                       \
    .sensorOrientation = 1                                              \
}
#endif // TGT_CAMS_CONFIG


// =============================================================================
// ?d config
// -----------------------------------------------------------------------------
/// @todo add other driver configuration here if needed
// =============================================================================



// #############################################################################
// #                                                                           #
// #                                 SERVICES                                  #
// #                                                                           #
// #############################################################################



// =============================================================================
// ?s config
// -----------------------------------------------------------------------------
/// @todo add other service configuration here if needed
// =============================================================================




#endif //_TGT_BOARD_CFG_H_

