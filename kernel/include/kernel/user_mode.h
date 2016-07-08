#ifndef _KERNEL_USERMODE_H
#define _KERNEL_USERMODE_H

#include <stddef.h>

void init_user_mode(void);
extern void switch_to_usermode(void);
#endif