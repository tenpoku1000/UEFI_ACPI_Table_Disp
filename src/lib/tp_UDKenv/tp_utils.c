
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include "UDK.h"
#include "tp_utils.h"

static EFI_FILE* efi_file = NULL;

static void convert_to_ascii(char* ascii, CHAR16* wide)
{
    UINTN size = StrLen(wide);

    for (UINTN i = 0; size > i; ++i) {

        ascii[i] = (char)(wide[i] & 0xff);
    }

    ascii[size] = '\0';
}

void tp_put_log(CHAR16* fmt, ...)
{
    static CHAR16 buffer[256];

    VA_LIST arg;

    VA_START(arg, fmt);

    SetMem(buffer, sizeof(buffer), 0);

    UnicodeVSPrint(buffer, sizeof(buffer), fmt, arg);

    gST->ConOut->OutputString(gST->ConOut, buffer);

    gST->ConOut->SetCursorPosition(gST->ConOut, 0, gST->ConOut->Mode->CursorRow);

    if (efi_file){

        static char buffer_a[256];

        SetMem(buffer_a, sizeof(buffer_a), 0);

        convert_to_ascii(buffer_a, buffer);

        UINTN size = AsciiStrLen(buffer_a);

        EFI_STATUS status = efi_file->Write(efi_file, &size, buffer_a);

        if (EFI_ERROR(status)){

            tp_error_print(L"Write() failed.\n");
        }
    }

    VA_END(arg);
}

void tp_error_print(CHAR16* fmt, ...)
{
    static CHAR16 buffer[256];

    VA_LIST arg;

    VA_START(arg, fmt);

    SetMem(buffer, sizeof(buffer), 0);

    UnicodeVSPrint(buffer, sizeof(buffer), fmt, arg);

    gST->ConOut->OutputString(gST->ConOut, buffer);

    gST->ConOut->SetCursorPosition(gST->ConOut, 0, gST->ConOut->Mode->CursorRow);

    VA_END(arg);
}

EFI_FILE* open_print_info(CHAR16* path, EFI_HANDLE device_handle)
{
    EFI_FILE_IO_INTERFACE* efi_simple_file_system = NULL;
    EFI_FILE* efi_file_root = NULL;

    EFI_STATUS status = gBS->OpenProtocol(
        device_handle,
        &gEfiSimpleFileSystemProtocolGuid,
        &efi_simple_file_system,
        gImageHandle,
        NULL,
        EFI_OPEN_PROTOCOL_GET_PROTOCOL
    );

    if (EFI_ERROR(status)){

        tp_error_print(L"OpenProtocol() gEfiSimpleFileSystemProtocolGuid failed.\n");

        return NULL;
    }

    status = efi_simple_file_system->OpenVolume(
        efi_simple_file_system, &efi_file_root
    );

    if (EFI_ERROR(status)){

        tp_error_print(L"OpenVolume() failed.\n");

        return NULL;
    }

    status = efi_file_root->Open(
        efi_file_root, &efi_file, path,
        EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE |
        EFI_FILE_MODE_CREATE, EFI_FILE_ARCHIVE
    );

    if (EFI_ERROR(status)){

        tp_error_print(L"Open() failed.\n");

        return NULL;
    }

    status = efi_file_root->Close(efi_file_root);

    if (EFI_ERROR(status)){

        tp_error_print(L"Close() failed.\n");

        return NULL;
    }

    return efi_file;
}

void close_print_info(void)
{
    if (efi_file){

        EFI_STATUS status = efi_file->Close(efi_file);

        if (EFI_ERROR(status)){

            tp_error_print(L"Close() failed.\n");
        }
    }
}

