#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

uint8_t send_retry_type = 1; // 0: at most confirm twice;  1: at most confirm once; 2:don't need check confirm 


uint8_t app_task_lora_tx_buffer_len[APP_TASK_LORA_TX_QUEUE_MAX] = { 0 };
bool app_task_lora_tx_buffer_confirmed[APP_TASK_LORA_TX_QUEUE_MAX] = { false };
uint8_t app_task_lora_tx_buffer[APP_TASK_LORA_TX_QUEUE_MAX][LORAWAN_APP_DATA_MAX_SIZE] = { 0 };
uint8_t app_task_lora_tx_in = 0;
uint8_t app_task_lora_tx_out = 0;

uint32_t app_send_confirmed_count = 0;
uint32_t app_task_lora_tx_confirmed_count = 0;  

static uint32_t tx_last_timestemp = 0;

bool app_task_lora_tx_check = false;

bool app_task_lora_tx_off_line = false;
bool app_task_lora_tx_cache = true;
static uint32_t   app_task_lora_tx_toa = 0;


void app_lora_confirmed_count_increment( void )
{
    app_send_confirmed_count++;
}

uint32_t app_lora_get_confirmed_count( void )
{
    return app_send_confirmed_count;
}


bool app_task_lora_tx_engine( void )
{
    static uint8_t packet_send_cnt = 0;

    smtc_modem_status_mask_t modem_status;
    smtc_modem_region_t cur_region;
    uint8_t  dutycycle_enable = 0;

    static uint8_t last_time_tx_res = false;
    static uint8_t out = 0;

    uint32_t tx_now_timestemp = smtc_modem_hal_get_time_in_ms( );

    ASSERT_SMTC_MODEM_RC( smtc_modem_get_region(0, &cur_region));
    
    ASSERT_SMTC_MODEM_RC( smtc_modem_get_duty_cycle_enable(&dutycycle_enable));

    if(dutycycle_enable == 1)
    {
        if((cur_region == SMTC_MODEM_REGION_EU_868) || (cur_region == SMTC_MODEM_REGION_RU_864))
        {
            hal_mcu_trace_print( "app_task_lora_tx_toa: %u\r\n", app_task_lora_tx_toa );
            if(tx_now_timestemp<(tx_last_timestemp+app_task_lora_tx_toa * 99))  //Limit dutycycle to less than 1% 
            {
                last_time_tx_res = false;
                return false;
            }
        }
    }

    if( app_task_lora_tx_check ) //need check confirmed last time send packet
    {
        app_task_lora_tx_check = false;
        uint32_t count_temp = app_lora_get_confirmed_count( );
        
        if( app_task_lora_tx_confirmed_count >= count_temp )    //unconfirmed
        {
            if(app_task_lora_tx_cache == true)
            {
                if(packet_send_cnt >= 2)
                {
                    packet_send_cnt = 0;
                    app_task_lora_save_tx_data( app_task_lora_tx_buffer[app_task_lora_tx_out], app_task_lora_tx_buffer_len[app_task_lora_tx_out] );
                    app_task_lora_tx_buffer_len[out] = 0;   //Clear the last data
                    out = (++app_task_lora_tx_out % APP_TASK_LORA_TX_QUEUE_MAX);
                }   
            }
            else
            {
                if(send_retry_type == 1) //just confirm once,then unconfirm
                {
                    if(packet_send_cnt >= 1)
                    {
                        packet_send_cnt = 0;
                        app_task_lora_tx_buffer_confirmed[app_task_lora_tx_out] = false;     
                    }
                } 
                else if(send_retry_type == 0) //confirm twice,then unconfirm 
                {
                    if(packet_send_cnt >= 2)
                    {
                        packet_send_cnt = 0;
                        app_task_lora_tx_buffer_confirmed[app_task_lora_tx_out] = false; 
                    }    
                }
                else
                {
                    packet_send_cnt = 0;
                    app_task_lora_tx_buffer_len[out] = 0;   //Clear the last data
                    out = (++app_task_lora_tx_out % APP_TASK_LORA_TX_QUEUE_MAX);                
                }
            }
        }
        else    //confirmed
        {
            if( app_task_lora_tx_off_line )
            {
                app_task_lora_tx_off_line = false;
                int8_t i = 3;
                while( i-- )
                {
                    if( delete_pos_msg_datas( 1, true ))
                    {
                        hal_mcu_trace_print( "app delete old pos data ok\r\n" );
                        break;
                    }
                }
            }
            app_task_lora_tx_buffer_len[out] = 0;   //Clear the last data
            out = (++app_task_lora_tx_out % APP_TASK_LORA_TX_QUEUE_MAX);
        }
    }
    else
    {
        if(last_time_tx_res == true)
        {
            app_task_lora_tx_buffer_len[out] = 0;   //Clear the last data
            out = (++app_task_lora_tx_out % APP_TASK_LORA_TX_QUEUE_MAX);
        }
    }
    
    if( app_task_lora_tx_buffer_len[out] )
    {
        
        smtc_modem_get_status( 0, &modem_status );
        if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED 
            && app_lora_is_idle( ) 
            && app_task_radio_gnss_is_busy( ) == false && app_task_radio_wifi_is_busy( ) == false 
            && app_task_track_gnss_is_busy( ) == false && app_task_track_wifi_is_busy( ) == false )
        {
            bool result = app_lora_send_frame( app_task_lora_tx_buffer[out], app_task_lora_tx_buffer_len[out], app_task_lora_tx_buffer_confirmed[out], false );
            if( result )
            {
                tx_last_timestemp = smtc_modem_hal_get_time_in_ms( );
                app_task_lora_tx_out = out;
                hal_mcu_trace_print( "app_task_lora_tx_out: %u,confirm:%s\r\n", app_task_lora_tx_out,app_task_lora_tx_buffer_confirmed[out] == true ? "true":"false" );
                
                smtc_modem_get_toa_status( &app_task_lora_tx_toa,app_task_lora_tx_buffer_len[out] + 13 );

                if( app_task_lora_tx_buffer_confirmed[out] )
                {
                    packet_send_cnt ++;
                    app_task_lora_tx_check = true;
                    app_task_lora_tx_off_line = false;
                    app_task_lora_tx_confirmed_count = app_lora_get_confirmed_count( );
                }
                else
                {
                    app_task_lora_tx_check = false;
                }
                last_time_tx_res = true;
                return true;
            }
        }
        else
        {
            last_time_tx_res = false;
            return false;
        }
    }
    else
    {
        if(app_task_lora_tx_cache == true)
        {
            app_taks_lora_off_line_send_and_check();
        }
        last_time_tx_res = false;
        return false;
    }
    last_time_tx_res = true;
    return true;
}


