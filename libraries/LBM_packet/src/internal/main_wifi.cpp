#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>


uint8_t wifi_result_buffer[4+( WIFI_AP_RSSI_SIZE + WIFI_AP_ADDRESS_SIZE ) * WIFI_MAX_RESULTS + 4];

uint8_t wifi_scan_status = 0;   //0:none  1:init 2:start 3:stop

void app_wifi_scan_init()
{
    wifi_scan_status = 1;
}

bool app_wifi_scan_start( void )
{
    mw_return_code_t wifi_rc;
    app_task_track_time_stamp = app_task_track_get_utc( );
    track_wifi_busy = true;  

    if( app_task_radio_wifi_is_busy(  )) // Wifi scan is doing
    {
        wifi_scan_status = 2;
        return true;
    }
    else if( app_lora_is_idle( ) && app_task_radio_gnss_is_busy( ) == false )
    {
        wifi_rc = wifi_mw_scan_start( 0 );
        if( wifi_rc == MW_RC_OK )
        {
            wifi_scan_status = 2;
            return true;
        }
        else
        {
            hal_mcu_trace_print( "Failed to start WiFi scan\n" );
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool app_wifi_get_results( uint8_t* result, uint8_t *size )
{
    uint32_t utc = 0;

    if( wifi_results.nbr_results )
    {
        uint8_t wifi_buffer_size = 0;

        // TODO - get wifi scan utc
        utc = app_task_track_get_utc( );

        memcpyr( wifi_result_buffer, ( uint8_t * )( &utc ), 4 );
        wifi_buffer_size += 4;

        /* Concatenate all results in send buffer */
        for( uint8_t i = 0; i < wifi_results.nbr_results; i++ )
        {
            /* Copy Access Point MAC address in result buffer */
            memcpy( &wifi_result_buffer[wifi_buffer_size], wifi_results.results[i].mac_address, WIFI_AP_ADDRESS_SIZE );
            wifi_buffer_size += WIFI_AP_ADDRESS_SIZE;

#if WIFI_AP_RSSI_EN
            /* Copy Access Point RSSI address in result buffer (if requested) */
            wifi_result_buffer[wifi_buffer_size] = wifi_results.results[i].rssi;
            wifi_buffer_size += WIFI_AP_RSSI_SIZE;
#endif
        }

        if( wifi_results.nbr_results < WIFI_MAX_RESULTS )
        {
            for( uint8_t i = 0; i < ( WIFI_MAX_RESULTS - wifi_results.nbr_results ); i++  )
            {
                memset( &wifi_result_buffer[wifi_buffer_size], 0xff, ( WIFI_AP_ADDRESS_SIZE + WIFI_AP_RSSI_SIZE ));
                wifi_buffer_size += ( WIFI_AP_ADDRESS_SIZE + WIFI_AP_RSSI_SIZE );
            }
        }
        
        if( result ) memcpy( result, wifi_result_buffer, wifi_buffer_size );
        if( size ) memcpy( size, &wifi_buffer_size, 1 );

        return true;
    }
    else
    {
        return false;
    }
}

void app_wifi_display_results( void )
{
    printf( "Number of results: %u\r\n", wifi_results.nbr_results );
    for( uint8_t i = 0; i < wifi_results.nbr_results; i++ )
    {
        for( uint8_t j = 0; j < WIFI_AP_ADDRESS_SIZE; j++ )
        {
            printf( "%02X ", wifi_results.results[i].mac_address[j] );
        }
        printf( "\r\n");
        printf( "Channel: %d, ", wifi_results.results[i].channel );
        printf( "Type: %d, ", wifi_results.results[i].type );
        printf( "RSSI: %d\r\n", wifi_results.results[i].rssi );
    }
}

void app_wifi_scan_stop( void )
{
    wifi_mw_scan_cancel( );
    track_wifi_busy = false;
    wifi_scan_status = 3;
}
