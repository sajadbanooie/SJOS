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

void keyboard_handler(struct regs *r)
{
	unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = read_port(0x60);
	printf("%X ",scancode);
}
extern uint32_t kernel_end;
void kernel_early(multiboot_info_t *multiboot_info)
{
	terminal_initialize();
	init_gdt();
	init_idt();
	init_isrs();
	init_irq();
	init_memory_manager(multiboot_info, &kernel_end);
	install_tss();
	irq_install_handler(1,keyboard_handler);
	// ata_init();
	init_virtual_memory();
}

void kernel_main(void)
{
	
	//switch_to_usermode();
	// alloc_blocks(2);
	// void *a = alloc_blocks(1024);
	// printf("%X",a);
	for (;;);
}
