#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

#include "app_param/default_config_datas.h"

////////////////////////////////////////////////////////////////////////////////
// Variables

uint8_t app_lora_packet_buffer[LORAWAN_APP_DATA_MAX_SIZE] = { 0 };
uint8_t app_lora_packet_len = 0;






void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size )
{
    dst = dst + ( size - 1 );
    while( size-- )
    {
        *dst-- = *src++;
    }
}

void default_param_load(void)
{
    is_first_time_sync = false;
    lorawan_param_init();
}





