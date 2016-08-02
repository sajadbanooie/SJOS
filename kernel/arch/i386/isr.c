#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/tty.h>

extern isr0(void);
extern isr1(void);
extern isr2(void);
extern isr3(void);
extern isr4(void);
extern isr5(void);
extern isr6(void);
extern isr7(void);
extern isr8(void);
extern isr9(void);
extern isr10(void);
extern isr11(void);
extern isr12(void);
extern isr13(void);
extern isr14(void);
extern isr15(void);
extern isr16(void);
extern isr17(void);
extern isr18(void);
extern isr19(void);
extern isr20(void);
extern isr21(void);
extern isr22(void);
extern isr23(void);
extern isr24(void);
extern isr25(void);
extern isr26(void);
extern isr27(void);
extern isr28(void);
extern isr29(void);
extern isr30(void);
extern isr31(void);


void init_isrs(void){
	log_info("isrs init","isr");
	add_idt_entry(0, (unsigned)isr0, 0x08, 0x8E);
	add_idt_entry(1, (unsigned)isr1, 0x08, 0x8E);
	add_idt_entry(2, (unsigned)isr2, 0x08, 0x8E);
	add_idt_entry(3, (unsigned)isr3, 0x08, 0x8E);
	add_idt_entry(4, (unsigned)isr4, 0x08, 0x8E);
	add_idt_entry(5, (unsigned)isr5, 0x08, 0x8E);
	add_idt_entry(6, (unsigned)isr6, 0x08, 0x8E);
	add_idt_entry(7, (unsigned)isr7, 0x08, 0x8E);
	add_idt_entry(8, (unsigned)isr8, 0x08, 0x8E);
	add_idt_entry(9, (unsigned)isr9, 0x08, 0x8E);
	add_idt_entry(10, (unsigned)isr10, 0x08, 0x8E);
	add_idt_entry(11, (unsigned)isr11, 0x08, 0x8E);
	add_idt_entry(12, (unsigned)isr12, 0x08, 0x8E);
	add_idt_entry(13, (unsigned)isr13, 0x08, 0x8E);
	add_idt_entry(14, (unsigned)isr14, 0x08, 0x8E);
	add_idt_entry(15, (unsigned)isr15, 0x08, 0x8E);
	add_idt_entry(16, (unsigned)isr16, 0x08, 0x8E);
	add_idt_entry(17, (unsigned)isr17, 0x08, 0x8E);
	add_idt_entry(18, (unsigned)isr18, 0x08, 0x8E);
	add_idt_entry(19, (unsigned)isr19, 0x08, 0x8E);
	add_idt_entry(20, (unsigned)isr20, 0x08, 0x8E);
	add_idt_entry(21, (unsigned)isr21, 0x08, 0x8E);
	add_idt_entry(22, (unsigned)isr22, 0x08, 0x8E);
	add_idt_entry(23, (unsigned)isr23, 0x08, 0x8E);
	add_idt_entry(24, (unsigned)isr24, 0x08, 0x8E);
	add_idt_entry(25, (unsigned)isr25, 0x08, 0x8E);
	add_idt_entry(26, (unsigned)isr26, 0x08, 0x8E);
	add_idt_entry(27, (unsigned)isr27, 0x08, 0x8E);
	add_idt_entry(28, (unsigned)isr28, 0x08, 0x8E);
	add_idt_entry(29, (unsigned)isr29, 0x08, 0x8E);
	add_idt_entry(30, (unsigned)isr30, 0x08, 0x8E);
	add_idt_entry(31, (unsigned)isr31, 0x08, 0x8E);
	ok();
}

unsigned char *exception_messages[] =
{
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Invalid TSS",
    "Segment Not Present",
    "Not imp yet11",
    "Not imp yet12",
    "Not imp yet13",
    "General Protection Fault (GPF)",
    "Page Fault",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Not imp yet",
    "Reserved",
    "Reserved"
};


void isr_handler(struct regs *r){
	if (r->int_no < 32)
    {
        /* Display the description for the Exception that occurred.
        *  In this tutorial, we will simply halt the system using an
        *  infinite loop */
		printk("%X\n", r->err_code);
        error(exception_messages[r->int_no]);
        
        //for (;;);
    }
}
