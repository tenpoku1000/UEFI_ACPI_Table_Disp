
// Copyright 2016 Shin'ichi Ichikawa. Released under the MIT license.

#include "UDK.h"
#include <Base.h>
#include <Guid/Acpi.h>
#include <Guid/SmBios.h>
#include <IndustryStandard/Acpi.h>
#include <IndustryStandard/HighPrecisionEventTimerTable.h>
#include <IndustryStandard/SmBios.h>
#include "tp_acpi_table.h"
#include "tp_utils.h"

static EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER* acpi_rsdp = NULL;
static EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE* acpi_fadt = NULL;
static EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER* acpi_madt = NULL;
static EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER* acpi_hpet = NULL;

static SMBIOS_TABLE_3_0_ENTRY_POINT* smbios_ep = NULL;

typedef struct MADT_ST_HEADER_{
    UINT8 type;
    UINT8 length;
}MADT_ST_HEADER;

// ACPI Table
static void tp_print_acpi(UINT8* table);
// Fixed ACPI Description Table Structure(FADT)
static void tp_print_acpi_fadt(EFI_ACPI_DESCRIPTION_HEADER* header);
// Multiple APIC Description Table(MADT)
static void tp_print_acpi_madt(EFI_ACPI_DESCRIPTION_HEADER* header);
// [MADT] Processor Local APIC Structure
static void tp_print_acpi_madt_local_apic(MADT_ST_HEADER* acpi_madt_address);
// [MADT] IO APIC Structure
static void tp_print_acpi_madt_io_apic(MADT_ST_HEADER* acpi_madt_address);
// [MADT] Interrupt Source Override Structure
static void tp_print_acpi_madt_iso(MADT_ST_HEADER* acpi_madt_address);
// [MADT] Local APIC NMI Structure
static void tp_print_acpi_madt_nmi(MADT_ST_HEADER* acpi_madt_address);
// High Precision Event Timer Table
static void tp_print_acpi_hpet(EFI_ACPI_DESCRIPTION_HEADER* header);

// SMBIOS
static void tp_print_smbios(UINT8* table);

void tp_print_acpi_table(void)
{
    for (UINTN i = 0; gST->NumberOfTableEntries > i; ++i){

        EFI_GUID* guid = &(gST->ConfigurationTable[i].VendorGuid);
        UINT8* table = (UINT8*)(gST->ConfigurationTable[i].VendorTable);

        if (0 == CompareMem(guid, &gEfiAcpiTableGuid, sizeof(EFI_GUID))){

            // ACPI Table
            tp_print_acpi(table);

        }else if (0 == CompareMem(guid, &gEfiSmbiosTableGuid, sizeof(EFI_GUID))){

            // SMBIOS
            tp_print_smbios(table);
        }
    }
}

// ACPI Table
static void tp_print_acpi(UINT8* table)
{
    acpi_rsdp = (EFI_ACPI_6_0_ROOT_SYSTEM_DESCRIPTION_POINTER*)table;

    EFI_ACPI_DESCRIPTION_HEADER* acpi_header = (EFI_ACPI_DESCRIPTION_HEADER*)(acpi_rsdp->XsdtAddress);

    if (acpi_header){

        tp_put_log(L"Detect the EFI_ACPI_ROOT_SYSTEM_DESCRIPTION_POINTER.\n");

        UINTN size = (acpi_header->Length - sizeof(*acpi_header)) / sizeof(void*);

        ++acpi_header;

        EFI_ACPI_DESCRIPTION_HEADER** header = (EFI_ACPI_DESCRIPTION_HEADER**)acpi_header;

        for (UINTN j = 0; size > j; ++j){

            if (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE == header[j]->Signature){

                // Fixed ACPI Description Table Structure(FADT)
                tp_print_acpi_fadt(header[j]);

                continue;
            }

            if (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_SIGNATURE == header[j]->Signature){

                // Multiple APIC Description Table(MADT)
                tp_print_acpi_madt(header[j]);

                continue;
            }

            if (EFI_ACPI_6_0_HIGH_PRECISION_EVENT_TIMER_TABLE_SIGNATURE == header[j]->Signature){

                // High Precision Event Timer Table
                tp_print_acpi_hpet(header[j]);

                continue;
            }
        }
    }
}

