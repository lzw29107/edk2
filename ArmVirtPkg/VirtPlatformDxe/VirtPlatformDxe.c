/** @file
*  FDT client protocol driver for qemu,mach-virt
*
*  Copyright (c) 2024, Linaro Ltd. All rights reserved.
*
*  SPDX-License-Identifier: BSD-2-Clause-Patent
*
**/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/PcdLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/FdtClient.h>

EFI_STATUS
EFIAPI
InitializeVirtPlatformDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS       Status;
  FDT_CLIENT_PROTOCOL   *FdtClient;
  CONST UINT64          *Reg;
  UINT32                RegSize;
  UINTN                 AddressCells, SizeCells;
  UINTN                Base, Size;
  BOOLEAN               UseXhci;

  DEBUG ((DEBUG_INFO, "%a: InitializeVirtPlatformDxe called\n", __FUNCTION__));

  Status = gBS->LocateProtocol (&gFdtClientProtocolGuid, NULL,
                  (VOID **)&FdtClient);
  ASSERT_EFI_ERROR (Status);

  UseXhci = TRUE;
  Status = FdtClient->FindCompatibleNodeReg (
                             FdtClient,
                             "generic-xhci",
                             (CONST VOID **)&Reg,
                             &AddressCells,
                             &SizeCells,
                             &RegSize
                             );
  if (Status == EFI_NOT_FOUND) {
    UseXhci = FALSE;
    Status      = FdtClient->FindCompatibleNodeReg (
                               FdtClient,
                               "generic-ehci",
                               (CONST VOID **)&Reg,
                               &AddressCells,
                               &SizeCells,
                               &RegSize
                               );
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (AddressCells == 2);
  ASSERT (SizeCells == 2);
  ASSERT (RegSize == 2 * sizeof (UINT64));

  Base = (UINTN)SwapBytes64 (Reg[0]);
  Size = (UINTN)SwapBytes64 (Reg[1]);

  if (UseXhci) {
    DEBUG ((
      DEBUG_INFO,
      "%a: Got platform xHCI %llx %u\n",
      __FUNCTION__,
      Base,
      Size
      ));

    Status = RegisterNonDiscoverableMmioDevice (
               NonDiscoverableDeviceTypeXhci,
               NonDiscoverableDeviceDmaTypeCoherent,
               NULL,
               NULL,
               1,
               Base,
               Size
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: NonDiscoverable: Cannot install xHCI device @%p (Status == %r)\n",
        __FUNCTION__,
        Base,
        Status
        ));
      return Status;
    }
  } else if (Status != EFI_NOT_FOUND) {
      DEBUG ((
      DEBUG_INFO,
      "%a: Got platform EHCI %llx %u\n",
      __FUNCTION__,
      Base,
      Size
      ));

    Status = RegisterNonDiscoverableMmioDevice (
               NonDiscoverableDeviceTypeEhci,
               NonDiscoverableDeviceDmaTypeCoherent,
               NULL,
               NULL,
               1,
               Base,
               Size
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: NonDiscoverable: Cannot install EHCI device @%p (Status == %r)\n",
        __FUNCTION__,
        Base,
        Status
        ));
      return Status;
    }
  }

  Status = FdtClient->FindCompatibleNodeReg (
                             FdtClient,
                             "sdhci",
                             (CONST VOID **)&Reg,
                             &AddressCells,
                             &SizeCells,
                             &RegSize
                             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (AddressCells == 2);
  ASSERT (SizeCells == 2);
  ASSERT (RegSize == 2 * sizeof (UINT64));

  Base = (UINTN)SwapBytes64 (Reg[0]);
  Size = (UINTN)SwapBytes64 (Reg[1]);

  if (Status != EFI_NOT_FOUND) {
    DEBUG ((
      DEBUG_INFO,
      "%a: Got platform SDHCI %llx %u\n",
      __FUNCTION__,
      Base,
      Size
    ));

    Status = RegisterNonDiscoverableMmioDevice (
               NonDiscoverableDeviceTypeSdhci,
               NonDiscoverableDeviceDmaTypeCoherent,
               NULL,
               NULL,
               1,
               Base,
               Size
               );

    if (EFI_ERROR (Status)) {
      DEBUG ((
        DEBUG_ERROR,
        "%a: NonDiscoverable: Cannot install SDHCI device @%p (Status == %r)\n",
        __FUNCTION__,
        Base,
        Status
        ));
      return Status;
    }
  }
  return EFI_SUCCESS;
}