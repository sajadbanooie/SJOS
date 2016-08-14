#include <stdint.h>
#include <kernel/pit.h>
#include <kernel/portio.h>
#include <kernel/irq.h>
#include <kernel/isr.h>
#include <kernel/tty.h>

#define PIT_MODE 0x43



void pit_irq_handler(struct regs *r){
//    printk("timerrr");
}

void init_pit() {
    irq_install_handler(0,pit_irq_handler);
//    pit_set_mode(PIT_CHANNEL0, PIT_MODE5);
//    pit_set_reload(PIT_CHANNEL0, 1);
}

uint16_t pit_get_count(uint8_t channel) {
    return read_port(channel) | (read_port(channel) << 8);
}

void pit_set_reload(uint8_t channel, uint16_t reload) {
    write_port(channel, (uint8_t) (reload & 0xFF));
    write_port(channel, (uint8_t) (reload >> 8));
}

void pit_set_mode(uint8_t channel, uint8_t mode) {
    write_port(PIT_MODE, PIT_MODE0);
}



