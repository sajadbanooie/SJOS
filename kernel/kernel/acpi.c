//acpi
#include <stdint.h>
#include <kernel/acpi.h>
#include <string.h>
#include <kernel/tty.h>
#include <kernel/portio.h>

#define START_A  0x000E0000
#define END_A  0x00100000

struct RSDPDescriptor *rsdp;
struct RSDT *rsdt;
struct FADT *fadt;

uint8_t init_rsdp();

void init_fadt();

void init_rsdt();

uint32_t *EBDAAddress;

void init_acpi(){
    EBDAAddress = (uint32_t *) 0x040E;
    log_info("FINDING ACPI RSDP TABLE\n", "ACPI");
    init_rsdp();
    init_rsdt();
//    init_fadt();
}

uint8_t rsdp_is_valid(uint8_t *address){
    uint8_t sum = 0;
    for (int i = 0;i < sizeof(rsdp);i++){
        sum += *(address + i);
    }
    return sum;
}

uint8_t init_rsdp(){
    char *address;
    uint8_t f = 0;
    for (address = (char *) START_A; address <= (char *) END_A; address += 8){
        if (!memcmp(address, "RSD PTR ", 8)){
            if (rsdp_is_valid((uint8_t *) address) == 0) {
                f = 1;
                break;
            }
        }
    }
    if (f == 0)
        for (address = (char *) EBDAAddress; address <= (char *) EBDAAddress + 0x400; address += 8){
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
    printk("%X",rsdp->RsdtAddress);
    rsdt = (struct RSDT *) rsdp->RsdtAddress;

}

void init_fadt(){
    uint8_t f = 0;
    char *address;
    for (address = (char *) (&rsdt + sizeof(rsdt) + 1); address < (char *) (&rsdt + rsdt->h.Length + 1);address += 4){
        if (!memcmp(address, "FACT", 4)){
            f = 1;
            break;
        }
    }
    if(f == 1) {
        fadt = (struct FADT *) address;
    }
    else{
        error("acpi fadt was not found");
        fail();
    }
}

void acpi_reboot(){
    write_port((uint32_t) fadt->ResetReg.Address, fadt->ResetValue);
}