#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stddef.h>

//! page table represents 4mb address space
#define PTABLE_ADDR_SPACE_SIZE 0x400000

//! directory table represents 4gb address space
#define DTABLE_ADDR_SPACE_SIZE 0x100000000

//! page sizes are 4k
#define PAGE_SIZE 4096

enum PAGE_PDE_FLAGS {
 
	I86_PDE_PRESENT			=	1,		//0000000000000000000000000000001
	I86_PDE_WRITABLE		=	2,		//0000000000000000000000000000010
	I86_PDE_USER			=	4,		//0000000000000000000000000000100
	I86_PDE_PWT			=	8,		//0000000000000000000000000001000
	I86_PDE_PCD			=	0x10,		//0000000000000000000000000010000
	I86_PDE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PDE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PDE_4MB			=	0x80,		//0000000000000000000000010000000
	I86_PDE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PDE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
   	I86_PDE_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
};
enum PAGE_PTE_FLAGS {
 
	I86_PTE_PRESENT			=	1,		//0000000000000000000000000000001
	I86_PTE_WRITABLE		=	2,		//0000000000000000000000000000010
	I86_PTE_USER			=	4,		//0000000000000000000000000000100
	I86_PTE_WRITETHOUGH		=	8,		//0000000000000000000000000001000
	I86_PTE_NOT_CACHEABLE		=	0x10,		//0000000000000000000000000010000
	I86_PTE_ACCESSED		=	0x20,		//0000000000000000000000000100000
	I86_PTE_DIRTY			=	0x40,		//0000000000000000000000001000000
	I86_PTE_PAT			=	0x80,		//0000000000000000000000010000000
	I86_PTE_CPU_GLOBAL		=	0x100,		//0000000000000000000000100000000
	I86_PTE_LV4_GLOBAL		=	0x200,		//0000000000000000000001000000000
   	I86_PTE_FRAME			=	0x7FFFF000 	//1111111111111111111000000000000
};

typedef page_table_entry uint32_t;
typedef page_table page_table_entry[1024];
typedef page_dir_entry uint32_t;
typedef page_dir page_table_entry[1024];

void init_virtual_memory(void);
void alloc_page(page_table_entry pte);
void pd_entry_set_attrib (page_dir_entry* e, uint32_t attrib);
void pd_entry_get_attrib (page_dir_entry* e, uint32_t attrib);
void pd_entry_del_attrib (page_dir_entry* e, uint32_t attrib);
void pd_entry_set_frame (page_dir_entry* e, uint32_t frame);
void pt_entry_set_attrib (page_dir_entry* e, uint32_t attrib);
void pt_entry_get_attrib (page_dir_entry* e, uint32_t attrib);
void pt_entry_del_attrib (page_dir_entry* e, uint32_t attrib);
void pt_entry_set_frame (page_dir_entry* e, uint32_t frame);


#endif