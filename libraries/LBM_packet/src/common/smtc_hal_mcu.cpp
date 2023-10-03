#include "smtc_hal_mcu.h"
#include <cstdarg>
#include <cstdio>
#include <Arduino.h>

void hal_mcu_trace_print( const char* fmt, ... )
{
}

void __attribute__( ( optimize( "O0" ) ) ) hal_mcu_wait_us( const int32_t microseconds )
{
    // Work @64MHz
    const uint32_t nb_nop = microseconds * 1000 / 171;
    for( uint32_t i = 0; i < nb_nop; i++ )
    {
        __NOP( );
    }
}
void hal_mcu_wait_ms( const int32_t ms )
{
    for( int32_t i = 0; i < ms; i++ )
        hal_mcu_wait_us( 1000 );
}

void hal_mcu_reset( void )
{
    smtc_modem_hal_reset_mcu();
}