// Fixed ACPI Description Table Structure(FADT)
static void tp_print_acpi_fadt(EFI_ACPI_DESCRIPTION_HEADER* header)
{
    acpi_fadt = (EFI_ACPI_6_0_FIXED_ACPI_DESCRIPTION_TABLE*)header;

    if (acpi_fadt){

        tp_put_log(L"Detect the EFI_ACPI_FIXED_ACPI_DESCRIPTION_TABLE.\n");

        tp_put_log(L"  FADT.SCI_INT = %d\n", acpi_fadt->SciInt);
    }
}

// Multiple APIC Description Table(MADT)
static void tp_print_acpi_madt(EFI_ACPI_DESCRIPTION_HEADER* header)
{
    acpi_madt = (EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER*)(header);

    if (acpi_madt){

        tp_put_log(L"Detect the EFI_ACPI_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER.\n");

        UINT64 tmp_local_apic_id = acpi_madt->LocalApicAddress + 0x20;

        UINT8* local_apic_id = (UINT8*)tmp_local_apic_id;

        tp_put_log(
            L"  MADT.LocalApicAddress = %x, local_apic_id = %d\n",
            acpi_madt->LocalApicAddress, *local_apic_id
        );

        tp_put_log(
            L"  MADT.Flags = %s\n",
            (EFI_ACPI_6_0_PCAT_COMPAT & acpi_madt->Flags) ? L"1: The 8259 vectors must be disabled." : L"0"
        );

        EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER* acpi_madt_temp = acpi_madt;

        ++acpi_madt_temp;

        MADT_ST_HEADER* acpi_madt_address = (MADT_ST_HEADER*)acpi_madt_temp;

        UINT32 body_length = acpi_madt->Header.Length - sizeof(EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER);

        UINT32 prev_length = 0;

        for (UINT32 current_length = 0;
            body_length >= current_length;
            current_length += prev_length){

            if (0 == acpi_madt_address->length){

                break;
            }

            switch (acpi_madt_address->type){
            case EFI_ACPI_6_0_PROCESSOR_LOCAL_APIC:
                // [MADT] Processor Local APIC Structure
                tp_print_acpi_madt_local_apic(acpi_madt_address);
                break;
            case EFI_ACPI_6_0_IO_APIC:
                // [MADT] IO APIC Structure
                tp_print_acpi_madt_io_apic(acpi_madt_address);
                break;
            case EFI_ACPI_6_0_INTERRUPT_SOURCE_OVERRIDE:
                // [MADT] Interrupt Source Override Structure
                tp_print_acpi_madt_iso(acpi_madt_address);
                break;
            case EFI_ACPI_6_0_LOCAL_APIC_NMI:
                // [MADT] Local APIC NMI Structure
                tp_print_acpi_madt_nmi(acpi_madt_address);
                break;
            default:
                break;
            }

            UINT8* p = (UINT8*)acpi_madt_address;

            p += acpi_madt_address->length;
            
            prev_length = acpi_madt_address->length;
            
            acpi_madt_address = (MADT_ST_HEADER*)p;
        }
    }
}

// [MADT] Processor Local APIC Structure
static void tp_print_acpi_madt_local_apic(MADT_ST_HEADER* acpi_madt_address)
{
    EFI_ACPI_6_0_PROCESSOR_LOCAL_APIC_STRUCTURE* p =
        (EFI_ACPI_6_0_PROCESSOR_LOCAL_APIC_STRUCTURE*)acpi_madt_address;

    if (p->Flags & EFI_ACPI_6_0_LOCAL_APIC_ENABLED){

        tp_put_log(
            L"  PROCESSOR_LOCAL_APIC:AcpiProcessorUid = %d, ApicId = %d\n",
            p->AcpiProcessorUid, p->ApicId
        );
    }
}

