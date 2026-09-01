/** @file
 *
 * FDT client protocol driver for qemu,mach-virt
 *
 * Copyright (c) 2024, Linaro Ltd. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause-Patent
 *
 **/

#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/DxeServicesTableLib.h>
#include <Library/NonDiscoverableDeviceRegistrationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiDriverEntryPoint.h>
#include <Protocol/FdtClient.h>

typedef struct {
  CONST CHAR8                    *Compatible;
  NON_DISCOVERABLE_DEVICE_TYPE    Type;
} VIRT_PLATFORM_DEVICE;

STATIC CONST VIRT_PLATFORM_DEVICE mDevices[] = {
  {
    "generic-xhci",
    NonDiscoverableDeviceTypeXhci
  },
  {
    "generic-ehci",
    NonDiscoverableDeviceTypeEhci
  },
  {
    "sdhci",
    NonDiscoverableDeviceTypeSdhci
  }
};

EFI_STATUS
EFIAPI
RegisterMmioRegion (
  IN UINT64  Base,
  IN UINT64  Size
  )
{
  EFI_STATUS Status;

  Status = gDS->AddMemorySpace (
                  EfiGcdMemoryTypeMemoryMappedIo,
                  Base,
                  Size,
                  EFI_MEMORY_UC
                  );

  if (EFI_ERROR (Status) &&
      (Status != EFI_ALREADY_STARTED)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: AddMemorySpace failed: Base=%llx Size=%llx %r\n",
      __FUNCTION__,
      Base,
      Size,
      Status
      ));

    return Status;
  }

  Status = gDS->SetMemorySpaceAttributes (
                  Base,
                  Size,
                  EFI_MEMORY_UC
                  );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: SetMemorySpaceAttributes failed: Base=%llx Size=%llx %r\n",
      __FUNCTION__,
      Base,
      Size,
      Status
      ));
  }

  return Status;
}

EFI_STATUS
EFIAPI
RegisterFdtDevice (
  IN FDT_CLIENT_PROTOCOL          *FdtClient,
  IN CONST VIRT_PLATFORM_DEVICE  *Device
  )
{
  EFI_STATUS    Status;
  CONST UINT64  *Reg;
  UINT32        RegSize;
  UINTN         AddressCells;
  UINTN         SizeCells;
  UINT64         Base;
  UINT64         Size;

  Status = FdtClient->FindCompatibleNodeReg (
                        FdtClient,
                        Device->Compatible,
                        (CONST VOID **)&Reg,
                        &AddressCells,
                        &SizeCells,
                        &RegSize
                        );

  if (Status == EFI_NOT_FOUND) {
    DEBUG ((
      DEBUG_INFO,
      "%a: %a not found\n",
      __FUNCTION__,
      Device->Compatible
      ));

    return EFI_SUCCESS;
  }

  if (EFI_ERROR (Status)) {
    return Status;
  }

  ASSERT (AddressCells == 2);
  ASSERT (SizeCells == 2);
  ASSERT (RegSize == sizeof (UINT64) * 2);

  Base = SwapBytes64 (ReadUnaligned64 ((VOID *)&Reg[0]));
  Size = SwapBytes64 (ReadUnaligned64 ((VOID *)&Reg[1]));

  DEBUG ((
    DEBUG_INFO,
    "%a: %a @ %llx size %llx\n",
    __FUNCTION__,
    Device->Compatible,
    Base,
    Size
    ));

  Status = RegisterMmioRegion (
             Base,
             Size
             );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = RegisterNonDiscoverableMmioDevice (
             Device->Type,
             NonDiscoverableDeviceDmaTypeCoherent,
             NULL,
             NULL,
             1,
             (UINTN)Base,
             (UINTN)Size
             );

  if (EFI_ERROR (Status)) {
    DEBUG ((
      DEBUG_ERROR,
      "%a: Register %a failed: %r\n",
      __FUNCTION__,
      Device->Compatible,
      Status
      ));
  }

  return Status;
}

EFI_STATUS
EFIAPI
RegisterPlatformDevices (
  IN FDT_CLIENT_PROTOCOL  *FdtClient
  )
{
  EFI_STATUS Status;
  UINTN      Index;

  for (Index = 0; Index < ARRAY_SIZE (mDevices); Index++) {

    Status = RegisterFdtDevice (
               FdtClient,
               &mDevices[Index]
               );

    if (EFI_ERROR (Status)) {
      return Status;
    }
  }

  return EFI_SUCCESS;
}

EFI_STATUS
EFIAPI
InitializeVirtPlatformDxe (
  IN EFI_HANDLE        ImageHandle,
  IN EFI_SYSTEM_TABLE  *SystemTable
  )
{
  EFI_STATUS            Status;
  FDT_CLIENT_PROTOCOL  *FdtClient;

  DEBUG ((
    DEBUG_INFO,
    "%a: InitializeVirtPlatformDxe called\n",
    __FUNCTION__
    ));

  Status = gBS->LocateProtocol (
                  &gFdtClientProtocolGuid,
                  NULL,
                  (VOID **)&FdtClient
                  );

  if (EFI_ERROR (Status)) {
    return Status;
  }

  Status = RegisterPlatformDevices (
             FdtClient
             );

  return Status;
}