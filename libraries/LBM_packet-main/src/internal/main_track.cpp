#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>


#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>
#include <Adafruit_TinyUSB.h> // for Serial

using namespace Adafruit_LittleFS_Namespace;

#define FILENAME    "/group_id.txt"


static lfs_t *lfs;
static lfs_file_t file;



uint8_t tracker_gps_scan_len = 0;
uint8_t tracker_gps_scan_data[64] = { 0 };

uint8_t tracker_wifi_scan_len = 0;
uint8_t tracker_wifi_scan_data[64] = { 0 };

uint8_t tracker_ble_scan_len = 0;
uint8_t tracker_ble_scan_data[64] = { 0 };

uint8_t tracker_scan_temp_len = 0;
uint8_t tracker_scan_data_temp[64] = { 0 };

uint8_t tracker_scan_type = TRACKER_SCAN_BLE_WIFI_GPS; // 0: gps, 1: wifi, 2: ble, 3: gps/wifi, 4: gps/ble, 5: gps/wifi/ble
uint8_t tracker_sensor_en = TRACKER_SENSOR_ON; // 0: sensor data off, 1: sensor data on


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


void app_task_track_set_scan_state( uint32_t bit )
{
    state_all |= bit;
}

void app_task_track_clear_scan_state( uint32_t bit )
{
    state_all &= ( ~bit );
}

void app_task_track_motion_index_update( void )
{
    motion_index = motion_index_backup + 1;
}

void app_task_track_motion_index_clear( void )
{
    motion_index_backup = motion_index;
    motion_index = 0;
}

