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
/// @file halp_irq_prio.h                                                     //
/// Prototypes for private IRQ priorities and handlers                        //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////


#include "cs_types.h"

#include "global_macros.h"
#include "sys_irq.h"

#include "chip_id.h"

#include "halp_gpio.h"
#include "halp_key.h"
#include "halp_timers.h"
#include "halp_calendar.h"
#include "halp_fint.h"
#include "halp_tcu.h"
#include "halp_uart.h"
#include "halp_dma.h"
#include "halp_sys_ifc.h"
#include "halp_sim.h"
#include "halp_spi.h"
#include "halp_lps.h"
#include "halp_comregs.h"
#include "halp_rfspi.h"
#include "halp_voc.h"
#include "halp_aif.h"
#include "halp_usb.h"
#include "halp_camera.h"
#include "halp_gouda.h"
#include "pmd_m.h"
#include "halp_sdio.h"
#include "halp_bt.h"

#include "halp_ir.h"
#include "hal_spi_flash.h"


// =============================================================================
//  MACROS
// =============================================================================

/// Highest priority 0 - no IRQ are re-enabled during treatment
#define MASK_PRIO_0_IRQS            (0)

#define PRIO_MASK_LPS_IRQ           (MASK_PRIO_0_IRQS)

#define PRIO_0_IRQS                 (SYS_IRQ_SYS_IRQ_LPS)

/// priority 1 - Priority 0 IRQs re-enabled
#define MASK_PRIO_1_IRQS            (MASK_PRIO_0_IRQS|PRIO_0_IRQS)

#ifdef RFSPI_IFC_WORKAROUND
// Allow RFSPI IFC config (TCU1 IRQ) during MCP locking time (TCU0 IRQ),
// so that RFSPI IFC config will never be delayed across the real FINT (TCU wrap event).
// Otherwise RFSPI cmds might NOT be sent to XCV strictly before the first rx win starts at
// next frame, resulting in data lost and wrong data timing. 
#define PRIO_MASK_TCU1_IRQ          (MASK_PRIO_1_IRQS)
#define PRIO_1_IRQS_OPTION_TCU1     (SYS_IRQ_SYS_IRQ_TCU1)
#else // !RFSPI_IFC_WORKAROUND
#define PRIO_1_IRQS_OPTION_TCU1     (0)
#endif // !RFSPI_IFC_WORKAROUND
#define PRIO_MASK_USB_IRQ           (MASK_PRIO_1_IRQS)
#define PRIO_1_IRQS                 (PRIO_1_IRQS_OPTION_TCU1|SYS_IRQ_SYS_IRQ_USBC)

/// priority 2 - Priority 1 and higher IRQs re-enabled
#define MASK_PRIO_2_IRQS            (MASK_PRIO_1_IRQS|PRIO_1_IRQS)

#if defined(PAL_WINDOWS_LOCK_MCP_ON_RX) || defined(DCDC_FREQ_DIV_WORKAROUND)
#define PRIO_MASK_TCU0_IRQ          (MASK_PRIO_2_IRQS)
#define PRIO_2_IRQS_OPTION_TCU0     (SYS_IRQ_SYS_IRQ_TCU0)
#else // !(PAL_WINDOWS_LOCK_MCP_ON_RX || DCDC_FREQ_DIV_WORKAROUND)
#define PRIO_2_IRQS_OPTION_TCU0     (0)
#endif // !(PAL_WINDOWS_LOCK_MCP_ON_RX || DCDC_FREQ_DIV_WORKAROUND)

#define PRIO_2_IRQS                 (PRIO_2_IRQS_OPTION_TCU0)

/// priority 3 - Priority 2 and higher IRQs re-enabled
#define MASK_PRIO_3_IRQS            (MASK_PRIO_2_IRQS|PRIO_2_IRQS)

#define PRIO_MASK_COM0_IRQ          (MASK_PRIO_3_IRQS)

#define PRIO_3_IRQS                 (SYS_IRQ_SYS_IRQ_COM0)

/// priority 4 - Priority 3 and higher IRQs re-enabled
#define MASK_PRIO_4_IRQS            (MASK_PRIO_3_IRQS|PRIO_3_IRQS)

#define PRIO_MASK_FRAME_IRQ         (MASK_PRIO_4_IRQS)

#define PRIO_4_IRQS                 (SYS_IRQ_SYS_IRQ_FRAME)

