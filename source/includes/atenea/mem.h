#ifndef MEM_H
#define MEM_H

# include <stdint.h>

# define NULL    0x0
# define NBALLOC 1024

typedef union header
{
    struct
    {
        union header *next;
        unsigned len;
    } metadata;
    long align;
} u_header;

void *sbrk(uint32_t increment);
void *kmalloc(uint32_t size);
void kfree(void *ptr);
void *kcalloc(uint32_t num, uint32_t len);
void *krealloc(void *ptr, uint32_t size);


#endif /* SOURCE_INCLUDES_ATENEA_MEM_H_ */