void app_task_track_scan_send( void )
{
    int16_t temp = 0;
    uint16_t light = 0;
    int8_t battery = 0;
    uint8_t motion_index_temp = 0;

    memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
    tracker_scan_temp_len = 0;

    if(( state_all & TRACKER_STATE_BIT3_MOT_END ) == TRACKER_STATE_BIT3_MOT_END )
    {
        motion_index_temp = motion_index_backup;
    }
    else
    {
        motion_index_temp = motion_index;
    }

#if TRACKER_DATA_SEND
    
    if( gnss_mw_custom_send_buffer_num == 0 && tracker_wifi_scan_len == 0 && tracker_ble_scan_len == 0 )
    {
        tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_POS_STATUS;

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

        tracker_scan_data_temp[2] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[4] = ( state_all ) & 0xff;
        tracker_scan_temp_len += 5;
        // uint32_t utc_temp = app_task_track_get_utc( );
        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &utc_temp ), 4 );
        tracker_scan_temp_len += 4;
        if( tracker_sensor_en )
        {
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &temperture_val ), 2 );
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len + 2, ( uint8_t * )( &light_val ), 2 );
            tracker_scan_temp_len += 4;
        }
        else
        {
            uint16_t data_temp = 0x8000;
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &data_temp ), 2 );
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len + 2, ( uint8_t * )( &data_temp ), 2 );
            tracker_scan_temp_len += 4;
        }
        memcpy( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &battery ), 1 );
        tracker_scan_temp_len += 1;

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
    }

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
        if( tracker_sensor_en )
        {
            for( uint8_t i = 0; i < gnss_raw_num; i++ )
            {
                memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
                tracker_scan_temp_len = 0;

                tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_RAW;
                tracker_scan_temp_len += 1;

                tracker_scan_data_temp[1] = ( fragment_total << 4 ) + i; // fragment data
                tracker_scan_temp_len += 1;

                memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); // group id
                tracker_scan_temp_len += 2;

                tracker_scan_data_temp[4] = gnss_mw_custom_send_buffer_len[len_array[i]]; // GNSS raw lenght
                tracker_scan_temp_len += 1;

                memcpy( tracker_scan_data_temp + tracker_scan_temp_len, gnss_mw_custom_send_buffer[len_array[i]], gnss_mw_custom_send_buffer_len[len_array[i]] );
                tracker_scan_temp_len += gnss_mw_custom_send_buffer_len[len_array[i]];

                app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
            }

            memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
            tracker_scan_temp_len = 0;

            tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_SEN_BAT;
            tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
            tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
            tracker_scan_data_temp[3] = ( state_all ) & 0xff;
            tracker_scan_data_temp[4] = motion_index_temp;
            tracker_scan_temp_len += 5;

            // uint32_t utc_temp = app_task_track_get_utc( );
            uint32_t utc_temp = app_task_track_time_stamp;
            memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );
            tracker_scan_temp_len += 4;

            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &temperture_val ), 2 );
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len + 2, ( uint8_t * )( &light_val ), 2 );
            tracker_scan_temp_len += 4;

            memcpy( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &battery ), 1 );
            tracker_scan_temp_len += 1;

            tracker_scan_data_temp[tracker_scan_temp_len] = ( fragment_total << 4 ) + gnss_raw_num; // fragment data
            tracker_scan_temp_len += 1;

            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); // group id
            tracker_scan_temp_len += 2;

            app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
        }
        else
        {
            for( uint8_t i = 0; i < gnss_raw_num; i++ )
            {
                memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
                tracker_scan_temp_len = 0;

                tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_RAW;
                tracker_scan_temp_len += 1;

                tracker_scan_data_temp[1] = ( fragment_total << 4 ) + i; // fragment data
                tracker_scan_temp_len += 1;

                memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); // group id
                tracker_scan_temp_len += 2;

                tracker_scan_data_temp[4] = gnss_mw_custom_send_buffer_len[len_array[i]];
                tracker_scan_temp_len += 1;

                memcpy( tracker_scan_data_temp + tracker_scan_temp_len, gnss_mw_custom_send_buffer[len_array[i]], gnss_mw_custom_send_buffer_len[len_array[i]] );
                tracker_scan_temp_len += gnss_mw_custom_send_buffer_len[len_array[i]];

                app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
            }

            memset( tracker_scan_data_temp, 0, sizeof( tracker_scan_data_temp ));
            tracker_scan_temp_len = 0;

            tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_GNSS_BAT;
            tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
            tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
            tracker_scan_data_temp[3] = ( state_all ) & 0xff;
            tracker_scan_data_temp[4] = motion_index_temp;
            tracker_scan_temp_len += 5;

            // uint32_t utc_temp = app_task_track_get_utc( );
            uint32_t utc_temp = app_task_track_time_stamp;
            memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );
            tracker_scan_temp_len += 4;

            memcpy( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &battery ), 1 );
            tracker_scan_temp_len += 1;

            tracker_scan_data_temp[tracker_scan_temp_len] = ( fragment_total << 4 ) + gnss_raw_num; // fragment data
            tracker_scan_temp_len += 1;

            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &track_gnss_group_id ), 2 ); // group id
            tracker_scan_temp_len += 2;
            
            app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
        }
        
        gnss_mw_custom_send_buffer_num = 0;
        for( uint8_t i = 0; i < GNSS_SCAN_GROUP_SIZE_MAX; i++ ) gnss_mw_custom_send_buffer_len[i] = 0;
        hal_mcu_trace_print( "GNSS raw group id: 0x%04x\r\n", track_gnss_group_id );
        track_gnss_group_id ++;
        if(!gnss_group_id_write())
        {
            hal_mcu_trace_print( "save gnss raw group id error\r\n" );
        }
    }
#endif

#if TRACKER_WIFI_EN
    if( tracker_wifi_scan_len )
    {
        if( tracker_sensor_en ) tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_WIFI_SEN_BAT;
        else tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_WIFI_BAT;

        tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all ) & 0xff;
        tracker_scan_data_temp[4] = motion_index_temp;

        memcpy( tracker_scan_data_temp + 5, tracker_wifi_scan_data, tracker_wifi_scan_len ); // utc + wifi
        tracker_scan_temp_len = tracker_wifi_scan_len + 5;

        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );

        if( tracker_sensor_en )
        {
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &temperture_val ), 2 );
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len + 2, ( uint8_t * )( &light_val ), 2 );
            tracker_scan_temp_len += 4;
        }

        memcpy( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &battery ), 1 );
        tracker_scan_temp_len += 1;

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
        tracker_wifi_scan_len = 0;
    }
