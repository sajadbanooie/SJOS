#include <stdint.h>
#include <kernel/portio.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/tty.h>
#include <kernel/ata_pio.h>
#include <string.h>
#include <stdio.h>

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
#define ATA_ERR 2
#define ATA_OK 0

#define ATA 0
#define ATAPI 1
#define SATA 2

int ata_wait = 0;
void ata_irq(struct regs *r){
	ata_wait = 0;
}
void ata_grab(){
	ata_wait = 1;
}
void ata_release(){
	ata_wait = 0;
}
int ata_schedule(uint16_t bus){
	int res;
	while (ata_wait == 1){
		res = read_port(ATA_CMD + bus);
		if (res & 2 == 1){
			return 1;
		}
	}
	ata_wait = 1;
	return 0;
}

uint32_t atapi_get_sector_count(uint16_t bus,uint8_t drive);
uint32_t atapi_get_sector_size(uint16_t bus,uint8_t drive);

void ata_select_delay(uint16_t bus);

int atapi_write(struct ata_device *pDevice, uint16_t *pInt, int lba, int count);

void ata_IDENTIFY(struct ata_device *atad){
    union identify{
        uint16_t identify_w[256];
        uint8_t identify_b[512];
    } identify;
	uint16_t bus = atad->bus;
	uint8_t drive = atad->drive;
	ata_grab();
	write_port(ATA_INFO + bus, 0);
    write_port(ATA_DRIVE + bus, (uint8_t) (0xA0 + drive));
	ata_select_delay(bus);
    write_port(ATA_SECTOR_COUNT + bus, 0x00);
    write_port(ATA_LBA_HI + bus, 0x00);
    write_port(ATA_LBA_MID + bus, 0x00);
    write_port(ATA_LBA_LOW + bus, 0x00);
    write_port(ATA_CMD + bus, 0xEC);
    int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		atad->type = ATAPI;
	}
    if (res == 0){
		ata_release();
        atad->status = ATA_DRIVE_NOT_EXIST;
		return ;
	}
	// res = read_port(ATA_CMD + bus);
	// while (res & (1 << 7) >> 7 == 1)
	// 	res = read_port(ATA_CMD + bus);
	// while (res & (1 << 3) >> 3 == 0)
	// 	res = read_port(ATA_CMD + bus);
	if (ata_schedule((uint8_t) bus) == 1) {

		atad->status = ATA_ERR;
		ata_release();
		return;
	}
    res = read_port(ATA_CMD + bus);
	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
    if ((lba_hi ==0) && (lba_mid ==0))
		atad->type = ATA;
	else if ((lba_hi == 0xEB) && (lba_mid == 0x14)){
		atad->type = ATAPI;
		write_port(ATA_CMD + bus, 0xA1);
	}
	else if ((lba_hi ==0xc3 ) && (lba_mid == 0x3c ))
		atad->type = SATA;
	else{
		atad->status = ATA_DEVICE_IS_NOT_ATA;
		ata_release();
        return ;
	}
    res = read_port(ATA_CMD + bus);
    while (res & (1 << 3) == 0)
        res = read_port(ATA_CMD + bus);
    res = read_port(ATA_CMD + bus);
    if (res & 2 == 1){

				atad->status = ATA_ERR;
				ata_release();
				return;
	
	}
	
    for(int i=0;i<256;i++){
        identify.identify_w[i] = read_port_w(ATA_DATA + bus);
    }
	memset(atad->serialnumber, 0, 21);
	for (int i = 10 * 2;i < 19 * 2 +1;i++){
		atad->serialnumber[i - 20] = (char) identify.identify_b[i];
	}
	memset(atad->frim_rev, 0, 11);

	for (int i = 23*2;i < 26 * 2 +1;i++){
        atad->frim_rev[i - 46] = (char) identify.identify_b[i];
    }
	memset(atad->model, 0, 41);

	for (int i = 27 * 2;i < 46 * 2 + 1;i++){
        atad->model[i - 27 * 2] = (char) identify.identify_b[i];
    }

	atad->bus = bus;
	atad->drive = drive;
	if (atad->type != ATAPI){
	atad->sector_count = (identify.identify_w[61] << 16) | (identify.identify_w[60]);
	atad->sector_size = 512;

	atad->status = ATA_OK;
	// while ((res = read_port(ATA_CMD + bus)) & 0x88);
	ata_release();
    return ;
	}
	atad->sector_count = atapi_get_sector_count(bus,drive);
	atad->sector_size = atapi_get_sector_size(bus,drive);
	atad->status = ATA_OK;
	// while ((res = read_port(ATA_CMD + bus)) & 0x88);
	ata_release();
}

