cmake_minimum_required(VERSION 3.5)
project(sjos)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    kernel/arch/i386/ata_pio.c
    kernel/arch/i386/gdt.c
    kernel/arch/i386/idt.c
    kernel/arch/i386/irq.c
    kernel/arch/i386/isr.c
    kernel/arch/i386/memory.c
    kernel/arch/i386/paging.c
    kernel/arch/i386/pci.c
    kernel/arch/i386/tty.c
    kernel/arch/i386/user_mode.c
    kernel/include/kernel/ata_pio.h
    kernel/include/kernel/gdt.h
    kernel/include/kernel/idt.h
    kernel/include/kernel/irq.h
    kernel/include/kernel/isr.h
    kernel/include/kernel/memory.h
    kernel/include/kernel/multiboot.h
    kernel/include/kernel/paging.h
    kernel/include/kernel/pci.h
    kernel/include/kernel/portio.h
    kernel/include/kernel/tty.h
    kernel/include/kernel/user_mode.h
    kernel/include/kernel/vga.h
    kernel/kernel/kernel.c
    libc/include/sys/cdefs.h
    libc/include/stdio.h
    libc/include/stdlib.h
    libc/include/string.h
    libc/stdio/printf.c
    libc/stdio/putchar.c
    libc/stdio/puts.c
    libc/stdlib/abort.c
    libc/string/memcmp.c
    libc/string/memcpy.c
    libc/string/memmove.c
    libc/string/memset.c
    libc/string/strlen.c)

include_directories(.)
include_directories(kernel/include)
include_directories(libc/include)
add_executable(sjos ${SOURCE_FILES})
