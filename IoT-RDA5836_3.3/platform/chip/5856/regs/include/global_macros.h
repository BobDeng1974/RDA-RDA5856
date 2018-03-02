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

#ifndef _GLOBAL_MACROS_H_
#define _GLOBAL_MACROS_H_



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================



#ifdef CHIP_HAS_NOCACHE
#define KSEG0(addr)     ( (addr) | 0xa0000000 )
#else
#define KSEG0(addr)     ( (addr) | 0x80000000 )
#endif
#define KSEG1(addr)     ( (addr) | 0xa0000000 )


/* Define access cached or uncached */
#ifdef CHIP_HAS_NOCACHE
#define MEM_ACCESS_CACHED(addr)   ((UINT32*)((UINT32)(addr)|0x20000000))
#else
#define MEM_ACCESS_CACHED(addr)     ((UINT32*)((UINT32)(addr)&0xdfffffff))
#endif
#define MEM_ACCESS_UNCACHED(addr)   ((UINT32*)((UINT32)(addr)|0x20000000))

/* Register access for assembly */
#define BASE_HI(val) (((0xa0000000 | val) & 0xffff8000) + (val & 0x8000))
#define BASE_LO(val) (((val) & 0x7fff) - (val & 0x8000))


/* to extract bitfield from register value */
#define GET_BITFIELD(dword, bitfield) (((dword) & (bitfield ## _MASK)) >> (bitfield ## _SHIFT))

#define EXP2(n) (1<<(n))



#endif

