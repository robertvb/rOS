/*
Copyright (c) 2015 Robert A. Vazeux Blanco

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
 */

/* refactoring from John Cronin, Thank you! */

#ifndef MMIO_H
#define MMIO_H

#include <stdint.h>

// Write value to mmio register.
static inline void write_mmio(uint32_t reg, uint32_t val)
{
    uint32_t *addr = (uint32_t *)reg;
    asm volatile(
                     "str %[val], [%[reg]]"
                     :: [reg]"r"(addr), [val]"r"(val)
                    );
}

// Read value from mmio register.
static inline uint32_t read_mmio(uint32_t reg)
{
    uint32_t *addr = (uint32_t *)reg;
    uint32_t ret;
    asm volatile(
                     "ldr %[ret], [%[reg]]"
                     : [ret]"=r"(ret)
                     : [reg]"r"(addr)
                    );
    return ret;
}

static inline void sync_mem(void)
{
    asm ("mcr p15, 0, ip, c7, c5, 0");
    asm ("mcr p15, 0, ip, c7, c5, 6");
    asm ("mcr p15, 0, ip, c7, c10, 4");
    asm ("mcr p15, 0, ip, c7, c5, 4");
}

static inline void dmb(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c10, #5" : : [zero] "r" (0));
}

static inline void dsb(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c10, #4" : : [zero] "r" (0));
}

static inline void flush(void)
{
    asm volatile ("mcr p15, #0, %[zero], c7, c14, #0" : : [zero] "r" (0));
}

#endif /* SOURCE_INCLUDES_HADES_MMIO_H_ */