/// priority 5 - Priority 4 and higher IRQs re-enabled
#define MASK_PRIO_5_IRQS            (MASK_PRIO_4_IRQS|PRIO_4_IRQS)

#define PRIO_MASK_VOC_IRQ           (MASK_PRIO_5_IRQS)

#define PRIO_MASK_BBIFC0_IRQ        (MASK_PRIO_5_IRQS)
#define PRIO_MASK_BBIFC1_IRQ        (MASK_PRIO_5_IRQS)

#define PRIO_5_IRQS                 (SYS_IRQ_SYS_IRQ_BBIFC0| \
                                     SYS_IRQ_SYS_IRQ_BBIFC1| \
                                     SYS_IRQ_SYS_IRQ_VOC)

/// priority 6 - Priority 5 and higher IRQs re-enabled
#define MASK_PRIO_6_IRQS            (MASK_PRIO_5_IRQS|PRIO_5_IRQS)

#define PRIO_MASK_DMA_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_GPIO_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_TIMERS_IRQ        (MASK_PRIO_6_IRQS)
#define PRIO_MASK_OS_TIMER_IRQ      (MASK_PRIO_6_IRQS)
#define PRIO_MASK_CALENDAR_IRQ      (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SPI_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_TRACE_UART_IRQ    (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SIM_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_COM1_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_IRC_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_PMU               (MASK_PRIO_6_IRQS)

#ifndef PRIO_MASK_TCU0_IRQ
#define PRIO_MASK_TCU0_IRQ          (MASK_PRIO_6_IRQS)
#endif // !PRIO_MASK_TCU0_IRQ

#ifndef PRIO_MASK_TCU1_IRQ
#define PRIO_MASK_TCU1_IRQ          (MASK_PRIO_6_IRQS)
#endif // !PRIO_MASK_TCU1_IRQ

#define PRIO_MASK_UART_IRQ          (MASK_PRIO_6_IRQS)
#define PRIO_MASK_I2C_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_BT_IRQ            (MASK_PRIO_6_IRQS)
#define PRIO_MASK_KEYPAD_IRQ        (MASK_PRIO_6_IRQS)
//#define PRIO_MASK_USB_IRQ           (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_DMA_TX_IRQ  (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_DMA_RX_IRQ  (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC_IRQ         (MASK_PRIO_6_IRQS)
#define PRIO_MASK_CACHE_IRQ         (MASK_PRIO_6_IRQS)
#define PRIO_MASK_USB_DET_IRQ       (MASK_PRIO_6_IRQS)
#define PRIO_MASK_SDMMC2_IRQ        (MASK_PRIO_6_IRQS)




// =============================================================================
//  GLOBAL VARIABLES
// =============================================================================

// =============================================================================
// g_halHwModuleIrqHandler
// -----------------------------------------------------------------------------
///     Physical interrupts mapping 
///     This order must match the order of the physical interrupts.
// =============================================================================
PRIVATE HAL_MODULE_IRQ_HANDLER_T g_halHwModuleIrqHandler[SYS_IRQ_QTY]
  =
{
    hal_TcuIrqHandler, // IRQ 0x00
    hal_TcuIrqHandler, // IRQ 0x01
    NULL, // hal_FintIrqHandler, // IRQ 0x02
    NULL, // hal_ComregsIrqHandler, // IRQ 0x03
    NULL, // hal_ComregsIrqHandler, // IRQ 0x04
    hal_VocIrqHandler, // IRQ 0x05
    hal_DmaIrqHandler, // IRQ 0x06
    hal_GpioIrqHandler, // IRQ 0x07
    //update by wangxin gpio
    NULL, // IRQ 0x08
    hal_TimTimersIrqHandler, // IRQ 0x09
    hal_TimTickIrqHandler, // IRQ 0x0A
    hal_TimRtcIrqHandler, // IRQ 0x0B
    hal_SpiIrqHandler, // IRQ 0x0C: SPI
    hal_SpiIrqHandler, // IRQ 0x0D: SPI2
    hal_SpiIrqHandler, // IRQ 0x0E SPI3
    NULL, // IRQ 0x0F: No irq on hst_trace uart
    hal_UartIrqHandler, // IRQ 0x10
    hal_UartIrqHandler, // IRQ 0x11
    NULL, // IRQ 0x12: reserved for i2c, no IRQ for it
    hal_BtIrqHandler, // IRQ 0x13: BT
    NULL, // IRQ 0x14: NULL
    NULL, // hal_SimIrqHandler, // IRQ 0x15
    #if defined(__IRC_SUPPORT__)
    hal_IRhandle, // IRQ 0x16: IRC handler//Add new irc handler here...
    #else
    NULL, 
    #endif
    hal_LpsIrqHandler, // IRQ 0x17
    hal_AifIrqHandler, // IRQ 0x18: BBIFC0 handler
    hal_AifIrqHandler, // IRQ 0x19: BBIFC1 handler
    #ifndef DISABLE_USB_SUPPORT
    hal_UsbIrqHandler, // IRQ 0x1As
    #else
    NULL,
    #endif
    hal_FlashCacheIrqHandler,// IRQ 0x1B cache irq
    NULL, // IRQ 0x1C: SDMMC
    #ifndef DISABLE_USB_SUPPORT
    hal_UsbRoleDet, // hal_CameraIrqHandler, // IRQ 0x1D (//hal_UsbRoleDet shaohuima 2016/2/19)
    #else
    NULL, 
    #endif
    pmd_IrqHandler, // IRQ 0x1E: PMU
    NULL, // hal_SdioIrqHandler, // IRQ 0x1F: SDMMC2
};



