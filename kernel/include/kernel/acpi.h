//
// Created by simulation on 8/3/16.
//

#ifndef _KERNEL_ACPI_H
#define _KERNEL_ACPI_H

struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
} __attribute__ ((packed));

void init_acpi();
#endif