// [MADT] IO APIC Structure
static void tp_print_acpi_madt_io_apic(MADT_ST_HEADER* acpi_madt_address)
{
    EFI_ACPI_6_0_IO_APIC_STRUCTURE* p =
        (EFI_ACPI_6_0_IO_APIC_STRUCTURE*)acpi_madt_address;

    tp_put_log(
        L"  IO_APIC:IoApicId = %d, IoApicAddress = %x, GlobalSystemInterruptBase = %d\n",
        p->IoApicId, p->IoApicAddress, p->GlobalSystemInterruptBase
    );
}

// [MADT] Interrupt Source Override Structure
static void tp_print_acpi_madt_iso(MADT_ST_HEADER* acpi_madt_address)
{
    EFI_ACPI_6_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE* p =
        (EFI_ACPI_6_0_INTERRUPT_SOURCE_OVERRIDE_STRUCTURE*)acpi_madt_address;

    tp_put_log(
        L"  INTERRUPT_SOURCE_OVERRIDE:"
        L"  Bus = %d, Source = %d, GlobalSystemInterrupt = %d, "
        L"  Flags.POLARITY = %d, Flags.TRIGGER_MODE = %d\n",
        p->Bus, p->Source, p->GlobalSystemInterrupt,
        (p->Flags & EFI_ACPI_6_0_POLARITY), (p->Flags & EFI_ACPI_6_0_TRIGGER_MODE)
    );
}

// [MADT] Local APIC NMI Structure
static void tp_print_acpi_madt_nmi(MADT_ST_HEADER* acpi_madt_address)
{
    EFI_ACPI_6_0_LOCAL_APIC_NMI_STRUCTURE* p =
        (EFI_ACPI_6_0_LOCAL_APIC_NMI_STRUCTURE*)acpi_madt_address;

    tp_put_log(
        L"  LOCAL_APIC_NMI:AcpiProcessorUid = %x, "
        L"  Flags.POLARITY = %d, Flags.TRIGGER_MODE = %d, LocalApicLint = %d\n",
        p->AcpiProcessorUid,
        (p->Flags & EFI_ACPI_6_0_POLARITY), (p->Flags & EFI_ACPI_6_0_TRIGGER_MODE),
        p->LocalApicLint
    );
}

// High Precision Event Timer Table
static void tp_print_acpi_hpet(EFI_ACPI_DESCRIPTION_HEADER* header)
{
    acpi_hpet = (EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER*)header;

    if (acpi_hpet){

        tp_put_log(L"Detect the EFI_ACPI_HIGH_PRECISION_EVENT_TIMER_TABLE_HEADER.\n");

        CHAR16* page_protection = L"";

        switch (acpi_hpet->PageProtectionAndOemAttribute & 0x03){
        case EFI_ACPI_NO_PAGE_PROTECTION:
            page_protection = L"EFI_ACPI_NO_PAGE_PROTECTION";
            break;
        case EFI_ACPI_4KB_PAGE_PROTECTION:
            page_protection = L"EFI_ACPI_4KB_PAGE_PROTECTION";
            break;
        case EFI_ACPI_64KB_PAGE_PROTECTION:
            page_protection = L"EFI_ACPI_64KB_PAGE_PROTECTION";
            break;
        default:
            page_protection = L"EFI_ACPI_UNKNOWN_PROTECTION";
            break;
        }

        tp_put_log(
            L"  HPET:BaseAddressLower32Bit.Address = %x, HpetNumber = %d, \n",
            acpi_hpet->BaseAddressLower32Bit.Address, acpi_hpet->HpetNumber
        );

        tp_put_log(
            L"  MainCounterMinimumClockTickInPeriodicMode = %d, EventTimerBlockId = %x, \n",
            acpi_hpet->MainCounterMinimumClockTickInPeriodicMode,
            acpi_hpet->EventTimerBlockId
        );

        tp_put_log(L"  page_protection = %s\n", page_protection);
    }
}

// SMBIOS
static void tp_print_smbios(UINT8* table)
{
    smbios_ep = (SMBIOS_TABLE_3_0_ENTRY_POINT*)table;

    if (smbios_ep){

        tp_put_log(L"Detect the SMBIOS_TABLE_ENTRY_POINT.\n");
    }
}

