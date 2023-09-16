#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

uint8_t app_gnss_scan_mode = 1; // 0: GNSS_MW_MODE_STATIC, 1: GNSS_MW_MODE_MOBILE

uint32_t app_gps_lock_in_rtc_second = 0;
uint32_t app_gps_lock_in_utc_second = 0;

uint8_t  gps_scan_status = 0;


void app_gps_scan_init( void )
{
    gps_scan_status = 1;
    // gnss_group_id_init();
}

bool app_gps_scan_start( void )
{
    mw_return_code_t gnss_rc;
    gnss_mw_custom_send_buffer_num = 0;
    track_gnss_busy = true;
    app_task_track_time_stamp = app_task_track_get_utc( );
    for( uint8_t i = 0; i < GNSS_SCAN_GROUP_SIZE_MAX; i++ )
    {
        gnss_mw_custom_send_buffer_len[i] = 0;
        memset( gnss_mw_custom_send_buffer[i], 0, 64 );
    }

    if( app_task_radio_gnss_is_busy(  )) // GNSS scan is doing
    {
        gps_scan_status = 2;
        return true;
    }
    else if( app_lora_is_idle( ) && app_task_radio_wifi_is_busy( ) == false )
    {
        gnss_rc = gnss_mw_scan_start( (gnss_mw_mode_t)app_gnss_scan_mode, 0 ); /* start ASAP */
        if( gnss_rc == MW_RC_OK )
        {
            gps_scan_status = 2;
            return true;
        }
        else
        {
            hal_mcu_trace_print( "Failed to start GNSS scan\n" );
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool app_gps_get_results( uint8_t *buf, uint8_t *size )
{
    if( gnss_mw_custom_send_buffer_num )
    {
        return true;
    }
    else
    {
        return false;
    }
}

void app_gps_scan_stop( void )
{
    gnss_mw_scan_cancel( );
    track_gnss_busy = false;
    gps_scan_status = 3;
}

void app_gps_display_results( void )
{
    if( gnss_mw_custom_send_buffer_num )
    {
        printf( "GNSS Raw:\r\n" );
    }
    for( uint8_t i = 0; i < gnss_mw_custom_send_buffer_num; i++ )
    {
        for( uint8_t j = 0; j < gnss_mw_custom_send_buffer_len[i]; j++ )
        {
            printf( "%02X", gnss_mw_custom_send_buffer[i][j] );
        }
        printf( "\r\n" );
    }
}
