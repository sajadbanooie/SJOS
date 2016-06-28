#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <kernel/idt.h>
#include <kernel/isr.h>
#include <kernel/tty.h>
#include <kernel/portio.h>

extern irq0(void);
extern irq1(void);
extern irq2(void);
extern irq3(void);
extern irq4(void);
extern irq5(void);
extern irq6(void);
extern irq7(void);
extern irq8(void);
extern irq9(void);
extern irq10(void);
extern irq11(void);
extern irq12(void);
extern irq13(void);
extern irq14(void);
extern irq15(void);

void *irq_routines[16] =
{
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
};

void irq_install_handler(int irq, void (*handler)(struct regs *r))
{
    irq_routines[irq] = handler;
}

void irq_uninstall_handler(int irq)
{
    irq_routines[irq] = 0;
}
static inline void io_wait(void)
{
    /* TODO: This is probably fragile. */
    asm volatile ( "jmp 1f\n\t"
                   "1:jmp 2f\n\t"
                   "2:" );
}
void irq_remap(void)
{
    log_info("remaping PIC chip","irq");
    write_port(0x20, 0x11);
    io_wait();
    write_port(0xA0, 0x11);
    io_wait();
    write_port(0x21, 0x20);
    io_wait();
    write_port(0xA1, 0x28);
    io_wait();
    write_port(0x21, 0x04);
    io_wait();
    write_port(0xA1, 0x02);
    io_wait();
    write_port(0x21, 0x01);
    io_wait();
    write_port(0xA1, 0x01);
    io_wait();
    write_port(0x21, 0);
    write_port(0xA1, 0x0);
    ok();
}

void init_irq(void){
	log_info("init irqs\n","irq");
	irq_remap();
	add_idt_entry(32, (unsigned)irq0, 0x08, 0x8E);
	add_idt_entry(33, (unsigned)irq1, 0x08, 0x8E);
	add_idt_entry(34, (unsigned)irq2, 0x08, 0x8E);
	add_idt_entry(35, (unsigned)irq3, 0x08, 0x8E);
	add_idt_entry(36, (unsigned)irq4, 0x08, 0x8E);
	add_idt_entry(37, (unsigned)irq5, 0x08, 0x8E);
	add_idt_entry(38, (unsigned)irq6, 0x08, 0x8E);
	add_idt_entry(39, (unsigned)irq7, 0x08, 0x8E);
	add_idt_entry(40, (unsigned)irq8, 0x08, 0x8E);
	add_idt_entry(41, (unsigned)irq9, 0x08, 0x8E);
	add_idt_entry(42, (unsigned)irq10, 0x08, 0x8E);
	add_idt_entry(43, (unsigned)irq11, 0x08, 0x8E);
	add_idt_entry(44, (unsigned)irq12, 0x08, 0x8E);
	add_idt_entry(45, (unsigned)irq13, 0x08, 0x8E);
	add_idt_entry(46, (unsigned)irq14, 0x08, 0x8E);
	add_idt_entry(47, (unsigned)irq15, 0x08, 0x8E);
}
/*  an EOI, you won't raise any more IRQs */
void irq_handler(struct regs *r)
{
    /* This is a blank function pointer */
    void (*handler)(struct regs *r);

    /* Find out if we have a custom handler to run for this
    *  IRQ, and then finally, run it */
    handler = irq_routines[r->int_no - 32];
    if (handler)
    {
        handler(r);
    }

    /* If the IDT entry that was invoked was greater than 40
    *  (meaning IRQ8 - 15), then we need to send an EOI to
    *  the slave controller */
    if (r->int_no >= 40)
    {
        write_port(0xA0, 0x20);
    }

    /* In either case, we need to send an EOI to the master
    *  interrupt controller too */
    write_port(0x20, 0x20);
}

