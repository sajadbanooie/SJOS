#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/memory.h>

//! size of physical memory
uint32_t	memory_size=0;
 
//! number of blocks currently in use
uint32_t	used_blocks=0;
 
//! maximum number of available memory blocks
uint32_t	max_blocks=1048576;
 
//! memory map bit array. Each bit represents a memory block
uint32_t*	memory_bit_map= 0;

memory_map_t *memory_map;

void mmap_unset_bit(int n){
    memory_bit_map[n / 32] |= 1 << (n % 32);
}

void mmap_set_bit(int n){
    memory_bit_map[n / 32] &= ~(1 << (n % 32));
}
int mmap_first_free(){
    for (int i =0;i<max_blocks/32;i++){
        if (memory_bit_map[i] != 0){
            for (int j = 0;j<32;j++){
                int x = ((memory_bit_map[i] & (1 << j)));
                if (x != 0){
                    return (i * 32 + j);
                }
            }
        }
    }
    return -1;
}
int mmap_first_frees(size_t size){
    int n = 0;
    int block = -1;
    for (int i =0;i<max_blocks/32;i++){
        if (memory_bit_map[i] != 0){
            for (int j = 0;j<32;j++){
                int x = ((memory_bit_map[i] & (1 << j)));
                if (x != 0)
                    n++;
                if (x == 0)
                    n = 1;
                if (n == 1)
                    block = (i * 32 + j);
                if (n == size)
                    break;
            }
        }
        if (n == size)
            break;
    }
    return block;
}

void init_region(int start,int len){
    for (int i = start / 4096;i<(start + len)/4096 + 1;i++)
        mmap_unset_bit(i);
}
void dinit_region(int start,int len){
    for (int i = start / 4096;i<(start + len)/4096 + 1;i++)
        mmap_set_bit(i);
}
void* alloc_block(){
    int block = mmap_first_free();
    if (block >= 0){
        mmap_set_bit(block);
        return 4096 * block;
        
    }
    return -1;
}
void* alloc_blocks(size_t size){
    int block = mmap_first_frees(size);
    if (block >= 0){
        for (int i=0;i<size;i++)
            mmap_set_bit(block + i);
        return 4096 * block;
        
    }
    return -1;
}

void free_block(void *p){
    int block = (int) p / 4096;
    mmap_unset_bit(block);
}
void free_blocks(void *p,size_t size){
    int block = (int) p / 4096;
    for (int i=0;i<size;i++)
            mmap_unset_bit(block + i);
    
}
void init_memory_manager(multiboot_info_t *multiboot_info, uint32_t kernel_end){
    memory_size = multiboot_info->mem_lower + (multiboot_info->mem_upper);
    max_blocks = memory_size * 1024 / 4096;
    printf("%d\n", memory_size * 1024);
    memory_map = (memory_map_t *) (multiboot_info->mmap_addr);
    printf("%X\n",memory_map);
    memory_bit_map = kernel_end + 1;
    memset(memory_bit_map, 0, max_blocks/8);
   
    while (memory_map < multiboot_info->mmap_addr + multiboot_info->mmap_length){
        int start = (memory_map->base_addr_high * 64) | (memory_map->base_addr_low);
        int len = (memory_map->length_high * 64) | (memory_map->length_low);
        if (memory_map->type == 1)
            init_region(start, len);
        printf("start: %d length: %d type: %d ",start,len,memory_map->type);
        memory_map = (memory_map_t*) ( (unsigned int)memory_map +memory_map->size + sizeof(unsigned int) );
    }

    dinit_region(0x100000, kernel_end + max_blocks/8);
    printf("fist free block: %d\n", mmap_first_free());
}