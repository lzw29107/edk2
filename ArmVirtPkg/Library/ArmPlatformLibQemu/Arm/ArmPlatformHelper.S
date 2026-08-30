//
//  Copyright (c) 2012-2013, ARM Limited. All rights reserved.
//
//  SPDX-License-Identifier: BSD-2-Clause-Patent
//

#include <AsmMacroLib.h>
#include <Library/ArmLib.h>

ASM_FUNC(ArmPlatformPeiBootAction)
  mov   r12, lr
  bl    ArmPlatformIsPrimaryCore
  cmp   r0, #1
  beq   PrimaryCore
  bl    ArmPlatformIsGicV2OrLower
  cmp   r0, #1
  beq   SecondaryParking
PrimaryCore:
  mov   lr, r12
  bx    lr

ASM_FUNC(ArmPlatformIsGicV2OrLower)
  MOV32 (r1, FixedPcdGet32 (PcdCpuGicDistributorBase))
  ldr   r0, [r1, #0x4]
  and   r0, r0, #0x1f
  cmp   r0, #0x1f
  moveq r0, #0
  movne r0, #1
  bx    lr

ASM_FUNC(SecondaryParking)
  mrc   p15, 0, r0, c0, c0, 5
  and   r0, r0, #0xff
  mov   r4, r0
  MOV32 (r5, FixedPcdGet32 (PcdCpuMailboxBase))
  add   r5, r5, r0, lsl #12
  MOV32 (r1, FixedPcdGet32 (PcdCpuGicInterruptInterfaceBase))
  mov   r2, #1
  str   r2, [r1, #0]
  mov   r2, #0xff
  str   r2, [r1, #4]
  str   r4, [r5, #0]
  mov   r2, #0
  str   r2, [r5, #8]
  dsb
  isb
1:
  wfi
  ldr   r3, [r5, #0]
  cmp   r3, r4
  bne   1b
  ldr   r3, [r5, #8]
  cmp   r3, #0
  beq   1b
  mov   r2, #0
  str   r2, [r5, #8]
  mrc   p15, 0, r0, c1, c0, 0
  bic   r0, r0, #1
  bic   r0, r0, #4
  bic   r0, r0, #0x1000
  mcr   p15, 0, r0, c1, c0, 0
  dsb
  isb
  msr   cpsr_c, #0xd3
  mov   r0, r5
  bx    r3

ASM_FUNC(ArmPlatformGetCorePosition)
  and   r0, r0, #0xff
  bx    lr

ASM_FUNC(ArmPlatformIsPrimaryCore)
  mrc   p15, 0, r0, c0, c0, 5
  and   r0, r0, #0xff
  cmp   r0, #0
  moveq r0, #1
  movne r0, #0
  bx    lr

ASM_FUNC(ArmPlatformGetPrimaryCoreMpId)
  mov   r0, #0
  bx    lr
