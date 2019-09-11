/*----------------------------------------------------------------------------
 * Copyright (c) <2013-2015>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/

#ifndef _LOS_MEMSTAT_PH
#define _LOS_MEMSTAT_PH

#include "los_typedef.h"
#include "los_memory.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cplusplus */
#endif /* __cplusplus */

typedef struct {
    UINT32 uwMemUsed;
} TSK_MEM_USED_INFO;

extern VOID osTaskMemUsedInc(UINT32 uwUsedSize, UINT32 uwTaskID);
extern VOID osTaskMemUsedDec(UINT32 uwUsedSize, UINT32 uwTaskID);
extern UINT32 osTaskMemUsage(UINT32 uwTaskID);
extern VOID osTaskMemClear(UINT32 uwTaskID);
#define OS_MEM_ENABLE_MEM_STATISTICS


#ifdef LOS_MEM_SLAB
typedef struct {
    UINT32 uwSlabUsed;
} TSK_SLAB_USED_INFO;

extern VOID osTaskSlabUsedInc(UINT32 uwUsedSize, UINT32 uwTaskID);
extern VOID osTaskSlabUsedDec(UINT32 uwUsedSize, UINT32 uwTaskID);
extern UINT32 osTaskSlabUsage(UINT32 uwTaskID);
#endif

#ifdef OS_MEM_ENABLE_MEM_STATISTICS
#define OS_MEM_ADD_USED(uwUsedSize, uwTaskID)            osTaskMemUsedInc(uwUsedSize, uwTaskID)
#define OS_MEM_REDUCE_USED(uwUsedSize, uwTaskID)      osTaskMemUsedDec(uwUsedSize, uwTaskID)
#define OS_MEM_CLEAR(uwTaskID)                                      osTaskMemClear(uwTaskID)
#ifdef LOS_MEM_SLAB
#define OS_SLAB_ADD_USED(uwUsedSize, uwTaskID)           osTaskSlabUsedInc(uwUsedSize, uwTaskID)
#define OS_SLAB_REDUCE_USED(uwUsedSize, uwTaskID)     osTaskSlabUsedDec(uwUsedSize, uwTaskID)
#endif
#else
#define OS_MEM_ADD_USED(uwUsedSize, uwTaskID)
#define OS_MEM_REDUCE_USED(uwUsedSize, uwTaskID)
#define OS_MEM_CLEAR(uwTaskID)
#ifdef LOS_MEM_SLAB
#define OS_SLAB_ADD_USED(uwUsedSize, uwTaskID)
#define OS_SLAB_REDUCE_USED(uwUsedSize, uwTaskID)
#endif
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */

#endif /* _LOS_MEMSTAT_PH */
