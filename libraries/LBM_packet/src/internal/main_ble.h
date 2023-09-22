#ifndef _MAIN_BLE_H_
#define _MAIN_BLE_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>


#define APP_SCAN_INTERVAL               160      /**< Determines scan interval(in units of 0.625 ms). */
#define APP_SCAN_WINDOW                 80       /**< Determines scan window(in units of 0.625 ms). */
#define APP_SCAN_DURATION               0        /**< Duration of the scanning(in units of 10 ms). */

#define BEACON_DATA_LEN     0x15
#define BEACON_DATA_TYPE    0x02
#define COMPANY_IDENTIFIER  0x004C        //Apple's company ID  

#define BLE_BEACON_BUF_MAX      16
#define BLE_BEACON_SEND_MUM     3
#define BLE_BEACON_FILTER       -80

typedef struct sBleBeacons
{
    uint32_t utc;               //time 
    uint16_t company_id;        //ibeacon company id
    uint8_t uuid[16];           //ibeacon uuid
    uint16_t major;
    uint16_t minor;
    int8_t rssi;                 
    int8_t rssi_;
    uint8_t mac[8];
}BleBeacons_t;

extern uint8_t ble_scan_status;   //0:none  1:init 2:start 3:stop

extern uint8_t ble_beacon_res_num;  //Number of ibeacons scanned
extern BleBeacons_t ble_beacon_buf[BLE_BEACON_BUF_MAX];     //ibeacons data buffer
extern uint8_t ble_beacon_rssi_array[BLE_BEACON_BUF_MAX];   //ibeacons rssi data buffer

extern char ble_rec_data_buf[244];  //ble receive data buffer    
extern uint8_t ble_rec_data_len;    //ble receive data len 
extern volatile bool ble_rec_done;  //ble receive complete flag

extern volatile uint8_t ble_connect_status;    //0: none  1:init 2:connect 3:disconnect

/**
 * buf_cmp_value() - Compare the two groups to see if the numbers are the same
 * 
 * @param a    Data
 *
 * @param b    Data
 *
 * @param len    Data len
 *
 */
bool buf_cmp_value( uint8_t *a, uint8_t *b, uint8_t len );

/**
 * app_ble_scan_init() - Initializes the Bluetooth scan
 *
 */
void app_ble_scan_init( void );


/**
 * scan_params_init() - Initializes the Bluetooth scan parameter
 *
 */
void scan_params_init(void);

/**
 * app_ble_scan_start() - Start the Bluetooth scan
 *
 */
bool app_ble_scan_start( void );

/**
 * app_ble_get_results() - Get the Bluetooth scan results
 *
 * @param result    Data results 
 *
 * @param size    Size
 *
 */
bool app_ble_get_results( uint8_t *result, uint8_t *size );

/**
 * app_ble_scan_stop() - Stop the Bluetooth scan
 *
 */
void app_ble_scan_stop( void );

/**
 * app_ble_display_results() - Display the Bluetooth scan results
 *
 */
void app_ble_display_results( void );

/**
 * app_ble_adv_init() - Initializes the Bluetooth Advertising
 *
 */
void app_ble_adv_init( void );

/**
 * adv_params_init() - Initialize Bluetooth Advertising parameters
 *
 */
void adv_params_init(void);

/**
 * startAdv() - start Bluetooth Advertising
 *
 */
void startAdv(void);

/**
 * connect_callback() - Callback invoked when device connection
 *
 * @param conn_handle    connection where this event happens
 *
 */
void connect_callback(uint16_t conn_handle);

/**
 * disconnect_callback() - Callback invoked when device disconnection
 *
 * @param conn_handle    connection where this event happens
 *
 * @param reason    is a BLE_HCI_STATUS_CODE which can be found in ble_hci.h
 *
 */
void disconnect_callback(uint16_t conn_handle, uint8_t reason);

/**
 * hal_ble_trace_print() - print data by ble
 *
 * @param fmt    param
 *
 */
void hal_ble_trace_print(const char *fmt, ...);

/**
 * hal_ble_rec_data() - Detects whether Bluetooth receives data
 *
 * @return    true: received, false: none.  
 */
bool hal_ble_rec_data(void) ;


#endif
