#ifndef _KERNEL_ATA_PIO_H
#define _KERNEL_ATA_PIO_H

#include <stddef.h>

#define ATA_PRI 0x1F0
#define ATA_SEC 0x170

#define ATA_DATA 0
#define ATA_INFO 1
#define ATA_SECTOR_COUNT 2
#define ATA_LBA_LOW 3
#define ATA_LBA_MID 4
#define ATA_LBA_HI 5
#define ATA_DRIVE 6
#define ATA_CMD 7 // command and status
#define ATA_CTRL 0x206

#define ATA_MASTER 0
#define ATA_SLAVE 0x10

#define ATA_DRIVE_NOT_EXIST 3
#define ATA_DEVICE_IS_NOT_ATA 1

#define ATA_ER_BBK      0x80    // Bad sector
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // No media
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // No media
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark
#define ATA_ER_DF     0x03    // Drive Fault
#define ATA_ER_RN     0x05    // Reads Nothing
#define ATA_ER_WP     0x06    // Write Protected
#define ATA_OK 0

#define ATA 0
#define ATAPI 1
#define SATA 2

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
uint8_t ata_read(struct ata_device *atad,uint16_t *buffer,int LBA,int count);

#endif
