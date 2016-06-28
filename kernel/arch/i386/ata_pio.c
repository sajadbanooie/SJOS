#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/portio.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/ata_pio.h>
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

#define ATA_DRIVE_NOTEXIST 3
#define ATA_DEVICE_ISNOTATA 1
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
int ata_schedule(uint8_t bus){
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

uint32_t atapi_get_size(uint16_t bus,uint8_t drive);
void ata_IDENTIFY(uint16_t bus,uint8_t drive){
    uint16_t identify[256];
	struct ata_device *atad;
	if (bus == ATA_PRI){
		if (drive == ATA_MASTER)
			atad = &ata_devices[0];
		if (drive == ATA_SLAVE)
			atad = &ata_devices[1];
	}
	if (bus == ATA_SEC){
		if (drive == ATA_MASTER)
			atad = &ata_devices[2];
		if (drive == ATA_SLAVE)
			atad = &ata_devices[3];
	}
	ata_grab();
	write_port(ATA_INFO + bus, 0);
    write_port(ATA_DRIVE + bus, 0xA0 + drive);
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
        atad->status = ATA_DRIVE_NOTEXIST;
		return ;
	}
	if (ata_schedule(bus) == 1) {
		atad->status = ATA_ERR;
		return;
	}
	printf("salam");
    res = read_port(ATA_CMD + bus);
	printf("salam");
	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	printf("salam");
    if ((lba_hi ==0) && (lba_mid ==0))
		atad->type = ATA;
	else if ((lba_hi == 0xEB) && (lba_mid == 0x14))
		atad->type = ATAPI;
	else if ((lba_hi ==0xc3 ) && (lba_mid == 0x3c ))
		atad->type = SATA;
	else{
		printf("salam");
		atad->status = ATA_DEVICE_ISNOTATA;
        return ;
	}
	printf("salam");
    res = read_port(ATA_CMD + bus);
	printf("salam");
    while (res & (1 << 3) == 0)
        res = read_port(ATA_CMD + bus);
    res = read_port(ATA_CMD + bus);
	printf("salam");
    if (res & 2 == 1){

		if (atad->type == ATAPI){
		
			write_port(ATA_DRIVE + bus, 0xA0 + drive);
			write_port(ATA_SECTOR_COUNT + bus, 0x00);
			write_port(ATA_LBA_HI + bus, 0x00);
			write_port(ATA_LBA_MID + bus, 0x00);
			write_port(ATA_LBA_LOW + bus, 0x00);
			write_port(ATA_CMD + bus, 0xA1);
			res = read_port(ATA_CMD + bus);
		    while (res & (1 << 3) == 0)
		        res = read_port(ATA_CMD + bus);
			res = read_port(ATA_CMD + bus);
    		if (res & 2 == 1){
				atad->status = ATA_ERR;
        		return ;
			}
		}
		else {
				atad->status = ATA_ERR;
				return;
		}
			
	}
	
    for(int i=0;i<256;i++){
        identify[i] = read_port_w(ATA_DATA + bus);
    }
	if (atad->type != ATAPI){
	atad->size = (identify[61] << 16) | (identify[60]);
	atad->status = ATA_OK;
		
    return ;
	}
	printf("salam");
	atad->size = atapi_get_size(bus,drive);
	atad->status = ATA_OK;
	ata_release();
}
uint8_t atapi_read(uint16_t bus,uint8_t drive,uint16_t *buffer,int LBA,int count){
	ata_grab();
	uint8_t atapi_packet[12] = {0x08,0,0,0,1,0,0,0,0,0,0,0};
	write_port(ATA_DRIVE + bus, 0xA0 + drive);
	ata_select_delay(bus);
	write_port(ATA_INFO + bus,0);
	write_port(ATA_LBA_MID + bus, 0);
	write_port(ATA_LBA_HI + bus, 2);
	write_port(ATA_CMD + bus, 0xA0);
	int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		return -1;
	}
	// ata_schedule();
	printf("salam");
	for (int i=0;i<3;i++){
		write_port_w(ATA_DATA + bus,atapi_packet[i] | (atapi_packet[i+1] << 8));
	}
	ata_schedule(bus);
	printf("salam");
	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	printf("retured %d ",(lba_hi << 8) | lba_mid / 2);

	for (int i = 0;i < ((lba_hi << 8) | lba_mid) / 2;i++){
		printf("%X ", read_port_w(ATA_DATA + bus));
	}
	ata_release();
	return 1;
}
uint32_t atapi_get_size(uint16_t bus,uint8_t drive){
	ata_grab();
	uint8_t atapi_packet[12] = {0x25,0,0,0,0,0,0,0,0,0,0,0};
	write_port(ATA_DRIVE + bus, 0xA0 + drive);
	ata_select_delay(bus);
	write_port(ATA_INFO + bus,0);
	write_port(ATA_LBA_MID + bus, 8);
	write_port(ATA_LBA_HI + bus, 0);
	write_port(ATA_CMD + bus, 0xA0);
	
	int res = read_port(ATA_CMD + bus);
	if (res & 2 == 1){
		return -1;
	}

	printf("salam");
	for (int i=0;i<6;i++){
		write_port_w(ATA_DATA + bus,atapi_packet[i] | (atapi_packet[i+1] << 8));
	}
	if (ata_schedule(bus) == 1) {
		return 0;
	}

	printf("salam");
	int lba_hi = read_port(ATA_LBA_HI + bus);
	int lba_mid = read_port(ATA_LBA_MID + bus);
	uint16_t ret[4];
	for (int i = 0;i < ((lba_hi << 8) | lba_mid) / 2;i++){
		ret[i] = read_port_w(ATA_DATA + bus);
		printf("%X ", ret[i]);
	}
	ata_release();
	return ((ret[1] | (ret[0] << 16)) + 1) * 8;
	
}
void ata_select_delay(uint8_t bus){
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
	read_port(ATA_CMD + bus);
}
int ata_read(uint16_t bus,uint8_t drive,uint16_t *buffer,int LBA,int count){
	ata_grab();
    write_port(ATA_DRIVE + bus,(0xE0 + drive) | (LBA >> 24));
	ata_select_delay(bus);
    write_port(ATA_LBA_LOW + bus,(uint8_t) LBA);
    write_port(ATA_LBA_MID + bus, (uint8_t) (LBA >> 8));
    write_port(ATA_LBA_HI + bus, (uint8_t) (LBA >> 16));
    write_port(ATA_SECTOR_COUNT + bus, count);
    write_port(ATA_CMD + bus, 0x20);
    int res;
    ata_select_delay(bus);
    if (ata_schedule(bus) == 1) {
		
		return ATA_ERR;
	}
    for (int j=0;j<count;j++){
        for (int k=0;k<256;k++){
            buffer[j*256 + k] = read_port_w(ATA_DATA + bus);
        }
        ata_select_delay(bus);
    }
    return 0;
}
void ata_init(void){
	irq_install_handler(14,ata_irq);
	irq_install_handler(15,ata_irq);
	uint16_t a[256];

	ata_IDENTIFY(ATA_PRI,ATA_MASTER);
	if (ata_devices[0].status == ATA_OK)
    	printf("found ata device: ata0-master with %d sectors\n",ata_devices[0].size);

	ata_IDENTIFY(ATA_PRI,ATA_SLAVE);
	if (ata_devices[1].status == ATA_OK)
    	printf("found ata device: ata0-slave with %d sectors\n",ata_devices[1].size);

	ata_IDENTIFY(ATA_SEC,ATA_MASTER);
	if (ata_devices[2].status == ATA_OK)
    	printf("found ata device: ata1-master with %d sectors\n",ata_devices[2].size);

	ata_IDENTIFY(ATA_SEC,ATA_SLAVE);
	if (ata_devices[3].status == ATA_OK)
    	printf("found ata device: ata1-slave with %d sectors\n",ata_devices[3].size);

	atapi_read(ATA_PRI,ATA_MASTER,a,0,1);
	// for (int i=0;i<256;i++)
    // 	printf("%X ",a[i]);
	
}