void ata_select_delay(uint16_t bus){
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
}

uint32_t atapi_get_sector_size(uint16_t bus, uint8_t drive){
	ata_grab();
	uint8_t atapi_packet[12] = {0x25,0,0,0,0,0,0,0,0,0,0,0};
	write_port(ATA_DRIVE + bus, (uint8_t) (0xA0 + drive));
	ata_select_delay(bus);
	write_port(ATA_INFO + bus,0);
	write_port(ATA_LBA_MID + bus, 8);
	write_port(ATA_LBA_HI + bus, 0);
	write_port(ATA_CMD + bus, 0xA0);
	
	int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		ata_release();
		return (uint32_t) -1;
	}
	for (int i=0;i<6;i++){
		write_port_w(ATA_DATA + bus,atapi_packet[i] | (atapi_packet[i+1] << 8));
	}
	// if (ata_schedule(bus) == 1) {
	// 	ata_release();
	// 	return 0;
	// }

	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	uint16_t ret[4];
	ata_select_delay(bus);
	for (int i = 0;i < ((lba_hi << 8) | lba_mid) / 2;i++){
		ret[i] = read_port_w(ATA_DATA + bus);
	}
	// while ((res = read_port(ATA_CMD + bus)) & 0x88);
	ata_release();
	return (uint32_t) (((ret[3] | (ret[2] << 16))) * 512);
	
}

uint32_t atapi_get_sector_count(uint16_t bus,uint8_t drive){
	ata_grab();
	uint8_t atapi_packet[12] = {0x25,0,0,0,0,0,0,0,0,0,0,0};
	write_port(ATA_DRIVE + bus, (uint8_t) (0xA0 + drive));
	ata_select_delay(bus);
	write_port(ATA_INFO + bus,0);
	write_port(ATA_LBA_MID + bus, 8);
	write_port(ATA_LBA_HI + bus, 0);
	write_port(ATA_CMD + bus, 0xA0);
	
	int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		ata_release();
		return (uint32_t) -1;
	}

	for (int i=0;i<6;i++){
		write_port_w(ATA_DATA + bus,atapi_packet[i] | (atapi_packet[i+1] << 8));
	}

	// if (ata_schedule(bus) == 1) {
	// 	ata_release();
	// 	return 0;
	// }

	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	uint16_t ret[4];
	for (int i = 0;i < ((lba_hi << 8) | lba_mid) / 2;i++){
		ret[i] = read_port_w(ATA_DATA + bus);
	}
	// while ((res = read_port(ATA_CMD + bus)) & 0x88);
	ata_release();
	return (uint32_t) ((ret[1] | (ret[0] << 16)) + 1);
	
}

uint8_t atapi_read(struct ata_device *atad, uint16_t *buffer, int LBA, int count) {
	ata_grab();
	uint16_t bus = atad->bus;
	uint8_t drive = atad->drive;
	uint32_t sector_size = atad->sector_size;
	uint8_t atapi_packet[12] = {0xA8,0,0,0,0,0,0,0,0,0,0,0};
	write_port(ATA_DRIVE + bus, (uint8_t) (0xA0 + drive));
	ata_select_delay(bus);
	write_port(ATA_INFO + bus,0);

	write_port(ATA_LBA_MID + bus, (uint8_t) (sector_size * count & 0xff));
	write_port(ATA_LBA_HI + bus, (uint8_t) (sector_size * count >> 8 & 0xff));
	write_port(ATA_CMD + bus, 0xA0);
	int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		ata_release();
		return (uint8_t) -1;
	}
	res = read_port(ATA_CMD + bus);
	while (res & (1 << 7) >> 7 == 1)
		res = read_port(ATA_CMD + bus);
	while (res & (1 << 3) >> 3 == 0)
		res = read_port(ATA_CMD + bus);

