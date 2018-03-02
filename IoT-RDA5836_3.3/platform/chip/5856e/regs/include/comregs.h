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
     
//==============================================================================
//
/// @file
//
//==============================================================================

#ifndef _COMREGS_H_
#define _COMREGS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'comregs'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// COMREGS_T
// -----------------------------------------------------------------------------
/// 
// =============================================================================
#define REG_SYS_COMREGS_BASE        0x01A0B000
#define REG_BB_COMREGS_BASE         0x0190E000

typedef volatile struct
{
    REG32                          Snapshot;                     //0x00000000
    REG32                          Snapshot_Cfg;                 //0x00000004
    REG32                          Cause;                        //0x00000008
    REG32                          Mask_Set;                     //0x0000000C
    REG32                          Mask_Clr;                     //0x00000010
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Set;                    //0x00000014
    /// If accesses to ItReg_Set and ItReg_Clr registers are done simultaneously
    /// from both CPUs and affecting the same bits, the priority is given to set
    /// a bit.
    REG32                          ItReg_Clr;                    //0x00000018
} HWP_COMREGS_T;

#define hwp_sysComregs              ((HWP_COMREGS_T*) KSEG1(REG_SYS_COMREGS_BASE))
#define hwp_bbComregs               ((HWP_COMREGS_T*) KSEG1(REG_BB_COMREGS_BASE))


//Snapshot
#define COMREGS_SNAPSHOT(n)         (((n)&3)<<0)

//Snapshot_Cfg
#define COMREGS_SNAPSHOT_CFG_WRAP_2 (2<<0)
#define COMREGS_SNAPSHOT_CFG_WRAP_3 (3<<0)

//Cause
#define COMREGS_IRQ0_CAUSE(n)       (((n)&0xFF)<<0)
#define COMREGS_IRQ0_CAUSE_MASK     (0xFF<<0)
#define COMREGS_IRQ0_CAUSE_SHIFT    (0)
#define COMREGS_IRQ1_CAUSE(n)       (((n)&0xFF)<<8)
#define COMREGS_IRQ1_CAUSE_MASK     (0xFF<<8)
#define COMREGS_IRQ1_CAUSE_SHIFT    (8)

//Mask_Set
#define COMREGS_IRQ0_MASK_SET(n)    (((n)&0xFF)<<0)
#define COMREGS_IRQ0_MASK_SET_MASK  (0xFF<<0)
#define COMREGS_IRQ0_MASK_SET_SHIFT (0)
#define COMREGS_IRQ1_MASK_SET(n)    (((n)&0xFF)<<8)
#define COMREGS_IRQ1_MASK_SET_MASK  (0xFF<<8)
#define COMREGS_IRQ1_MASK_SET_SHIFT (8)

//Mask_Clr
#define COMREGS_IRQ0_MASK_CLR(n)    (((n)&0xFF)<<0)
#define COMREGS_IRQ0_MASK_CLR_MASK  (0xFF<<0)
#define COMREGS_IRQ0_MASK_CLR_SHIFT (0)
#define COMREGS_IRQ1_MASK_CLR(n)    (((n)&0xFF)<<8)
#define COMREGS_IRQ1_MASK_CLR_MASK  (0xFF<<8)
#define COMREGS_IRQ1_MASK_CLR_SHIFT (8)

//ItReg_Set
#define COMREGS_IRQ0_SET(n)         (((n)&0xFF)<<0)
#define COMREGS_IRQ0_SET_MASK       (0xFF<<0)
#define COMREGS_IRQ0_SET_SHIFT      (0)
#define COMREGS_IRQ1_SET(n)         (((n)&0xFF)<<8)
#define COMREGS_IRQ1_SET_MASK       (0xFF<<8)
#define COMREGS_IRQ1_SET_SHIFT      (8)

//ItReg_Clr
#define COMREGS_IRQ0_CLR(n)         (((n)&0xFF)<<0)
#define COMREGS_IRQ1_CLR(n)         (((n)&0xFF)<<8)




#endif

