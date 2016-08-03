#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <kernel/portio.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/pci.h>
#include <kernel/tty.h>

#define CONFIG_ADDRESS 0xCF8
#define CONFIG_DATA 0xCFC

uint32_t pci_config_read_dword (uint8_t bus, uint8_t device,uint8_t func, uint8_t reg){
    uint32_t address;
    address = (bus << 16) | (device << 11) | (func << 8) | (reg & 0xfc) | ((uint32_t)0x80000000);
    write_port_d(CONFIG_ADDRESS,address);
    return read_port_d(CONFIG_DATA);
}

void init_pci(void){
    for (int i = 0;i<32;i++)
        printk("%X ",pci_config_read_dword(1,i,0,8));
}