//	if (ata_schedule(bus) == 1) {
//		return (uint8_t) -1;
//	}
//
	atapi_packet[9] = (uint8_t) count;

	for (int i=0;i<6;i++){
		write_port_w(ATA_DATA + bus,atapi_packet[i] | (atapi_packet[i+1] << 8));
	}

	if (ata_schedule(bus) == 1) {
		ata_release();
		return (uint8_t) -1;
	}

	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	ata_select_delay(bus);

	for (int i = 0;i < ((lba_hi << 8) | lba_mid) / 2;i++){

		buffer[i] = read_port_w(ATA_DATA + bus);
	}
	// while ((res = read_port(ATA_CMD + bus)) & 0x88);
	ata_release();
	return 1;
}

uint8_t ata_read(struct ata_device *atad, uint16_t *buffer, int LBA, int count){
    if (atad->type == ATAPI)
        return atapi_read(atad, buffer, LBA, count);
	ata_grab();
    uint16_t bus = atad->bus;
    uint8_t drive = atad->drive;
    write_port(ATA_DRIVE + bus, (uint8_t) ((0xE0 + drive) | (LBA >> 24)));
	ata_select_delay(bus);
    write_port(ATA_LBA_LOW + bus,(uint8_t) LBA);
    write_port(ATA_LBA_MID + bus, (uint8_t) (LBA >> 8));
    write_port(ATA_LBA_HI + bus, (uint8_t) (LBA >> 16));
    write_port(ATA_SECTOR_COUNT + bus, (uint8_t) count);
    write_port(ATA_CMD + bus, 0x20);
    int res;
    ata_select_delay(bus);
    res = read_port(ATA_CMD + bus);
    if (res & 2 == 1){
        ata_release();
        return (uint8_t) -1;
    }
    res = read_port(ATA_CMD + bus);
    while (res & (1 << 7) >> 7 == 1)
        res = read_port(ATA_CMD + bus);
    while (res & (1 << 3) >> 3 == 0)
        res = read_port(ATA_CMD + bus);
//    if (ata_schedule(bus) == 1) {
//
//		return ATA_ERR;
//	}
    for (int j=0;j<count;j++){
        for (int k=0;k<256;k++){
            buffer[j*256 + k] = read_port_w(ATA_DATA + bus);
        }
        ata_select_delay(bus);
    }
    if (ata_schedule(bus) == 1) {
//
		return ATA_ERR;
	}
    // cache flush
    write_port(ATA_CMD + bus, 0x37);
    ata_release();
    return 0;
}

int ata_write(struct ata_device *atad,uint16_t *buffer,int LBA,int count){
	if (atad->type == ATAPI)
		return atapi_write(atad, buffer, LBA, count);
	ata_grab();
	uint16_t bus = atad->bus;
	uint8_t drive = atad->drive;
	write_port(ATA_DRIVE + bus, (uint8_t) ((0xE0 + drive) | (LBA >> 24)));
	ata_select_delay(bus);
	write_port(ATA_LBA_LOW + bus,(uint8_t) LBA);
	write_port(ATA_LBA_MID + bus, (uint8_t) (LBA >> 8));
	write_port(ATA_LBA_HI + bus, (uint8_t) (LBA >> 16));
	write_port(ATA_SECTOR_COUNT + bus, (uint8_t) count);
	write_port(ATA_CMD + bus, 0x30);
	int res;
	ata_select_delay(bus);
	res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		ata_release();
		return (uint8_t) -1;
	}
	res = read_port(ATA_CMD + bus);
	while (res & (1 << 7) >> 7 == 1)
		res = read_port(ATA_CMD + bus);
	while (res & (1 << 3) >> 3 == 0)
		res = read_port(ATA_CMD + bus);