bool app_task_lora_tx_queue( uint8_t *buf, uint8_t len, bool confirmed, bool emergency )
{
    bool result = false;
    smtc_modem_status_mask_t modem_status;
    smtc_modem_get_status( 0, &modem_status );
    if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED )
    {
        hal_mcu_trace_print( "app_task_lora_tx_in: %u\r\n", app_task_lora_tx_in );        
        uint8_t in = app_task_lora_tx_in  % APP_TASK_LORA_TX_QUEUE_MAX;
        app_task_lora_tx_buffer_len[in] = len;
        app_task_lora_tx_buffer_confirmed[in] = confirmed;
        for( uint8_t j = 0; j < len; j++ )
        {
            app_task_lora_tx_buffer[in][j] = buf[j];
        }
        app_task_lora_tx_in = (++app_task_lora_tx_in)%APP_TASK_LORA_TX_QUEUE_MAX;
    }
    else
    {
        result = false;
    }

    return result;
}

uint32_t app_task_lora_get_timestamp( void )
{
    uint32_t timestamp_now = 0;
    if( app_task_lora_clock_is_synch( ))
    {
        timestamp_now = apps_modem_common_get_utc_time( );
    }
    return timestamp_now;
}


void app_task_packet_downlink_decode( uint8_t *buf, uint8_t len )
{
    uint8_t data_id = 0;

    if( buf && len )
    {
        data_id = buf[0];
        switch( data_id )
        {
            case DATA_ID_DOWN_PACKET_USER_CODE:
                {

                }
                break;

            default:
                break;
        }
    }
}


