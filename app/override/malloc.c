#include "stdint.h"
#include "stdbool.h"
#include "string.h"

#include <stddef.h>

#include "mem_mgnt.h"

void *malloc(size_t size)
{
    return mem_malloc(size);
}

void free(void *ptr)
{
    mem_free(ptr);
}