#ifndef _MEM_MGNT_H
#define _MEM_MGNT_H

#include <stddef.h>

void mem_init(void);

void *mem_malloc(size_t size);

void mem_free(void *ptr);

#endif // !_MEM_MGNT_H

