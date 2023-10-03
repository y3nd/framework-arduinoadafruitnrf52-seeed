#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

#include <bluefruit.h>

uint8_t ble_scan_status = 0;    

uint8_t ble_beacon_res_num = 0; 
BleBeacons_t ble_beacon_buf[BLE_BEACON_BUF_MAX] = { 0 };
uint8_t ble_beacon_rssi_array[BLE_BEACON_BUF_MAX] = { 0 };


// BLE adv Service
const uint16_t TRACKER_UUID1 = 0x2886;
const uint16_t TRACKER_UUID2 = 0xA886;

BLEUuid uuid1 = BLEUuid(TRACKER_UUID1);
BLEUuid uuid2 = BLEUuid(TRACKER_UUID2);
BLEComm blecomm;

volatile uint8_t ble_connect_status = 0;    //0: none  1:init 2:connect 3:disconnect

volatile bool notify_status = false;
char ble_rec_data_buf[244] = {0};
uint8_t ble_rec_data_len = 0;
volatile bool ble_rec_done = false;
static char  ble_tx_buf[244];



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
    char adv_device_name[24] = {0};
    hexTonum((unsigned char *)adv_device_name, app_param.hardware_info.Sn, 9);

    uint8_t templen = strlen("-1110");
    
    memcpy(&adv_device_name[18],"-1110",templen); 

    Bluefruit.begin(0, 1);
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values

    /* Set the device name */
    Bluefruit.setName(adv_device_name);

    /* Set the LED interval for blinky pattern on BLUE LED */
    // Bluefruit.setConnLedInterval(250);
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
    printf( "iBeacon: %d\r\n", ble_beacon_res_num );
    for( uint8_t i = 0; i < ble_beacon_res_num; i ++ )
    {
        printf("%04x, ", ble_beacon_buf[ble_beacon_rssi_array[i]].company_id );
        for( uint8_t j = 0; j < 16; j++ )
        {
            printf( "%02x ", ble_beacon_buf[ble_beacon_rssi_array[i]].uuid[j] );
        }

        uint16_t major = 0, minor = 0;
        memcpyr(( uint8_t *)( &major ), ( uint8_t *)( &ble_beacon_buf[ble_beacon_rssi_array[i]].major ), 2 );
        memcpyr(( uint8_t *)( &minor ), ( uint8_t *)( &ble_beacon_buf[ble_beacon_rssi_array[i]].minor ), 2 );
        printf(", %04x, ", major );
        printf("%04x, ", minor );
        printf("%d/%d dBm, ", ble_beacon_buf[ble_beacon_rssi_array[i]].rssi, ble_beacon_buf[ble_beacon_rssi_array[i]].rssi_ );

        for( uint8_t j = 0; j < 5; j++ )
        {
            printf( "%02x:", ble_beacon_buf[ble_beacon_rssi_array[i]].mac[5 - j] );
        }
        printf( "%02x\r\n", ble_beacon_buf[ble_beacon_rssi_array[i]].mac[0] );
    }
}


void app_ble_adv_init( void )
{
    adv_params_init();
}


void adv_params_init(void)
{
    char adv_device_name[24] = {0};
    hexTonum((unsigned char *)adv_device_name, app_param.hardware_info.Sn, 9);

    uint8_t templen = strlen("-1110");

    memcpy(&adv_device_name[18],"-1110",templen); 

    // Config the peripheral connection with maximum bandwidth 
    // more SRAM required by SoftDevice
    // Note: All config***() function must be called before begin()
    Bluefruit.configPrphBandwidth(BANDWIDTH_MAX);

    Bluefruit.begin();
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
    Bluefruit.setName(adv_device_name);


    Bluefruit.Periph.setConnectCallback(connect_callback);
    Bluefruit.Periph.setDisconnectCallback(disconnect_callback);

    // Configure and Start BLE Uart Service
    blecomm.begin();

    // Set up and start advertising
    startAdv();
    memset(ble_rec_data_buf,0,244);
    ble_rec_data_len = 0;
    ble_rec_done = false;

    ble_connect_status = 1;
}

void startAdv(void)
{

    Bluefruit.Advertising.addUuid(uuid1,uuid2);
    Bluefruit.Advertising.addName();

    /* Start Advertising
    * - Enable auto advertising if disconnected
    * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
    * - Timeout for fast mode is 30 seconds
    * - Start(timeout) with timeout = 0 will advertise forever (until connected)
    * 
    * For recommended advertising interval
    * https://developer.apple.com/library/content/qa/qa1931/_index.html   
    */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
}

// callback invoked when central connects
void connect_callback(uint16_t conn_handle)
{
    // Get the reference to current connection
    BLEConnection* connection = Bluefruit.Connection(conn_handle);
    char central_name[32] = { 0 };
    ble_gap_addr_t ble_gap_addr;
    ble_gap_addr = connection->getPeerAddr(); 
    connection->getPeerName(central_name, sizeof(central_name));
    hal_mcu_trace_print("Connected to %s\r\n",central_name);
    hal_mcu_trace_print("mac:%02x:%02x:%02x:%02x:%02x:%02x\r\n",ble_gap_addr.addr[5],ble_gap_addr.addr[4],ble_gap_addr.addr[3],\
                                            ble_gap_addr.addr[2],ble_gap_addr.addr[1],ble_gap_addr.addr[0]);

    // request to update data length
    connection->requestDataLengthUpdate();

    // // request mtu exchange
    // connection->requestMtuExchange(247);

    ble_connect_status = 2;                   
}

bool bleMtuExchange(void)
{
    BLEConnection* connection = Bluefruit.Connection(0);
    // request to update data length
    connection->requestDataLengthUpdate();

    // request to update data length
    bool ret = connection->requestDataLengthUpdate();
    return ret;
}



/**
 * Callback invoked when a connection is dropped
 * @param conn_handle connection where this event happens
 * @param reason is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason)
{
    (void) conn_handle;
    (void) reason;

    hal_mcu_trace_print("Disconnected, reason = 0x%0x",reason);
    ble_connect_status = 3;    
}


static void vprint(const char *fmt, va_list argp) 
{
    uint8_t ble_tx_len = 0;
    
    notify_status = blecomm.notifyEnabled();
    if(notify_status == true)
    {
        if (0 < vsprintf(ble_tx_buf, fmt, argp)) // build string
        {
            ble_tx_len = strlen(ble_tx_buf);
            blecomm.write( ble_tx_buf, ble_tx_len );
        }    
    }        
}

void hal_ble_trace_print(const char *fmt, ...) 
{
    va_list argp;
    va_start(argp, fmt);
    vprint(fmt, argp);
    va_end(argp);
}


bool hal_ble_rec_data(void) 
{
    ble_rec_data_len = blecomm.available();
    parse_cmd_type = 0;
    if(ble_rec_data_len > 0 )
    {
        parse_cmd_type = 1;
        blecomm.read(ble_rec_data_buf,ble_rec_data_len);
        ble_rec_done = true;
        blecomm.flush(); // empty rx fifo
        return true;
    }
    return false;
}


void app_ble_disconnect( void )
{
    Bluefruit.disconnect(0);;
}


