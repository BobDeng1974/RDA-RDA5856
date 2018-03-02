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
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
/// @file hal_open.h                                                         //
/// Contains private init related header of Ruby's hal driver
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef __HAL_OPEN_H__
#define __HAL_OPEN_H__

#include "cs_types.h"
#include "hal_config.h"

// =============================================================================
// hal_Open
// -----------------------------------------------------------------------------
/// Initializes the HAL layer. 
/// This function also do the open of the PMD driver and set the idle-mode.
// =============================================================================
PUBLIC VOID hal_Open(CONST HAL_CFG_CONFIG_T*);

#endif // __HAL_OPEN_H__


