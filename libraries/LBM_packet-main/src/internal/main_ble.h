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
#define COMPANY_IDENTIFIER  0x004C

#define BLE_BEACON_BUF_MAX      16
#define BLE_BEACON_SEND_MUM     3
#define BLE_BEACON_FILTER       -80

typedef struct sBleBeacons
{
    uint32_t utc;
    uint16_t company_id;
    uint8_t uuid[16];
    uint16_t major;
    uint16_t minor;
    int8_t rssi;
    int8_t rssi_;
    uint8_t mac[8];
}BleBeacons_t;

extern uint8_t ble_scan_status;   //0:none  1:init 2:start 3:stop

extern uint8_t ble_beacon_res_num;
extern BleBeacons_t ble_beacon_buf[BLE_BEACON_BUF_MAX];
extern uint8_t ble_beacon_rssi_array[BLE_BEACON_BUF_MAX];





bool buf_cmp_value( uint8_t *a, uint8_t *b, uint8_t len );

void app_ble_scan_init( void );
bool app_ble_scan_start( void );
bool app_ble_get_results( uint8_t *result, uint8_t *size );
void app_ble_scan_stop( void );

void app_ble_display_results( void );

void scan_params_init(void);
#endif
