#ifndef VIRTPLATFORM_DXE_H
#define VIRTPLATFORM_DXE_H

#include <Uefi.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>
#include <Library/DebugLib.h>

const UINTN DEVICE_UNIQUE_ID_VERSION = 1;

typedef struct _DEVICE_UNIQUE_ID_COPY
{
    UINT8 DeviceIdCopy[8];
} DEVICE_UNIQUE_ID_COPY;

struct _DEVICE_UNIQUE_ID
{
    UINT8 DeviceId[32];
} DEVICE_UNIQUE_ID;

EFI_STATUS
EFIAPI
SetDeviceUniqueIdUefiVariable (
    VOID
);

#endif // VIRTPLATFORM_DXE_H