#ifndef _KERNEL_GDT_H
#define _KERNEL_GDT_H

#include <stddef.h>

void init_user_mode(void);
extern void switch_to_usermode(void);
#endif