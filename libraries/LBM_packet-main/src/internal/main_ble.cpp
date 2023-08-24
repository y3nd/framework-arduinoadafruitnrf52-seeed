#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

#include <bluefruit.h>



uint8_t ble_scan_status = 0;

uint8_t ble_beacon_res_num = 0;
BleBeacons_t ble_beacon_buf[BLE_BEACON_BUF_MAX] = { 0 };
uint8_t ble_beacon_rssi_array[BLE_BEACON_BUF_MAX] = { 0 };




bool buf_cmp_value( uint8_t *a, uint8_t *b, uint8_t len )
{
    for( uint8_t i = 0; i < len; i++ )
    {
        if( a[i] != b[i] )
        {
            return false;
        }
    }
    return true;
}


static void ble_scanner_evt_handler( ble_gap_evt_adv_report_t* report )
{
    PRINT_LOCATION();
    uint8_t len = 0;
    uint8_t buffer[32];
    memset(buffer, 0, sizeof(buffer));
    len = Bluefruit.Scanner.parseReportByType(report, BLE_GAP_AD_TYPE_MANUFACTURER_SPECIFIC_DATA, buffer, sizeof(buffer));
    if (len)    
    {
        uint8_t beacon_data_len = 0;
        uint8_t beacon_data_type = 0;
        uint16_t company_identifier = 0;
        beacon_data_len = buffer[3];
        beacon_data_type = buffer[2];
        memcpy(( uint8_t * )( &company_identifier ), buffer, 2 );
        if( beacon_data_type == BEACON_DATA_TYPE )
        {
            if( company_identifier == COMPANY_IDENTIFIER )
            {
                if( beacon_data_len == BEACON_DATA_LEN )
                {

                    bool res0 = true, res1 = true, res2 = true, res3 = true, res4 = true, res5 = true;
                    for( uint8_t j = 0; j < BLE_BEACON_BUF_MAX; j++ )
                    {
                        res1 = buf_cmp_value( ble_beacon_buf[j].uuid, ( uint8_t *)( buffer + 4 ), 16 );
                        res2 = buf_cmp_value(( uint8_t *)( &ble_beacon_buf[j].major ), ( uint8_t *)( buffer + 20 ), 2 );
                        res3 = buf_cmp_value(( uint8_t *)( &ble_beacon_buf[j].minor ), ( uint8_t *)( buffer + 22 ), 2 );
                        // res4 = buf_cmp_value(( uint8_t *)( &ble_beacon_buf[j].rssi ), report->rssi, 1 );
                        res5 = buf_cmp_value(( uint8_t *)( &ble_beacon_buf[j].mac ), ( uint8_t *)( report->peer_addr.addr ), 6 );
                        if( res1 && res2 && res3 && res4 && res5 ) // all is same, don't save the scan result
                        {
                            res0 = false;
                            break;
                        }
                    }
                    if( res0 )
                    {
                        if(( ble_beacon_res_num < BLE_BEACON_BUF_MAX ) && ( ble_beacon_buf[ble_beacon_res_num].company_id == 0 ))
                        {
                            uint32_t utc = 0;
                            // TODO - get wifi scan utc
                            utc = app_task_track_get_utc( );
                            ble_beacon_buf[ble_beacon_res_num].utc = utc;
                            ble_beacon_buf[ble_beacon_res_num].company_id = company_identifier;
                            memcpy( ble_beacon_buf[ble_beacon_res_num].uuid, buffer + 4, 16 );
                            memcpy(( uint8_t *)( &ble_beacon_buf[ble_beacon_res_num].major ), buffer + 20, 2 );
                            memcpy(( uint8_t *)( &ble_beacon_buf[ble_beacon_res_num].minor ), buffer + 22, 2 );
                            memcpy(( uint8_t *)( &ble_beacon_buf[ble_beacon_res_num].rssi ), buffer + 24, 1 );
                            memcpy(( uint8_t *)( &ble_beacon_buf[ble_beacon_res_num].rssi_ ), ( uint8_t *)( &report->rssi ), 1 );
                            memcpy(( uint8_t *)( &ble_beacon_buf[ble_beacon_res_num].mac ), ( uint8_t *)( report->peer_addr.addr ), 6 );
                            ble_beacon_res_num ++;
                        }   
                    }
                }
            }
        }
    }
    Bluefruit.Scanner.resume();
}

void scan_params_init(void)
{
    Bluefruit.begin(0, 1);
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

    /* Set the device name */
    Bluefruit.setName("wio tracker");

    /* Set the LED interval for blinky pattern on BLUE LED */
    Bluefruit.setConnLedInterval(250);
    /* Start Central Scanning
    * - Enable auto scan if disconnected
    * - Filter out packet with a min rssi
    * - Interval = 100 ms, window = 50 ms
    * - Use active scan (used to retrieve the optional scan response adv packet)
    * - Start(0) = will scan forever since no timeout is given
    */
    Bluefruit.Scanner.setRxCallback(ble_scanner_evt_handler);
    Bluefruit.Scanner.restartOnDisconnect(true);
    Bluefruit.Scanner.filterRssi(BLE_BEACON_FILTER);
    //Bluefruit.Scanner.filterUuid(BLEUART_UUID_SERVICE); // only invoke callback if detect bleuart service
    Bluefruit.Scanner.setInterval(APP_SCAN_INTERVAL, APP_SCAN_WINDOW);       // in units of 0.625 ms
    Bluefruit.Scanner.useActiveScan(true);        // Request scan response data
}

