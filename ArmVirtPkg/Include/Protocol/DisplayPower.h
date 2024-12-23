#ifndef __DISPLAY_POWER_H__
#define __DISPLAY_POWER_H__

#include <Uefi.h>

#define EFI_DISPLAY_POWER_PROTOCOL_GUID {        \
0xf352021d, 0x9593, 0x4432, {                    \
  0xbf, 0x4, 0x67, 0xb9, 0xf3, 0xb7, 0x60, 0x8   \
  }                                              \
}

extern EFI_GUID gEfiDisplayPowerProtocolGuid;

typedef struct _EFI_DISPLAY_POWER_PROTOCOL EFI_DISPLAY_POWER_PROTOCOL;

typedef enum _EFI_DISPLAY_POWER_STATE {
  EfiDisplayPowerStateUnknown = 0,  
  EfiDisplayPowerStateOff,
  EfiDisplayPowerStateMaximum,
} EFI_DISPLAY_POWER_STATE;

typedef EFI_STATUS (EFIAPI * EFI_DISPLAY_POWER_SETDISPLAYPOWERSTATE) (
  IN EFI_DISPLAY_POWER_PROTOCOL *This,
  IN EFI_DISPLAY_POWER_STATE PowerState
  );

typedef EFI_STATUS (EFIAPI * EFI_DISPLAY_POWER_GETDISPLAYPOWERSTATE) (
  IN EFI_DISPLAY_POWER_PROTOCOL *This,
  OUT EFI_DISPLAY_POWER_STATE *PowerState
  );

struct _EFI_DISPLAY_POWER_PROTOCOL {
  UINT32                                    Revision;
  EFI_DISPLAY_POWER_SETDISPLAYPOWERSTATE    SetDisplayPowerState;
  EFI_DISPLAY_POWER_GETDISPLAYPOWERSTATE    GetDisplayPowerState;
};

#endif