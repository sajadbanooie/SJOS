#ifndef _KERNEL_MEMORY_H
#define _KERNEL_MEMORY_H

#include <stddef.h>


// block size (4k)
#define PMMNGR_BLOCK_SIZE	4096
 
// block alignment
#define PMMNGR_BLOCK_ALIGN	PMMNGR_BLOCK_SIZE

void init_memory_manager(multiboot_info_t *multiboot_info,uint32_t kernel_end);
void* alloc_block();
void* alloc_blocks(size_t size);
#endif