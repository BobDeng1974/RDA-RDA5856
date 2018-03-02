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
///                                                                           //
/// @file boot_mem_init.h                                                           //
///                                                                           //
/// Boot memory init driver API                                                            //
///                                                                           //
//////////////////////////////////////////////////////////////////////////////// 

#ifndef _BOOT_MEM_INIT_H_
#define _BOOT_MEM_INIT_H_

#include "cs_types.h"
#include "hal_sys.h"
#include "hal_ebc.h"

/// @defgroup mem_init BOOT MEM INIT Driver
///
/// This document describes the characteristics of the mem init Driver 
/// and how to use it via its Hardware Abstraction Layer API.
///
///
/// @{
///


// ============================================================================
// TYPES
// ============================================================================

// ============================================================================
// FUNCTIONS
// ============================================================================


// ============================================================================
// boot_EbcCsSetupWithCallback
// ----------------------------------------------------------------------------
/// Special handler in internal sram, will call a func that must also be in sram that is responsible for
/// switching the flash/sram in burst mode. No function not in internal sram should be called.
/// stack must be also in internal sram
/// no other master (bcpu, dma, voc ...) should access the CS, ebc lock prevent this
///
/// @param cs Chip Select to Enable.
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
// ============================================================================
PROTECTED VOID boot_EbcCsSetupWithCallback(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc);


// ============================================================================
// boot_EbcSramOpen
// ----------------------------------------------------------------------------
/// Enable a CS for SRAM. The chip selects for 
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given 
/// chip select, and must support the external maximal clock
///
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @param cbFunc callback function for enabling the burst mode on sram
/// or NULL if not needed. Note: cbFunc must not be in sram
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* boot_EbcSramOpen(CONST HAL_EBC_CS_CFG_T* csConfig, HAL_EBC_BURSTCALLBACK_T* cbFunc);


// ============================================================================
// boot_EbcCsOpen
// ----------------------------------------------------------------------------
/// Enable a CS other than the one for FLASH.
/// The enabling of a CS returns the base address of the configured space.
/// The settings are given at the opening of the peripheral on the given 
/// chip select, and must support the external maximal clock.
///
/// @param cs Chip Select to Enable. (HAL_EBC_SRAM and CS2 or above)
/// @param csConfig Configuration for the chip select. The \c csEn enable bit
/// of the mode configuration must be set to 1 there if the chip select
/// of the opened peripheral have to be enabled !
/// @return Base address of the configured space
// ============================================================================
PUBLIC VOID* boot_EbcCsOpen(HAL_EBC_CS_T cs, CONST HAL_EBC_CS_CFG_T* csConfig);


// ============================================================================
// boot_EbcCsSetup
// ----------------------------------------------------------------------------
/// Configure the CS settings according to the system clock frequency
/// This function configures the CS time and mode settings.
///
/// @param sysFreq System frequency to which adjust 
/// the CS settings
// ============================================================================
PUBLIC VOID boot_EbcCsSetup(HAL_SYS_MEM_FREQ_T ebcFreq);


// =============================================================================
// boot_MemInit
// -----------------------------------------------------------------------------
/// Initialize flash memory and RAM memory.
///
/// @param bootSectorStructValid whether boot sector structure is valid.
// =============================================================================
PUBLIC VOID boot_MemInit(BOOL bootSectorStructValid);


#ifdef __PRJ_WITH_SDRAM__
PUBLIC VOID boot_SysSetSdramTiming(HAL_SYS_MEM_FREQ_T MPMCFreq);
PUBLIC VOID boot_SysSetMPMCTiming(HAL_SYS_MEM_FREQ_T MPMCFreq);
#endif


///  @} <- End of the EBC group 


#endif // _BOOT_MEM_INIT_H_
