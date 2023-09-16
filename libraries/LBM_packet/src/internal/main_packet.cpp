#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

#include "app_param/default_config_datas.h"

////////////////////////////////////////////////////////////////////////////////
// Variables

uint8_t app_lora_packet_buffer[LORAWAN_APP_DATA_MAX_SIZE] = { 0 };
uint8_t app_lora_packet_len = 0;

uint8_t factory_sensor_temp_len = 0;
uint8_t factory_sensor_data_temp[64] = { 0 };

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


void app_task_factory_sensor_data_send( void )
{
    memset( factory_sensor_data_temp, 0, sizeof( factory_sensor_data_temp ));
    factory_sensor_temp_len = 0;

    factory_sensor_data_temp[0] = DATA_ID_UP_PACKET_FACT_SENSOR;

    //event  status
    factory_sensor_data_temp[1] = ( state_all >> 16 ) & 0xff;
    factory_sensor_data_temp[2] = ( state_all >> 8 ) & 0xff;
    factory_sensor_data_temp[3] = ( state_all ) & 0xff;

    factory_sensor_temp_len = factory_sensor_temp_len + 4;

    //utc time
    uint32_t utc_temp = app_task_track_get_utc( );
    memcpyr( factory_sensor_data_temp + 4, ( uint8_t * )( &utc_temp ), 4 );

    factory_sensor_temp_len = factory_sensor_temp_len + 4;

    //sensor datas
    memcpy( factory_sensor_data_temp + factory_sensor_temp_len, factory_sensor_data, factory_sensor_data_len ); // utc + sensor data
    factory_sensor_temp_len = factory_sensor_data_len + factory_sensor_temp_len;

    app_task_lora_tx_queue( factory_sensor_data_temp, factory_sensor_temp_len, true, false );
    factory_sensor_data_len = 20;

    hal_mcu_trace_print("factory sensor datas uplink:");
    for(uint8_t u8i = 0; u8i < factory_sensor_temp_len; u8i++ )
    {
        hal_mcu_trace_print("%02x",factory_sensor_data_temp[u8i]);
    }
    hal_mcu_trace_print("\r\n");     

    sound_sample_index = 0;
    sound_average_data = 0;
    state_all = 0;

}



void app_task_user_sensor_data_send( void )
{
    uint8_t user_sensor_temp_len = 0;
    uint8_t user_sensor_data_temp[64] = { 0 };   
    memset( user_sensor_data_temp, 0, sizeof( user_sensor_data_temp ));
    user_sensor_temp_len = 0;

    user_sensor_data_temp[0] = DATA_ID_UP_PACKET_USER_SENSOR;

    //event  status
    user_sensor_data_temp[2] = ( state_all >> 16 ) & 0xff;
    user_sensor_data_temp[3] = ( state_all >> 8 ) & 0xff;
    user_sensor_data_temp[4] = ( state_all ) & 0xff;

    //utc time
    uint32_t utc_temp = app_task_track_get_utc( );
    memcpyr( user_sensor_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );
    user_sensor_temp_len = 9;

    //sensor datas
    if(user_sensor_data_len % 4 == 0) //must be 4 bytes/group
    {
        memcpy( user_sensor_data_temp + user_sensor_temp_len, user_sensor_data, user_sensor_data_len ); // sensor data
        user_sensor_temp_len = user_sensor_temp_len + user_sensor_data_len;
    }
    //package len
    user_sensor_data_temp[1] = user_sensor_temp_len;

    app_task_lora_tx_queue( user_sensor_data_temp, user_sensor_temp_len, true, false );

    user_sensor_data_len = 0;
    hal_mcu_trace_print("user sensor datas uplink:");
    for(uint8_t u8i = 0; u8i < user_sensor_temp_len; u8i++ )
    {
        hal_mcu_trace_print("%02x ",user_sensor_data_temp[u8i]);
    }
    hal_mcu_trace_print("\r\n"); 

    state_all = 0;
}