bool app_task_lora_save_tx_data( uint8_t *buf, uint8_t len )
{
    bool result = false;
    uint8_t ret = 0;

    memset(( uint8_t * )( &pos_msg_param ), 0, sizeof( pos_msg_param ));

    pos_msg_param.pos_type = buf[0];

    if( pos_msg_param.pos_type != DATA_ID_UP_PACKET_GNSS_RAW )
    {
        memcpyr(( uint8_t * )( &pos_msg_param.pos_status ), buf + 1, 4 );
        memcpyr(( uint8_t * )( &pos_msg_param.utc_time ), buf + 5, 4 );
    }

    switch( pos_msg_param.pos_type )
    {
        case DATA_ID_UP_PACKET_GNSS_RAW:
        {
            pos_msg_param.context_count = 1;
            pos_msg_param.context.gps_context.zone_flag = buf[1]; // fragment data
            pos_msg_param.context.gps_context.gnss_len = buf[4]; // GNSS raw lenght
            memcpyr(( uint8_t * )( &pos_msg_param.context.gps_context.group_id ), buf + 2, 2 ); // group id
            memcpy( pos_msg_param.context.gps_context.gnss_res, buf + 5, pos_msg_param.context.gps_context.gnss_len ); // GNSS raw data            
        }
        break;
        case DATA_ID_UP_PACKET_GNSS_END:
        {
            pos_msg_param.context_count = 1;
            pos_msg_param.context.gps_context.zone_flag = buf[9]; // fragment data
            memcpyr(( uint8_t * )( &pos_msg_param.context.gps_context.group_id ), buf + 10, 2 ); // group id
            pos_msg_param.context.gps_context.gnss_len = 0; // GNSS raw lenght
        }
        break;
        case DATA_ID_UP_PACKET_WIFI_RAW:
        {
            pos_msg_param.context_count = ( len - 9 ) / 7;  
            for( uint8_t i = 0; i < pos_msg_param.context_count; i++ )
            {
                memcpy( &pos_msg_param.context.wifi_context[i].wifi_mac, buf + 9 + i * 7, 6 );
                memcpy( &pos_msg_param.context.wifi_context[i].cur_rssi, buf + 9 + i * 7 + 6, 1 );
            }
        }
        break;
        case DATA_ID_UP_PACKET_BLE_RAW:
        {
            pos_msg_param.context_count = ( len - 9 ) / 7;
            for( uint8_t i = 0; i < pos_msg_param.context_count; i++ )
            {
                memcpy( &pos_msg_param.context.beac_context[i].beac_mac, buf + 9 + i * 7, 6 );
                memcpy( &pos_msg_param.context.beac_context[i].cur_rssi, buf + 9 + i * 7 + 6, 1 );
            }
        }
        break;
        case DATA_ID_UP_PACKET_POS_STATUS:
        {
            pos_msg_param.context_count = 1;
        }
        break;
        case DATA_ID_UP_PACKET_USER_SENSOR:
        {
            pos_msg_param.context_count = 1;
            pos_msg_param.context.sensor_context.len = len-1;
            memcpy( &pos_msg_param.context.sensor_context.sensor_data, &buf[1], len-1 );

        }
        break;
        case DATA_ID_UP_PACKET_FACT_SENSOR:
        {
            pos_msg_param.context_count = 1;
            pos_msg_param.context.sensor_context.len = len-1;
            memcpy( &pos_msg_param.context.sensor_context.sensor_data, &buf[1], len-1 );
        }
        break;  
        default:
        break;
    }

    ret = write_position_msg(  );
    if( ret == 0 ) hal_mcu_trace_print( "Save off-line data ok\r\n" );
    else hal_mcu_trace_print( "Save off-line data fail\r\n" );
    return result = ret == 0 ? true:false;
}

