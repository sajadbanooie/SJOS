//
// Created by sajad on 8/12/16.
//

#ifndef _KERNEL_PIT_H
#define _KERNEL_PIT_H

#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42

enum PIT_MODES{
    PIT_MODE0 = 0, // interrupt on terminal count
    PIT_MODE1 = 1, // hardware re-triggerable one-shot
    PIT_MODE2 = 2, // rate generator
    PIT_MODE3 = 3, // square wave generator
    PIT_MODE4 = 4, // software triggered strobe
    PIT_MODE5 = 5, // hardware triggered strobe
};

void init_pit();
uint16_t pit_get_count(uint8_t channel);
void pit_set_reload(uint8_t channel, uint16_t reload);
void pit_set_mode(uint8_t channal,uint8_t mode);
#endif //_KERNEL_PIT_H
