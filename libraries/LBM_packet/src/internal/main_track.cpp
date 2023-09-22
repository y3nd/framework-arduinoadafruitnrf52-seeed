#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

uint8_t tracker_gps_scan_len = 0;
uint8_t tracker_gps_scan_data[64] = { 0 };

uint8_t tracker_wifi_scan_len = 0;
uint8_t tracker_wifi_scan_data[64] = { 0 };

uint8_t tracker_ble_scan_len = 0;
uint8_t tracker_ble_scan_data[64] = { 0 };

uint8_t tracker_scan_temp_len = 0;
uint8_t tracker_scan_data_temp[64] = { 0 };

uint8_t tracker_scan_type = TRACKER_SCAN_BLE_WIFI_GPS; // 0: gps, 1: wifi, 2: ble, 3: gps/wifi, 4: gps/ble, 5: gps/wifi/ble


uint32_t state_all = 0;
uint8_t motion_index = 0;
uint8_t motion_index_backup = 0;

bool track_gnss_busy = false;
bool track_wifi_busy = false;
uint16_t track_gnss_group_id = 0;
uint32_t app_task_track_time_stamp = 0;



uint32_t app_task_track_get_utc( void )
{
    uint32_t utc_lns = 0, utc_ble = 0;
    utc_lns = app_task_lora_get_timestamp( );  
    if( utc_lns ) return utc_lns;  
    else return smtc_modem_hal_get_time_in_s( );
}

void app_task_track_motion_index_update( void )
{
    motion_index = motion_index_backup + 1;
}

//track-------------------------------------------------
void app_task_track_scan_send( void )
{
    uint8_t motion_index_temp = 0;

    memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
    tracker_scan_temp_len = 0;

    motion_index_temp = motion_index;

    //Location failure
    if( gnss_mw_custom_send_buffer_num == 0 && tracker_wifi_scan_len == 0 && tracker_ble_scan_len == 0 )
    {
        tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_POS_STATUS;

        //Location failure type
        if(( app_task_lora_clock_is_synch( ) == false ) || ( mw_gnss_event_state == GNSS_MW_EVENT_ERROR_NO_TIME ))
        {
            tracker_scan_data_temp[1] = 13; // sync time fail
        }
        else if( mw_gnss_event_state == GNSS_MW_EVENT_ERROR_ALMANAC_UPDATE )
        {
            tracker_scan_data_temp[1] = 14; // almanac too old
        }
        else
        {
            tracker_scan_data_temp[1] = tracker_scan_type + 1;
        }

        //event  status
        tracker_scan_data_temp[2] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[4] = ( state_all ) & 0xff;
        tracker_scan_temp_len += 5;

        //utc time
        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &utc_temp ), 4 );
        tracker_scan_temp_len += 4;

        hal_mcu_trace_print("Location failure uplink:");
        for(uint8_t u8i = 0; u8i < tracker_scan_temp_len; u8i++ )
        {
            hal_mcu_trace_print("%02x",tracker_scan_data_temp[u8i]);
        }
        hal_mcu_trace_print("\r\n"); 

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, true, false );

    }

    //Location success
