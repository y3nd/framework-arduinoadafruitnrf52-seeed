#ifndef _MAIN_TRACK_H_
#define _MAIN_TRACK_H_

#include <stdbool.h>
#include <stdint.h>


#define TRACKER_GPS_EN      1
#define TRACKER_WIFI_EN     1
#define TRACKER_BLE_EN      1



#define TRACKER_SCAN_GPS                0
#define TRACKER_SCAN_WIFI               1
#define TRACKER_SCAN_BLE                4
#define TRACKER_SCAN_BLE_WIFI_GPS       7



#define TRACKER_STATE_BIT2_MOT_BEG          0x0001
#define TRACKER_STATE_BIT3_MOT_END          0x0002
#define TRACKER_STATE_BIT4_DEV_STATIC       0x0004
#define TRACKER_STATE_BIT5_DEV_SHOCK        0x0008

#define TRACKER_STATE_BIT0_SOS              0x0040
#define TRACKER_STATE_BIT1_USER             0x0080



extern uint8_t tracker_gps_scan_len;    // gnss scan results len    
extern uint8_t tracker_gps_scan_data[64];   // gnss scan results data buffer

extern uint8_t tracker_wifi_scan_len;   // wifi scan results data len    
extern uint8_t tracker_wifi_scan_data[64];  // wifi scan results data buffer    

extern uint8_t tracker_ble_scan_len;   // ble scan results len     
extern uint8_t tracker_ble_scan_data[64];   // ble scan results data buffer 

extern uint8_t tracker_scan_temp_len;   // tracker scan results temp len    
extern uint8_t tracker_scan_data_temp[64];   // tracker scan results temp data len     

extern uint8_t tracker_scan_type; // 0: gps, 1: wifi, 2: ble, 3: gps/wifi, 4: gps/ble, 5: gps/wifi/ble


extern uint32_t state_all;      // event flag      
extern uint8_t motion_index;    // motion sequence index       
extern uint8_t motion_index_backup; // backup motion sequence index 



extern bool track_gnss_busy;    // gnss scan busy flag   

extern bool track_wifi_busy;    // wifi scan busy flag 

extern uint16_t track_gnss_group_id;    // gnss positioning group id 

extern uint32_t app_task_track_time_stamp;  // start track timestamp 


/**
 * app_task_track_scan_send() - Send tracker results
 * 
 */
void app_task_track_scan_send( void );

/**
 * app_task_track_get_utc() - Get current utc
 * 
 * @return    utc
 */
uint32_t app_task_track_get_utc( void );

/**
 * track_scan_type_set() - set track scan type
 * 
 * @param scan_type   TRACKER_SCAN_GPS/TRACKER_SCAN_WIFI/TRACKER_SCAN_BLE/TRACKER_SCAN_BLE_WIFI_GPS
 *
 */
void track_scan_type_set( uint8_t scan_type );

/**
 * app_task_track_motion_index_update() - Updata motion index
 * 
 */
void app_task_track_motion_index_update( void );

/**
 * app_task_track_wifi_is_busy() - check Whether wifi scan busy
 * 
 * @return    true: busy, false: idle
 */
bool app_task_track_wifi_is_busy( void );

/**
 * app_task_track_gnss_is_busy() - check Whether gnss scan busy
 * 
 * @return    true: busy, false: idle
 */
bool app_task_track_gnss_is_busy( void );

#endif
