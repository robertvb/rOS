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

#ifndef RPI_MAILBOX_H
#define RPI_MAILBOX_H

#include <stdint.h>
#include "mmio.h"

/* Masks */
#define MASK32     0x00000000
#define DATAMASK32 0xFFFFFFF0

/* Status */
#define ERROR32    0xFFFFFFFF

# define MAILBOX_BASE    0x2000B880

# define MAILBOX_READ    0x00
# define MAILBOX_WRITE   0x20
# define MAILBOX_STATE   0x18

# define MAILBOX_PROP    8

# define MAILBOX_FULL    0x80000000
# define MAILBOX_EMPTY   0x40000000

# define MAILBOX_TIMEOUT (1 << 20)

# define REP_SUCCESS     0x80000000
# define REP_FAILURE     0x80000001

# define CLOCK_TAG       0x00030002
# define POWERGET_TAG    0x00020001
# define POWERSET_TAG    0x00028001

void write_mailbox(uint8_t chan, uint32_t data);
uint32_t read_mailbox(uint8_t chan);

#endif /* SOURCE_INCLUDES_HADES_RPI_MAILBOX_H_ */
