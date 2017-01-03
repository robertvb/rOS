
#ifndef RPI_SYSTIMER_H
#define RPI_SYSTIMER_H

#include <stdint.h>

#define RPI_SYSTIMER_BASE       0x20003000


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
