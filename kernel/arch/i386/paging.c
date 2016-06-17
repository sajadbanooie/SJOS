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

extern set_page_dir(void);
extern enable_paging(void);
page_dir p_dir;

void init_virtual_memory(void){
    memset(p_dir, 0, 4094);
    for (int i = 0;i<1024;i++){
        page_table *table = (page_table *) alloc_block();
        memset(table, 0, 4094);
        p_dir[i].address = (uint32_t) table / PAGE_SIZE;
        p_dir[i].rw = 1;
        p_dir[i].present = 1;
    }
    for (int i = 0;i < 0x10000;i += PAGE_SIZE){
        map_page(i,i);
    }
    set_page_dir();
    
    write_port_w(0x8A00,0x8A00); write_port_w(0x8A00,0x08AE0);
    enable_paging();
    
}
void map_page(uint32_t v,uint32_t p){
    page_table *table = p_dir[PAGE_DIRECTORY_INDEX(v)].address * PAGE_SIZE;
    table[PAGE_TABLE_INDEX(v)]->address = p / PAGE_SIZE;
    table[PAGE_TABLE_INDEX(v)]->present = 1;
    table[PAGE_TABLE_INDEX(v)]->rw = 1;
}