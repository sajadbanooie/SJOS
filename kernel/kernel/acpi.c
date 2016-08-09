//acpi
#include <stdint.h>
#include <kernel/acpi.h>
#include <string.h>
#include <kernel/tty.h>

#define ROM_A  0x000F0000
#define ROM_A_END  0x000FFFFF

struct RSDPDescriptor rsdp;

uint8_t init_rsdp();

void init_acpi(){
    log_info("FINDING ACPI RSDP TABLE", "ACPI");
    init_rsdp();
}

uint8_t init_rsdp(){
    char *address;
    for (address = (char *) ROM_A; address <= (char *) ROM_A_END; address += 8){
        if (!memcmp(address, "RSD PTR ", 8)){
            printk("Found ACPI RSDP table at %X\n",address);
            printk("ACPI rev is %d\n",*(address + 16));
        }
    }
    ok();

    memcpy(&rsdp,address, sizeof(rsdp));
    return 0;
}