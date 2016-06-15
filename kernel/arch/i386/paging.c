#include <kernel/tty.h>
#include <kernel/multiboot.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <kernel/paging.h>

#define PAGE_DIRECTORY_INDEX(x) (((x) >> 22) & 0x3ff)
#define PAGE_TABLE_INDEX(x) (((x) >> 12) & 0x3ff)
#define PAGE_GET_PHYSICAL_ADDRESS(x) (*x & ~0xfff)
