//
// Created by sajad on 7/30/16.
//
#include <stdint.h>
#include <kernel/portio.h>
#include <kernel/dma.h>


void dma_set_address(uint8_t channel, uint8_t low, uint8_t high) {

    if ( channel > 8 )
        return;

    unsigned short port = 0;
    switch ( channel ) {

        case 0: {port = DMA0_CHAN0_ADDR_REG; break;}
        case 1: {port = DMA0_CHAN1_ADDR_REG; break;}
        case 2: {port = DMA0_CHAN2_ADDR_REG; break;}
        case 3: {port = DMA0_CHAN3_ADDR_REG; break;}
        case 4: {port = DMA1_CHAN4_ADDR_REG; break;}
        case 5: {port = DMA1_CHAN5_ADDR_REG; break;}
        case 6: {port = DMA1_CHAN6_ADDR_REG; break;}
        case 7: {port = DMA1_CHAN7_ADDR_REG; break;}
    }

    write_port(port, low);
    write_port(port, high);
}


void dma_set_count(uint8_t channel, uint8_t low, uint8_t high) {

    if ( channel > 8 )
        return;

    unsigned short port = 0;
    switch ( channel ) {

        case 0: {port = DMA0_CHAN0_COUNT_REG; break;}
        case 1: {port = DMA0_CHAN1_COUNT_REG; break;}
        case 2: {port = DMA0_CHAN2_COUNT_REG; break;}
        case 3: {port = DMA0_CHAN3_COUNT_REG; break;}
        case 4: {port = DMA1_CHAN4_COUNT_REG; break;}
        case 5: {port = DMA1_CHAN5_COUNT_REG; break;}
        case 6: {port = DMA1_CHAN6_COUNT_REG; break;}
        case 7: {port = DMA1_CHAN7_COUNT_REG; break;}
    }

    write_port(port, low);
    write_port(port, high);
}


void dma_set_external_page_register(uint8_t reg, uint8_t val) {

    if (reg > 14)
        return;

    unsigned short port = 0;
    switch ( reg ) {

        case 1: {port = DMA_PAGE_CHAN1_ADDRBYTE2; break;}
        case 2: {port = DMA_PAGE_CHAN2_ADDRBYTE2; break;}
        case 3: {port = DMA_PAGE_CHAN3_ADDRBYTE2; break;}
        case 4: {return;}//! nothing should ever write to register 4
        case 5: {port = DMA_PAGE_CHAN5_ADDRBYTE2; break;}
        case 6: {port = DMA_PAGE_CHAN6_ADDRBYTE2; break;}
        case 7: {port = DMA_PAGE_CHAN7_ADDRBYTE2; break;}
    }

    write_port(port, val);
}



void dma_mask_channel(uint8_t channel) {

    if (channel <= 4)
        write_port(DMA0_CHANMASK_REG, (uint8_t) (1 << (channel - 1)));
    else
        write_port(DMA1_CHANMASK_REG, (uint8_t) (1 << (channel - 5)));
}


void dma_unmask_channel(uint8_t channel) {

    if (channel <= 4)
        write_port(DMA0_CHANMASK_REG, channel);
    else
        write_port(DMA1_CHANMASK_REG, channel);
}


void dma_unmask_all(int dma) {

    //! it doesnt matter what is written to this register
    write_port(DMA1_UNMASK_ALL_REG, 0xff);
}


void dma_set_mode(uint8_t channel, uint8_t mode) {

    int dma = (channel < 4) ? 0 : 1;
    int chan = (dma==0) ? channel : channel-4;

    dma_mask_channel (channel);
    write_port( (channel < 4) ? (DMA0_MODE_REG) : DMA1_MODE_REG, (uint8_t) (chan | (mode)));
    dma_unmask_all ( dma );
}


void dma_set_read(uint8_t channel) {

    dma_set_mode (channel,
                  DMA_MODE_READ_TRANSFER | DMA_MODE_TRANSFER_SINGLE | DMA_MODE_MASK_AUTO);
}


void dma_set_write(uint8_t channel) {

    dma_set_mode (channel,
                  DMA_MODE_WRITE_TRANSFER | DMA_MODE_TRANSFER_SINGLE | DMA_MODE_MASK_AUTO);
}


void dma_reset_flipflop(int dma) {

    if (dma < 2)
        return;

    //! it doesnt matter what is written to this register
    write_port( (dma==0) ? DMA0_CLEARBYTE_FLIPFLOP_REG : DMA1_CLEARBYTE_FLIPFLOP_REG, 0xff);
}


void dma_reset(int dma) {

    //! it doesnt matter what is written to this register
    write_port(DMA0_TEMP_REG, 0xff);
}

