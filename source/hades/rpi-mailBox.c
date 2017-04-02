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

#ifndef RPI_MAILBOX_C
#define RPI_MAILBOX_C

#include "../includes/hades/rpi-mailBox.h"

void write_mailbox(uint8_t chan, uint32_t data)
{
    while (read_mmio(MAILBOX_BASE + MAILBOX_STATE) & MAILBOX_FULL) {}
    sync_mem();
    write_mmio((MAILBOX_BASE + MAILBOX_WRITE), chan | (data & DATAMASK32));
    sync_mem();
}

uint32_t read_mailbox(uint8_t chan)
{
    uint32_t timeout = 0;
    uint32_t data = 0;

    while (1)
    {
        while (read_mmio(MAILBOX_BASE + MAILBOX_STATE) & MAILBOX_EMPTY)
            if (timeout++ >= MAILBOX_TIMEOUT)
                return ERROR32;

        sync_mem();
        data = read_mmio(MAILBOX_BASE);
        sync_mem();

        if ((data & 0xF) == chan)
            break;
    }

    return (data & DATAMASK32);
}

#endif /* SOURCE_HADES_RPI_MAILBOX_C_ */
