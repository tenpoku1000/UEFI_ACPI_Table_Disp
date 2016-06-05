
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include <efi.h>
#include <efilib.h>
#include "tp_UDKenv.h"
#include "tp_acpi_table.h"
#include "tp_utils.h"

static void reset_system(EFI_STATUS status)
{
    EFI_STATUS local_status = EFI_SUCCESS;

    do{
        EFI_INPUT_KEY key;
        local_status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);
    } while (EFI_SUCCESS != local_status);

    RT->ResetSystem(EfiResetCold, status, 0, NULL);
}

static void error_print(CHAR16* msg)
{
    Print(msg);

    reset_system(EFI_SUCCESS);
}

EFI_STATUS efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    tp_set_uefi_pointers(ImageHandle, SystemTable);

    if (open_print_info(L"\\result.txt")){

        tp_print_acpi_table();

        close_print_info();

    }else{

        Print(L"open_print_info() faild.\n");
    }

    Print(L"\n");
    Print(L"When you press any key, the system will reboot.\n");

    reset_system(EFI_SUCCESS);

    return EFI_SUCCESS;
}

