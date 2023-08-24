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
