//acpi
#include <stdint.h>
#include <kernel/acpi.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/portio.h>
#include <kernel/paging.h>

#define START_A  0x000E0000
#define END_A  0x00100000

struct RSDPDescriptor *rsdp;
struct RSDT *rsdt;
struct FADT *fadt;

uint8_t init_rsdp();

void init_fadt();

void init_rsdt();

void enable_acpi();

uint32_t *EBDAAddress;

void init_acpi(){
    EBDAAddress = (uint32_t *) 0x040E;
    log_info("FINDING ACPI RSDP TABLE\n", "ACPI");
    init_rsdp();
    init_rsdt();
    init_fadt();
    enable_acpi();
}

uint8_t rsdp_is_valid(uint8_t *address){
    uint8_t sum = 0;
    for (int i = 0;i < sizeof(struct RSDPDescriptor);i++){
        sum += *(address + i);
    }
    return sum;
}

uint8_t init_rsdp(){
    char *address;
    uint8_t f = 0;
    for (address = (char *) START_A; address <= (char *) END_A; address += 1){
        if (!memcmp(address, "RSD PTR ", 8)){
            if (rsdp_is_valid((uint8_t *) address) == 0) {
                f = 1;
                break;
            }
        }
    }
    if (f == 0)
        for (address = (char *) EBDAAddress; address <= (char *) EBDAAddress + 0x400; address += 1){
            if (!memcmp(address, "RSD PTR ", 8)){
                if (rsdp_is_valid((uint8_t *) address) == 0) {
                    f = 1;
                    break;
                }
            }
        }
    if(f == 1) {
        printk("Found a valid ACPI RSDP table at %X\n", address);
        rsdp = (struct RSDPDescriptor *) address;
        printk("ACPI rev is %d\n", rsdp->Revision);
        ok();
    }
    else{
        error("acpi rsdp was not found");
        fail();
    }
    return 0;
}

void init_rsdt(){
    map_page(rsdp->RsdtAddress,rsdp->RsdtAddress);
    rsdt = (struct RSDT *) rsdp->RsdtAddress;
}

void init_fadt(){
    struct ACPISDTHeader *header = NULL;
    uint8_t f = 0;
    for (uint32_t i = 1;i <= rsdt->h.Length;i+=4){
        map_page(*((uint32_t*) (rsdt + i)),*((uint32_t*) (rsdt + i)));
        header = (struct ACPISDTHeader *) *((uint32_t*) (rsdt + i));
        if (!memcmp(header->Signature, "FACP",4)){
            f = 1;
            break;
        }
    }
    if (f == 1){
        fadt = (struct FADT *) header;
    }
    else{
        printk("FADT not founed");
        fail();
    }

}

void acpi_reboot(){
    write_port((uint32_t) fadt->ResetReg.Address, fadt->ResetValue);
    printk("rebooting...");
}

void enable_acpi(){
    printk("acpi mode is now enabled");
    write_port(fadt->SMI_CommandPort,fadt->AcpiEnable);

    while(read_port_w(fadt->PM1aControlBlock) & 1 == 0);
}