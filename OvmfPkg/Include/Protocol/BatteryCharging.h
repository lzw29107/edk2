#ifndef __BATTERY_CHARGING_H__
#define __BATTERY_CHARGING_H__

#include <Uefi.h>

#define EFI_BATTERY_CHARGING_PROTOCOL_GUID {       \
  0x840cb643, 0x8198, 0x428a, {                    \
    0xa8, 0xb3, 0xa0, 0x72, 0xce, 0x57, 0xcd, 0xb9 \
    }                                              \
  }

extern EFI_GUID gEfiBatteryChargingProtocolGuid;

typedef struct _EFI_BATTERY_CHARGING_PROTOCOL EFI_BATTERY_CHARGING_PROTOCOL;

typedef enum _EFI_BATTERY_CHARGING_STATUS {
  EfiBatteryChargingStatusNone = 0,
  EfiBatteryChargingStatusSuccess,
  EfiBatteryChargingStatusOverheat,
  EfiBatteryChargingStatusVoltageOutOfRange,
  EfiBatteryChargingStatusCurrentOutOfRange,
  EfiBatteryChargingStatusTimeout,
  EfiBatteryChargingStatusAborted,
  EfiBatteryChargingStatusDeviceError,
  EfiBatteryChargingStatusExtremeCold,
  EfiBatteryChargingStatusBatteryChargingNotSupported,
  EfiBatteryChargingStatusBatteryNotDetected,
  EfiBatteryChargingSourceNotDetected,
  EfiBatteryChargingSourceVoltageInvalid,
  EfiBatteryChargingSourceCurrentInvalid,
  EfiBatteryChargingErrorRequestShutdown,
  EfiBatteryChargingErrorRequestReboot
} EFI_BATTERY_CHARGING_STATUS;

typedef struct _EFI_BATTERY_CHARGING_COMPLETION_TOKEN {
  EFI_EVENT Event;
  EFI_BATTERY_CHARGING_STATUS Status;
} EFI_BATTERY_CHARGING_COMPLETION_TOKEN;

typedef EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_GET_BATTERY_STATUS) (
  IN EFI_BATTERY_CHARGING_PROTOCOL *This,
  OUT UINT32 *StateOfCharge,
  OUT UINT32 *RatedCapacity,
  OUT INT32 *ChargeCurrent );

typedef EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_CHARGE_BATTERY) (
  IN EFI_BATTERY_CHARGING_PROTOCOL *This,
  IN UINT32 MaximumCurrent,
  IN UINT32 TargetStateOfCharge,
  IN EFI_BATTERY_CHARGING_COMPLETION_TOKEN *CompletionToken );

typedef EFI_STATUS (EFIAPI * EFI_BATTERY_CHARGING_GET_BATTERY_INFORMATION) (
  IN EFI_BATTERY_CHARGING_PROTOCOL *This,
  OUT UINT32 *StateOfCharge,
  OUT INT32 *CurrentIntoBattery,
  OUT UINT32 *BatteryTerminalVoltage,
  OUT INT32 *BatteryTemperature,
  OUT UINT32 *USBCableVoltage,
  OUT UINT32 *USBCableCurrent );

struct _EFI_BATTERY_CHARGING_PROTOCOL {
  EFI_BATTERY_CHARGING_GET_BATTERY_STATUS         GetBatteryStatus;
  EFI_BATTERY_CHARGING_CHARGE_BATTERY             ChargeBattery;
  UINT32                                          Revision;
  EFI_BATTERY_CHARGING_GET_BATTERY_INFORMATION    GetBatteryInformation;
};

#endif