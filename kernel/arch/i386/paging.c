#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <kernel/paging.h>
#include <kernel/memory.h>
#include <kernel/portio.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)


void init_virtual_memory(uint32_t kernel_end){
    p_dir = (uint32_t *) alloc_block();
    memset(p_dir, 0, 4096);
    for (int i = 0;i<1024;i++){
        uint32_t *table = (uint32_t *) alloc_block();
        memset(table, 0, 4096);
        p_dir[i] |= ((uint32_t) table / 4096) << 12;
        p_dir[i] |= 0x2;
        p_dir[i] |= 1;
    }
    for (int i = 0;i <=0x3ff0000;i++){
        map_page(i,i);
    }
    set_page_dir();
    
    // write_port_w(0x8A00,0x8A00); write_port_w(0x8A00,0x08AE0);
    enable_paging();
    
}
void map_page(uint32_t v,uint32_t p){
    uint32_t *table = (uint32_t *) ((p_dir[PAGE_DIRECTORY_INDEX(v)] & 0xFFFFFF000)) ;
    table[PAGE_TABLE_INDEX(v)] |= (p / 4096) << 12;
    table[PAGE_TABLE_INDEX(v)] |= 1;
}