// =============================================================================
// g_halIrqPriorityMask
// -----------------------------------------------------------------------------
/// Priority mask defining which IRQs have higher priorities than a given one.
// =============================================================================
PRIVATE CONST UINT32 g_halIrqPriorityMask[SYS_IRQ_QTY] =
//     =
{  //  LOGICAL IRQ
    PRIO_MASK_TCU0_IRQ,             // IRQ 0x00: TCU0
    PRIO_MASK_TCU1_IRQ,             // IRQ 0x01: TCU1
    PRIO_MASK_FRAME_IRQ,            // IRQ 0x02: FINT
    PRIO_MASK_COM0_IRQ,             // IRQ 0x03: comreg0/DSP/EQUALISER
    PRIO_MASK_COM1_IRQ,             // IRQ 0x04: comreg1
    PRIO_MASK_VOC_IRQ,              // IRQ 0x05: voc
    PRIO_MASK_DMA_IRQ,              // IRQ 0x06: dma
    PRIO_MASK_GPIO_IRQ,             // IRQ 0x07: GPIO
    PRIO_MASK_KEYPAD_IRQ,           // IRQ 0x08: KEYPAD
    PRIO_MASK_TIMERS_IRQ,           // IRQ 0x09: TICK_TIMER
    PRIO_MASK_OS_TIMER_IRQ,         // IRQ 0x0A: ostick
    PRIO_MASK_CALENDAR_IRQ,         // IRQ 0x0B: calendar/RTC timer
    PRIO_MASK_SPI_IRQ,              // IRQ 0x0C: SPI
    PRIO_MASK_SPI_IRQ,              // IRQ 0x0D: SPI 2
    PRIO_MASK_SPI_IRQ,              // IRQ 0x0E: SPI 3
    PRIO_MASK_TRACE_UART_IRQ,       // IRQ 0x0F: trace uart
    PRIO_MASK_UART_IRQ,             // IRQ 0x10: UART
    PRIO_MASK_UART_IRQ,             // IRQ 0x11: UART2
    PRIO_MASK_I2C_IRQ,              // IRQ 0x12: I2C
    PRIO_MASK_BT_IRQ,               // IRQ 0x13: BT
    PRIO_MASK_I2C_IRQ,              // IRQ 0x14: NULL
    PRIO_MASK_SIM_IRQ,              // IRQ 0x15: SIM
    PRIO_MASK_IRC_IRQ,              // IRQ 0x16: IRC
    PRIO_MASK_LPS_IRQ,              // IRQ 0x17: lps,
    PRIO_MASK_BBIFC0_IRQ,           // IRQ 0x18: BBIFC0
    PRIO_MASK_BBIFC1_IRQ,           // IRQ 0x19: BBIFC1
    PRIO_MASK_USB_IRQ,              // IRQ 0x1A: USB
    PRIO_MASK_CACHE_IRQ,            // IRQ 0x1B: CACHE
    PRIO_MASK_SDMMC_IRQ,            // IRQ 0x1C: SDMMC
    PRIO_MASK_USB_DET_IRQ,           // IRQ 0x1D: CAMERA
    PRIO_MASK_PMU,                  // IRQ 0x1E: PMU
    PRIO_MASK_SDMMC2_IRQ,           // IRQ 0x1F: SDMMC2
};


