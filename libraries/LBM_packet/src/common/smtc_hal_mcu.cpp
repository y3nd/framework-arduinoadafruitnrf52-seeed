#include "smtc_hal_mcu.h"
#include <cstdarg>
#include <cstdio>
#include <Arduino.h>

static void vprint1( const char* fmt, va_list argp )
{
    char string[255];
    if( 0 < vsprintf( string, fmt, argp ) )  // build string
    {
        Serial1.write(( uint8_t* ) string, strlen( string ) );
    }
}

void hal_mcu_trace_print( const char* fmt, ... )
{
#if 0
    Serial1.begin(115200);
    va_list args;

    va_start(args, fmt);
    vprint1(fmt, args);

    va_end(args);
#endif
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
    for( uint32_t i = 0; i < ms; i++ )
        hal_mcu_wait_us( 1000 );
}

void hal_mcu_reset( void )
{
    smtc_modem_hal_reset_mcu();
}