//    if (ata_schedule(bus) == 1) {
//
//		return ATA_ERR;
//	}
	for (int j=0;j<count;j++){
		for (int k=0;k<256;k++){
			write_port_w(ATA_DATA + bus, buffer[j*256 + k]);
		}
		ata_select_delay(bus);
	}
    if (ata_schedule(bus) == 1) {
//
        return ATA_ERR;
    }
    // cache flush
    write_port(ATA_CMD + bus, 0x37);
    ata_release();
	return 0;
}

int atapi_write(struct ata_device *pDevice, uint16_t *pInt, int lba, int count) {
    return 0;
}

void ata_init(void){
	irq_install_handler(14,ata_irq);
	irq_install_handler(15,ata_irq);
	uint16_t a[2048];
	uint16_t b[2048];
    ata_ide_devices[0].bus = ATA_PRI;
    ata_ide_devices[0].drive = ATA_MASTER;
    ata_ide_devices[1].bus = ATA_PRI;
    ata_ide_devices[1].drive = ATA_SLAVE;
    ata_ide_devices[2].bus = ATA_SEC;
    ata_ide_devices[2].drive = ATA_MASTER;
    ata_ide_devices[3].bus = ATA_SEC;
    ata_ide_devices[3].drive = ATA_SLAVE;
	ata_IDENTIFY(&ata_ide_devices[0]);
	if (ata_ide_devices[0].status == ATA_OK){
        printk("found ata device: ata0-master with %d bytes\n",
               ata_ide_devices[0].sector_count * ata_ide_devices[0].sector_size);
        printk("Serial Number: ");
        printk(ata_ide_devices[0].serialnumber);
        printk("\n");
        printk("Model: ");
        printk(ata_ide_devices[0].model);
        printk("\n");
        printk("Firmware revision: ");
        printk(ata_ide_devices[0].frim_rev);
        printk("\n");
	}
	ata_IDENTIFY(&ata_ide_devices[1]);
	if (ata_ide_devices[1].status == ATA_OK){
        printk("found ata device: ata0-slave with %d bytes\n",
               ata_ide_devices[1].sector_count * ata_ide_devices[1].sector_size);
        printk("Serial Number: ");
        printk(ata_ide_devices[1].serialnumber);
        printk("\n");
        printk("Model: ");
        printk(ata_ide_devices[1].model);
        printk("\n");
        printk("Firmware revision: ");
        printk(ata_ide_devices[1].frim_rev);
        printk("\n");
	}
	ata_IDENTIFY(&ata_ide_devices[2]);
	if (ata_ide_devices[2].status == ATA_OK) {
        printk("found ata device: ata1-master with %d bytes\n",
               ata_ide_devices[2].sector_count * ata_ide_devices[2].sector_size);

        printk("Serial Number: ");
        printk(ata_ide_devices[2].serialnumber);
        printk("\n");
        printk("Model: ");
        printk(ata_ide_devices[2].model);
        printk("\n");
        printk("Firmware revision: ");
        printk(ata_ide_devices[2].frim_rev);
        printk("\n");
    }
	ata_IDENTIFY(&ata_ide_devices[3]);
	if (ata_ide_devices[3].status == ATA_OK) {
        printk("found ata device: ata1-slave with %d bytes\n",
               ata_ide_devices[3].sector_count * ata_ide_devices[3].sector_size);
        printk("Serial Number: ");
        printk(ata_ide_devices[3].serialnumber);
        printk("\n");
        printk("Model: ");
        printk(ata_ide_devices[3].model);
        printk("\n");
        printk("Firmware revision: ");
        printk(ata_ide_devices[3].frim_rev);
        printk("\n");
    }
    ata_read(&ata_ide_devices[1], a,0,1);
    a[255] = 0x55AA;
    ata_write(&ata_ide_devices[1], a, 0, 1);
    ata_read(&ata_ide_devices[1], b,0,1);

    printk("%X ",b[255]);

}
