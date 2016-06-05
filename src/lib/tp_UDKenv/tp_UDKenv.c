
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include "UDK.h"
#include "tp_UDKenv.h"

void tp_set_uefi_pointers(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    (void)UefiBootServicesTableLibConstructor(image_handle, system_table);
    (void)UefiRuntimeServicesTableLibConstructor(image_handle, system_table);
}

