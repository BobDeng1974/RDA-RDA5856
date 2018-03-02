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


#ifndef _BOOT_VERSION_H_
#define _BOOT_VERSION_H_

// =============================================================================
//  MACROS
// =============================================================================

#define BOOT_VERSION_REVISION                     (-1)

// =============================================================================
//  TYPES
// =============================================================================

#ifndef BOOT_VERSION_NUMBER
#define BOOT_VERSION_NUMBER                       (0)
#endif

#ifndef BOOT_VERSION_DATE
#define BOOT_VERSION_DATE                         (BUILD_DATE)
#endif

#ifndef BOOT_VERSION_STRING
#define BOOT_VERSION_STRING                       "BOOT version string not defined"
#endif

#ifndef BOOT_VERSION_STRING_WITH_BRANCH
#define BOOT_VERSION_STRING_WITH_BRANCH           BOOT_VERSION_STRING " Branch: " "none"
#endif

#define BOOT_VERSION_STRUCT                       {BOOT_VERSION_REVISION, \
                                                  BOOT_VERSION_NUMBER, \
                                                  BOOT_VERSION_DATE, \
                                                  BOOT_VERSION_STRING_WITH_BRANCH}

#endif // _BOOT_VERSION_H_