#endif

#if TRACKER_BLE_EN
    if( tracker_ble_scan_len )
    {
        if( tracker_sensor_en ) tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_BLE_SEN_BAT;
        else tracker_scan_data_temp[0] = DATA_ID_UP_PACKET_BLE_BAT;

        tracker_scan_data_temp[1] = ( state_all >> 16 ) & 0xff;
        tracker_scan_data_temp[2] = ( state_all >> 8 ) & 0xff;
        tracker_scan_data_temp[3] = ( state_all ) & 0xff;
        tracker_scan_data_temp[4] = motion_index_temp;

        memcpy( tracker_scan_data_temp + 5, tracker_ble_scan_data, tracker_ble_scan_len ); // utc + ble
        tracker_scan_temp_len = tracker_ble_scan_len + 5;

        uint32_t utc_temp = app_task_track_time_stamp;
        memcpyr( tracker_scan_data_temp + 5, ( uint8_t * )( &utc_temp ), 4 );

        if( tracker_sensor_en )
        {
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &temperture_val ), 2 );
            memcpyr( tracker_scan_data_temp + tracker_scan_temp_len + 2, ( uint8_t * )( &light_val ), 2 );
            tracker_scan_temp_len += 4;
        }

        memcpy( tracker_scan_data_temp + tracker_scan_temp_len, ( uint8_t * )( &battery ), 1 );
        tracker_scan_temp_len += 1;

        app_task_lora_tx_queue( tracker_scan_data_temp, tracker_scan_temp_len, false, false );
        tracker_ble_scan_len = 0;

    }
    state_all = 0;
#endif
#endif
}


void app_task_lora_load_utc_by_cache( void )
{
    bool result = false;

    if( result )
    {
        app_gps_lock_in_utc_second = app_utc_param.sync_utc;
        hal_mcu_trace_print( "app load old utc ok: %u\r\n", app_gps_lock_in_utc_second );
    }
    else
    {
        hal_mcu_trace_print( "app load old utc fail\r\n" );
    }
}

bool app_task_track_gnss_is_busy( void )
{
    return track_gnss_busy;
}


bool app_task_track_wifi_is_busy( void )
{
    return track_wifi_busy;
}

void gnss_group_id_init(void)
{
    uint32_t group_id_temp;
    uint8_t len = sizeof(group_id_temp);
    int ret = 0;
    InternalFS.begin();
    lfs = InternalFS._getFS();

    ret = lfs_file_open(lfs, &file, FILENAME, LFS_O_RDONLY);
    // file existed
    if ( ret == 0 )
    {
        uint8_t readlen;
        len = lfs_file_read(lfs, &file, &group_id_temp, len);
        lfs_file_close(lfs, &file);
        track_gnss_group_id = group_id_temp;
    }
    else
    {
        ret = lfs_file_open(lfs, &file, FILENAME, LFS_O_RDWR | LFS_O_CREAT);
        if( ret == 0 )
        {
            group_id_temp = track_gnss_group_id;
            lfs_file_rewind(lfs, &file);
            lfs_file_write(lfs, &file, &group_id_temp, sizeof(group_id_temp));
            lfs_file_close(lfs, &file);
        }
    }    
}
bool gnss_group_id_write(void)
{
    uint32_t group_id_temp;
    int ret = 0;

    group_id_temp = track_gnss_group_id;
    ret = lfs_file_open(lfs, &file, FILENAME, LFS_O_RDWR | LFS_O_CREAT);
    if(ret != 0)
    {
        lfs_file_close(lfs, &file); 
        return false;   
    }
    ret = lfs_file_read(lfs, &file, &group_id_temp, sizeof(group_id_temp));

    ret = lfs_file_rewind(lfs, &file);
    ret = lfs_file_write(lfs, &file, &group_id_temp, sizeof(group_id_temp));
    lfs_file_close(lfs, &file);
    return true;
}
