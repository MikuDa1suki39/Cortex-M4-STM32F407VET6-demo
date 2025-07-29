#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include <stdlib.h>
#include <stddef.h>

#include "tlsf.h"

static tlsf_t tlsf;

static size_t dyl_mem_pool[64 * 1024 / sizeof(size_t)];

void mem_init(void)
{
    tlsf = tlsf_create_with_pool(dyl_mem_pool, sizeof(dyl_mem_pool));
    if (tlsf == NULL)
    {
        while (1)
            ;
    }
}
 
void *mem_malloc(size_t size)
{
    return tlsf_malloc(tlsf,size);
}

void mem_free(void *ptr)
{
    tlsf_free(tlsf,ptr);
}