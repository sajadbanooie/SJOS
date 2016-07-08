#ifndef _KERNEL_ATA_PIO_H
#define _KERNEL_ATA_PIO_H

#include <stddef.h>

struct ata_device{
	uint8_t type;
	uint32_t sector_size;
	char serialnumber[10];
	char frim_rev[4];
	char model[20];
	// uint32_t size;
	uint8_t bus;
	uint8_t drive;
	uint32_t sector_count;
	uint8_t status;
};
struct ata_device ata_ide_devices[4];
uint8_t number_of_devices;
void ata_init(void);

#endif
