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
/// @file sxs_mutx.c
/// Systeme executif task mutual exclusion management.
//
////////////////////////////////////////////////////////////////////////////////

#include "sxs_type.h"
#include "sxr_mutx.h"
#include "sxp_mutx.h"
#include "sxr_sbx.h"
#include "sxp_debug.h"
#include "sxr_tksd.hp"

#include "tgt_m.h"

// =============================================================================
// sxr_TakeMutex
// -----------------------------------------------------------------------------
/// Take the mutex.
/// @param Id mutex Id.
/// @return UserId to give to sxr_ReleaseMutex()
// =============================================================================
u8    sxr_TakeMutex (u8 Id)
{
    u32   Status;
    u32   CallerAdd;
    u32   K1;
    u8    TaskId;

    SXS_GET_RA(&CallerAdd);

    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");

    /// @todo move that to hal
    asm volatile("move %0, $27":"=d"(K1));
    SX_MUTX_ASSERT(0 == K1, "sxr_TakeMutex Must be called from a Task!");
    
    Status = sxr_EnterSc ();
    TaskId = sxr_Task.Active;
    // current task already has the mutex ?
    if (sxr_Mutex.Queue [Id].TaskId != TaskId)
    {
        // other task
        // the task switch that might occur here will restore the Sc state.
        sxr_TakeSemaphore (sxr_Mutex.Queue [Id].SemaId);
        sxr_Mutex.Queue [Id].TaskId = TaskId;
        sxr_Mutex.Queue [Id].CallerAdd = CallerAdd;
    }
    sxr_Mutex.Queue [Id].Count++;

    sxr_ExitSc (Status);
    return TaskId;
}

// =============================================================================
// sxr_ReleaseMutex
// -----------------------------------------------------------------------------
/// Release a mutex.
/// @param Id mutex Id.
/// @param UserId from sxr_TakeMutex()
// =============================================================================
void  sxr_ReleaseMutex (u8 Id, u8 UserId)
{
    u32   K1;
    u32   Status = sxr_EnterSc ();
#ifndef SX_NO_ASSERT
    u8    TaskId = sxr_Task.Active;
#endif
    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");
    // ensure mutex is taken
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Count != 0, "sxr_ReleaseMutex while not taken!");
    /// @todo move that to hal
    asm volatile("move %0, $27":"=d"(K1));
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].TaskId == UserId, "sxr_ReleaseMutex Must be released by the UserId that took it!");
    /// @todo 0x10000000 is from halp_irq_handler.h
    SX_MUTX_ASSERT(0x01000000 > K1, "sxr_ReleaseMutex called from a Job!");
    // allow IRQ to free a mutex for a task...
    SX_MUTX_ASSERT((K1 != 0) || (TaskId == UserId), "sxr_ReleaseMutex Must be released by the Task that took it!");


    sxr_Mutex.Queue [Id].Count--;

    if (sxr_Mutex.Queue [Id].Count == 0)
    {
        sxr_Mutex.Queue [Id].TaskId = SXR_NO_TASK;
        sxr_ReleaseSemaphore (sxr_Mutex.Queue [Id].SemaId);
    }
    sxr_ExitSc (Status);
}

// =============================================================================
// sxr_MutexHot
// -----------------------------------------------------------------------------
/// Check if a mutex is free.
/// @param Id mutex Id.
/// @return \c TRUE when the mutex is free, FALSE when it is hot.
// =============================================================================
u8    sxr_MutexHot (u8 Id)
{
    // ensure mutex is allocated
    SX_MUTX_ASSERT(Id < SXR_NB_MAX_MUTEX, "sxr_ReleaseMutex Invalid mutex ID %d", Id);
    SX_MUTX_ASSERT(sxr_Mutex.Queue [Id].Next == SXR_MUTEX_ALLOCATED, "sxr_ReleaseMutex mutex is not allocated!");
    return sxr_SemaphoreHot(sxr_Mutex.Queue [Id].SemaId);
}