#if TRACKER_GPS_EN
    if( gnss_mw_custom_send_buffer_num )
    {
        uint8_t gnss_raw_num = 0;
        uint8_t len_max_index = 0;
        uint8_t len_array[GNSS_SCAN_GROUP_SIZE_MAX] = { 0 };

        for( uint8_t i = 0; i < gnss_mw_custom_send_buffer_num; i++ )
        {
            gnss_mw_custom_send_buffer[i][0] &= 0x7f; // clear all byte[0] high bit7
            len_array[i] = i;
        }

        for( uint8_t i = 0; i < gnss_mw_custom_send_buffer_num; i++ )
        {
            for( uint8_t j = i; j < gnss_mw_custom_send_buffer_num; j++ )
            {
                if( gnss_mw_custom_send_buffer_len[len_array[i]] < gnss_mw_custom_send_buffer_len[len_array[j]] )
                {
                    len_max_index = len_array[i];
                    len_array[i] = len_array[j];
                    len_array[j] = len_max_index;
                }
            }
        }

        if( gnss_mw_custom_send_buffer_num > GNSS_RAW_SCAN_SEND_MAX )
        {
            gnss_raw_num = GNSS_RAW_SCAN_SEND_MAX;
        }
        else
        {
            gnss_raw_num = gnss_mw_custom_send_buffer_num;
        }

        gnss_mw_custom_send_buffer[len_array[gnss_raw_num - 1]][0] |= 0x80; // set last raw byte[0] high bit7

        uint8_t fragment_total = gnss_raw_num + 1;

        //gnss datas need to subcontract delivery
        //these are gnss datas package
        for( uint8_t i = 0; i < gnss_raw_num; i++ ) 
        {
            memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
            tracker_scan_temp_len = 0;

            tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_RAW;
            tracker_scan_temp_len += 1;

            // fragment data,package num 
            tracker_scan_data_temp[1] = ( fragment_total << 4 ) + i; // fragment data,package num 
            tracker_scan_temp_len += 1;

            //group id
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); // group id
            tracker_scan_temp_len += 2;

            //raw data len
            tracker_scan_data_temp[4] = gnss_mw_custom_send_buffer_len[len_array[i]];
            tracker_scan_temp_len += 1;

            //raw data            
            memcpy( tracker_scan_data_temp + tracker_scan_temp_len, gnss_mw_custom_send_buffer[len_array[i]], gnss_mw_custom_send_buffer_len[len_array[i]] );
            tracker_scan_temp_len += gnss_mw_custom_send_buffer_len[len_array[i]];
            hal_mcu_trace_print("gnss uplink:");
            for(uint8_t u8i = 0; u8i < tracker_scan_temp_len; u8i++ )
            {
                hal_mcu_trace_print("%02x",tracker_scan_data_temp[u8i]);
            }
            hal_mcu_trace_print("\r\n"); 
            app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, true, false );
        }

        //this is a gnss datas end package
        memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
        tracker_scan_temp_len = 0;

        tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_END;

        //event  status
        tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all ) & 0xff;

        //motion index        
        tracker_scan_data_temp[4] = motion_index_temp;
        tracker_scan_temp_len += 5;

        //utc time
        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );
        tracker_scan_temp_len += 4;

        //fragment data
        tracker_scan_data_temp[tracker_scan_temp_len] = ( fragment_total << 4 ) + gnss_raw_num; 
        tracker_scan_temp_len += 1;

        //group id
        memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); 
        tracker_scan_temp_len += 2;


        hal_mcu_trace_print("gnss uplink:");
        for(uint8_t u8i = 0; u8i < tracker_scan_temp_len; u8i++ )
        {
            hal_mcu_trace_print("%02x",tracker_scan_data_temp[u8i]);
        }
        hal_mcu_trace_print("\r\n");      

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, true, false );
        
        //clear buffer
        gnss_mw_custom_send_buffer_num = 0;
        for( uint8_t i = 0; i < GNSS_SCAN_GROUP_SIZE_MAX; i++ ) gnss_mw_custom_send_buffer_len[i] = 0;
            hal_mcu_trace_print( "GNSS raw group id: 0x%04x\r\n", track_gnss_group_id );
        track_gnss_group_id ++;
    }
#endif

#if TRACKER_WIFI_EN
    if( tracker_wifi_scan_len )
    {
        tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_WIFI_RAW;

        //event  status
        tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all ) & 0xff;

        //motion index  
        tracker_scan_data_temp[4] = motion_index_temp;

        // utc + wifi
        memcpy( tracker_scan_data_temp + 5, tracker_wifi_scan_data, tracker_wifi_scan_len ); // utc + wifi
        tracker_scan_temp_len = tracker_wifi_scan_len + 5;

        //utc time,the previous UTC time is overwritten here(utc + wifi)
        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );

        hal_mcu_trace_print("wifi uplink:");
        for(uint8_t u8i = 0; u8i < tracker_scan_temp_len; u8i++ )
        {
            hal_mcu_trace_print("%02x",tracker_scan_data_temp[u8i]);
        }
        hal_mcu_trace_print("\r\n"); 
        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, true, false );
        tracker_wifi_scan_len = 0;
    }
#endif

#if TRACKER_BLE_EN
    if( tracker_ble_scan_len )
    {
        tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_BLE_RAW;

        //event  status
        tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all ) & 0xff;

        //motion index 
        tracker_scan_data_temp[4] = motion_index_temp;

        // utc + ble
        memcpy( tracker_scan_data_temp + 5, tracker_ble_scan_data, tracker_ble_scan_len ); 
        tracker_scan_temp_len = tracker_ble_scan_len + 5;

        //utc time,the previous UTC time is overwritten here(utc + wifi)
        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );

        hal_mcu_trace_print("ble uplink:");
        for(uint8_t u8i = 0; u8i < tracker_scan_temp_len; u8i++ )
        {
            hal_mcu_trace_print("%02x",tracker_scan_data_temp[u8i]);
        }
        hal_mcu_trace_print("\r\n"); 

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, true, false );
        tracker_ble_scan_len = 0;

    }
    state_all = 0;
#endif

}


bool app_task_track_gnss_is_busy( void )
{
    return track_gnss_busy;
}


bool app_task_track_wifi_is_busy( void )
{
    return track_wifi_busy;
}

void track_scan_type_set( uint8_t scan_type )
{
    switch(scan_type)
    {
        case TRACKER_SCAN_GPS:
            tracker_scan_type = TRACKER_SCAN_GPS;
            break;
        case TRACKER_SCAN_WIFI:
            tracker_scan_type = TRACKER_SCAN_WIFI;
            break;
        case TRACKER_SCAN_BLE:
            tracker_scan_type = TRACKER_SCAN_BLE;
            break;
        case TRACKER_SCAN_BLE_WIFI_GPS:
            tracker_scan_type = TRACKER_SCAN_BLE_WIFI_GPS;
            break;
        default:
            tracker_scan_type = TRACKER_SCAN_GPS;
    }
}


