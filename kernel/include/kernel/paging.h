#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stddef.h>


//! page sizes are 4k
#define PAGE_SIZE 4096

// typedef struct page_dir_entry_s{
// 	uint32_t present :1;
// 	uint32_t rw :1;
// 	uint32_t privl :1;
// 	uint32_t write_through  :1;
// 	uint32_t cache_disable  :1;
// 	uint32_t accessed  :1;
// 	uint32_t dirty  :1;
// 	uint32_t zero :1;
// 	uint32_t ignored  :1;
// 	uint32_t free  :3;
// 	uint32_t address  :20;
// } page_dir_entry;
// typedef struct page_table_entry_s{
// 	uint32_t present :1;
// 	uint32_t rw :1;
// 	uint32_t privl :1;
// 	uint32_t write_through  :1;
// 	uint32_t cache_disable  :1;
// 	uint32_t accessed  :1;
// 	uint32_t zero :1;
// 	uint32_t size  :1;
// 	uint32_t global  :1;
// 	uint32_t free  :3;
// 	uint32_t address  :20;
// }  page_table_entry;


// typedef page_table_entry page_table[1024];

// typedef page_dir_entry page_dir[1024];

void init_virtual_memory(uint32_t kernel_end);
void map_page(uint32_t v,uint32_t p);
extern set_page_dir(void);
uint32_t *p_dir;

#endif