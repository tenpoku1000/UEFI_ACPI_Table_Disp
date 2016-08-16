
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include <efi.h>
#include <efilib.h>
#include "tp_UDKenv.h"
#include "tp_acpi_table.h"
#include "tp_utils.h"
#include "efi_status.h"

static void init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table);
static void reset_system(EFI_STATUS status);
static void read_key(void);
static void error_print(CHAR16* msg, EFI_STATUS* status);

static EFI_LOADED_IMAGE* loaded_image = NULL;

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    init(image_handle, system_table);

    if (open_print_info(L"\\result.txt", loaded_image->DeviceHandle)){

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

static void init(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE* system_table)
{
    InitializeLib(image_handle, system_table);

    tp_set_uefi_pointers(image_handle, system_table);

    EFI_STATUS status = EFI_SUCCESS;

    if ((NULL == ST->ConIn) || (EFI_SUCCESS != (status = ST->ConIn->Reset(ST->ConIn, 0)))){

        error_print(L"Input device unavailable.\n", ST->ConIn ? &status : NULL);
    }

    status = BS->OpenProtocol(
        image_handle, &LoadedImageProtocol, &loaded_image,
        image_handle, NULL, EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    if (EFI_ERROR(status)){

        error_print(L"OpenProtocol() LoadedImageProtocol failed.\n", &status);
    }
}

static void reset_system(EFI_STATUS status)
{
    read_key();

    RT->ResetSystem(EfiResetCold, status, 0, NULL);
}

static void read_key(void)
{
    if (ST->ConIn){

        EFI_STATUS local_status = EFI_SUCCESS;

        do{
            EFI_INPUT_KEY key;

            local_status = ST->ConIn->ReadKeyStroke(ST->ConIn, &key);

        } while (EFI_SUCCESS != local_status);
    }
}

static void error_print(CHAR16* msg, EFI_STATUS* status)
{
    Print(msg);

    if (status){

        Print(L"EFI_STATUS = %d, %s\n", *status, print_status_msg(*status));
    }

    reset_system(EFI_SUCCESS);
}

