#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/tty.h>
#include <kernel/gdt.h>
#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/irq.h>
#include <kernel/portio.h>
#include <kernel/multiboot.h>
#include <kernel/memory.h>
#include <kernel/user_mode.h>
#include <kernel/ata_pio.h>
#include <kernel/paging.h>
#include <kernel/pci.h>
#include <kernel/acpi.h>
#include <kernel/pit.h>


void keyboard_handler(struct regs *r)
{
	unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = read_port(0x60);
	printk("%X ", scancode);
}
extern uint32_t kernel_end;

void kernel_early(multiboot_info_t *multiboot_info)
{
	terminal_initialize();
	init_gdt();
	init_idt();
	init_isrs();
	init_irq();
    init_pit();
	init_memory_manager(multiboot_info, (uint32_t) &kernel_end);
	init_virtual_memory((uint32_t) &kernel_end);

	install_tss();
	irq_install_handler(1,keyboard_handler);

}

void kernel_main(void)
{
    init_pci();
	ata_init();
    init_acpi();
//	 switch_to_usermode();
	// alloc_blocks(2);
	// void *a = alloc_blocks(1024);
	// printk("%X",a);

	for (;;);
}