void app_ble_scan_init( void )
{
    if(ble_scan_status == 1)
    {
        return;
    }
    scan_params_init();
    ble_scan_status = 1;
}

bool app_ble_scan_start( void )
{
    if(ble_scan_status == 2)
    {
        return true;
    }
    app_task_track_time_stamp = app_task_track_get_utc( );
    for( uint8_t i = 0; i < BLE_BEACON_BUF_MAX; i++ )
    {
        memset(( uint8_t *)( &ble_beacon_buf[i] ), 0, sizeof( BleBeacons_t ));    
    }
    
    ble_beacon_res_num = 0;
    Bluefruit.Scanner.start(0);
    ble_scan_status = 2; 
    return true;
}
void app_ble_scan_stop( void )
{
    if(ble_scan_status == 3)
    {
        return;
    }
    Bluefruit.Scanner.stop();
    ble_scan_status = 3;
}

bool app_ble_get_results( uint8_t *result, uint8_t *size )
{
    uint8_t beacon_num = 0;
    int8_t rssi_max_index = 0;

    if( result && size )
    {
        if( ble_beacon_res_num == 0 ) return false;
        
        for( uint8_t i = 0; i < ble_beacon_res_num; i++ )
        {
            ble_beacon_rssi_array[i] = i;
        }

        for( uint8_t i = 0; i < ble_beacon_res_num; i++ )
        {
            for( uint8_t j = i; j < ble_beacon_res_num; j++ )
            {
                if( ble_beacon_buf[ble_beacon_rssi_array[i]].rssi_ < ble_beacon_buf[ble_beacon_rssi_array[j]].rssi_ )
                {
                    rssi_max_index = ble_beacon_rssi_array[i];
                    ble_beacon_rssi_array[i] = ble_beacon_rssi_array[j];
                    ble_beacon_rssi_array[j] = rssi_max_index;
                }
            }
        }

        memcpyr( result, ( uint8_t * )( &ble_beacon_buf[0].utc ), 4 );
        *size += 4;

        if( ble_beacon_res_num > BLE_BEACON_SEND_MUM )
        {
            beacon_num = BLE_BEACON_SEND_MUM;
        }
        else
        {
            beacon_num = ble_beacon_res_num;
        }

        for( uint8_t i = 0; i < beacon_num; i ++ )
        {
            memcpyr( result + 4 + i * 7, ( uint8_t *)( &ble_beacon_buf[ble_beacon_rssi_array[i]].mac ), 6 );
            memcpy( result + 4 + i * 7 + 6, &ble_beacon_buf[ble_beacon_rssi_array[i]].rssi_, 1 );
            *size += 7;
        }

        if( beacon_num < BLE_BEACON_SEND_MUM )
        {
            for( uint8_t i = 0; i < ( BLE_BEACON_SEND_MUM - beacon_num ); i++ )
            {
                memset( result + 4 + beacon_num * 7 + i * 7, 0xff, 7 );
                *size += 7;
            }
        }
        
        if( beacon_num ) return true;
        else return false;
    }
    return false;
}

void app_ble_display_results( void )
{
    hal_mcu_trace_print( "iBeacon: %d\r\n", ble_beacon_res_num );
    for( uint8_t i = 0; i < ble_beacon_res_num; i ++ )
    {
        hal_mcu_trace_print("%04x, ", ble_beacon_buf[ble_beacon_rssi_array[i]].company_id );
        for( uint8_t j = 0; j < 16; j++ )
        {
            hal_mcu_trace_print( "%02x ", ble_beacon_buf[ble_beacon_rssi_array[i]].uuid[j] );
        }

        uint16_t major = 0, minor = 0;
        memcpyr(( uint8_t *)( &major ), ( uint8_t *)( &ble_beacon_buf[ble_beacon_rssi_array[i]].major ), 2 );
        memcpyr(( uint8_t *)( &minor ), ( uint8_t *)( &ble_beacon_buf[ble_beacon_rssi_array[i]].minor ), 2 );
        hal_mcu_trace_print(", %04x, ", major );
        hal_mcu_trace_print("%04x, ", minor );
        hal_mcu_trace_print("%d/%d dBm, ", ble_beacon_buf[ble_beacon_rssi_array[i]].rssi, ble_beacon_buf[ble_beacon_rssi_array[i]].rssi_ );

        for( uint8_t j = 0; j < 5; j++ )
        {
            hal_mcu_trace_print( "%02x:", ble_beacon_buf[ble_beacon_rssi_array[i]].mac[5 - j] );
        }
        hal_mcu_trace_print( "%02x\r\n", ble_beacon_buf[ble_beacon_rssi_array[i]].mac[0] );
    }
}


