#ifndef _KERNEL_ATA_PIO_H
#define _KERNEL_ATA_PIO_H

#include <stddef.h>

struct ata_device{
	uint8_t type;
	uint32_t sector_size;
	char serialnumber[21];
	char frim_rev[11];
	char model[41];
	// uint32_t size;
	uint16_t bus;
	uint8_t drive;
	uint32_t sector_count;
	uint8_t status;
};
struct ata_device ata_ide_devices[4];
uint8_t number_of_devices;
void ata_init(void);

#endif
