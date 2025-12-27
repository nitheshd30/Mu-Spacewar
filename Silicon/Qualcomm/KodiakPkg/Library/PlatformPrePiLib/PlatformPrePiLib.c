#include <Library/IoLib.h>
#include <Library/PlatformPrePiLib.h>
#include <Library/PcdLib.h>
#include <Library/ConfigurationMapHelperLib.h>
#include <Library/HobLib.h>  // <--- ADDED: Required for BuildResourceDescriptorHob

#include "PlatformRegisters.h"

VOID
PlatformInitialize ()
{
  EFI_STATUS Status;
  UINT32     EarlyInitCoreCnt;

  // ------------------------------------------------------------------------
  // FIX: Add missing Memory Region 0x10000000 for Windows Boot Manager
  // This creates a HOB that tells the OS and DXE Core that this memory exists
  // and is reserved for MMIO, preventing the ConvertPages crash.
  // ------------------------------------------------------------------------
  BuildResourceDescriptorHob (
    EFI_RESOURCE_MEMORY_MAPPED_IO,
    (
      EFI_RESOURCE_ATTRIBUTE_PRESENT |
      EFI_RESOURCE_ATTRIBUTE_INITIALIZED |
      EFI_RESOURCE_ATTRIBUTE_UNCACHEABLE |
      EFI_RESOURCE_ATTRIBUTE_TESTED
    ),
    0x10000000,  // Start Address (matches your error)
    0x00200000   // Length (2MB - covers the 101EDFFF range)
  );
  // ------------------------------------------------------------------------

  // Get Early Cores Count
  Status = LocateConfigurationMapUINT32ByName("EarlyInitCoreCnt", &EarlyInitCoreCnt);
  if (!EFI_ERROR (Status)) {
    // Wake Up all Cores
    for (UINTN i = 0; i < EarlyInitCoreCnt; i++) {
      MmioWrite32 (GICR_WAKER_CPU(i), (MmioRead32 (GICR_WAKER_CPU(i)) & ~GIC_WAKER_PROCESSORSLEEP));
    }
  }
}
