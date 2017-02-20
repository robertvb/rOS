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

#ifndef RPI_SYSTIMER_H
#define RPI_SYSTIMER_H

#include <stdint.h>
#include "../poseidon/rpi-base.h"

#define RPI_SYSTIMER_BASE 0x20003000


typedef volatile struct {
     uint32_t control_status;
     uint32_t counter_lo;
     uint32_t counter_hi;
     uint32_t compare0;
     uint32_t compare1;
     uint32_t compare2;
     uint32_t compare3;
    } rpi_sys_timer_t;


rpi_sys_timer_t* RPI_GetSystemTimer(void);
void RPI_esperarMicroSeconds( uint32_t us );

#endif