void app_taks_lora_off_line_send_and_check( void )
{
    smtc_modem_status_mask_t modem_status;
    pos_msg_param_t pos_msg;

    memset( app_lora_data_tx_buffer, 0, sizeof( app_lora_data_tx_buffer ));
    app_lora_data_tx_size = 0;

    uint16_t pos_msg_cnt = get_pos_msg_cnt( );
    if( pos_msg_cnt == 0 )
    {
        return;
    }

    if( read_pos_data( &pos_msg, true ))
    {
        app_lora_data_tx_buffer[0] = pos_msg.pos_type;

        if( pos_msg.pos_type != DATA_ID_UP_PACKET_GNSS_RAW )
        {
            memcpyr( app_lora_data_tx_buffer + 1, ( uint8_t * )( &pos_msg.pos_status ), 4 );
            memcpyr( app_lora_data_tx_buffer + 5, ( uint8_t * )( &pos_msg.utc_time ), 4 );
        }

        switch( pos_msg.pos_type )
        {
            case DATA_ID_UP_PACKET_WIFI_RAW:
            {
                for( uint8_t i = 0; i < pos_msg.context_count; i++ )
                {
                    memcpy( app_lora_data_tx_buffer + 9 + i * 7, &pos_msg.context.wifi_context[i].wifi_mac, 6 );
                    memcpy( app_lora_data_tx_buffer + 9 + i * 7 + 6, &pos_msg.context.wifi_context[i].cur_rssi, 1 );
                }
                app_lora_data_tx_size = 9 + pos_msg.context_count * 7;
            }
            break;

            case DATA_ID_UP_PACKET_BLE_RAW:
            {
                for( uint8_t i = 0; i < pos_msg.context_count; i++ )
                {
                    memcpy( app_lora_data_tx_buffer + 9 + i * 7, &pos_msg.context.beac_context[i].beac_mac, 6 );
                    memcpy( app_lora_data_tx_buffer + 9 + i * 7 + 6, &pos_msg.context.beac_context[i].cur_rssi, 1 );
                }
                app_lora_data_tx_size = 9 + pos_msg.context_count * 7;
            }
            break;

            case DATA_ID_UP_PACKET_GNSS_RAW:
            {
                app_lora_data_tx_buffer[1] = pos_msg.context.gps_context.zone_flag; // fragment data
                app_lora_data_tx_buffer[4] = pos_msg.context.gps_context.gnss_len; // GNSS raw lenght
                memcpyr( app_lora_data_tx_buffer + 2, ( uint8_t * )( &pos_msg.context.gps_context.group_id ), 2 ); // group id
                memcpy( app_lora_data_tx_buffer + 5, pos_msg.context.gps_context.gnss_res, pos_msg.context.gps_context.gnss_len ); // GNSS raw data
                app_lora_data_tx_size = 5 + pos_msg.context.gps_context.gnss_len;
            }
            break;

            case DATA_ID_UP_PACKET_GNSS_END:
            {
                app_lora_data_tx_buffer[9] = pos_msg.context.gps_context.zone_flag; // fragment data
                memcpyr( app_lora_data_tx_buffer + 11, ( uint8_t * )( &pos_msg.context.gps_context.group_id ), 2 ); // group id
                app_lora_data_tx_size = 12;
            }
            break;

            case DATA_ID_UP_PACKET_POS_STATUS:
            {
                app_lora_data_tx_size = app_lora_data_tx_size + 9;
            }
            break;
            case DATA_ID_UP_PACKET_USER_SENSOR:
            {
                memcpy( &app_lora_data_tx_buffer[1], ( uint8_t * )( pos_msg.context.sensor_context.sensor_data ), pos_msg.context.sensor_context.len );
                app_lora_data_tx_size = app_lora_data_tx_size + pos_msg.context.sensor_context.len+1;
            }
            break;
            case DATA_ID_UP_PACKET_FACT_SENSOR:
            {
                memcpy( &app_lora_data_tx_buffer[1], ( uint8_t * )( pos_msg.context.sensor_context.sensor_data ), pos_msg.context.sensor_context.len );
                app_lora_data_tx_size = app_lora_data_tx_size + pos_msg.context.sensor_context.len+1;
            }
            break;            
            default:
            {
                delete_pos_msg_datas( 1, true );
                return;
            }
            break;
        }

        hal_mcu_trace_print( "app data off-line: " );
        for( uint8_t i = 0; i < app_lora_data_tx_size; i++ )
        {
            hal_mcu_trace_print( "%02x ", app_lora_data_tx_buffer[i] );
        }
        hal_mcu_trace_print( "\r\n" );

        smtc_modem_get_status( 0, &modem_status );
        if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED 
            && app_lora_is_idle( ) 
            && app_task_radio_gnss_is_busy( ) == false && app_task_radio_wifi_is_busy( ) == false 
            && app_task_track_gnss_is_busy( ) == false && app_task_track_wifi_is_busy( ) == false )
        {

            bool result = app_lora_send_frame( app_lora_data_tx_buffer, app_lora_data_tx_size, true, false );
            if( result )
            {
                tx_last_timestemp = smtc_modem_hal_get_time_in_ms( );
                smtc_modem_get_toa_status( &app_task_lora_tx_toa,app_lora_data_tx_size + 13 );
                app_task_lora_tx_check = true;
                app_task_lora_tx_off_line = true;
                app_task_lora_tx_confirmed_count = app_lora_get_confirmed_count( );
            }
        }
    }
    else
    {
        hal_mcu_trace_print( "app read old pos data fail\r\n" );
    }
}
