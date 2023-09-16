
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


#define TRACKER_STATE_BIT1_USER             0x0080



extern uint8_t tracker_gps_scan_len;
extern uint8_t tracker_gps_scan_data[64];

extern uint8_t tracker_wifi_scan_len;
extern uint8_t tracker_wifi_scan_data[64];

extern uint8_t tracker_ble_scan_len;
extern uint8_t tracker_ble_scan_data[64];

extern uint8_t tracker_scan_temp_len;
extern uint8_t tracker_scan_data_temp[64];

extern uint8_t tracker_scan_type; // 0: gps, 1: wifi, 2: ble, 3: gps/wifi, 4: gps/ble, 5: gps/wifi/ble


extern uint32_t state_all;
extern uint8_t motion_index;
extern uint8_t motion_index_backup;



extern bool track_gnss_busy;
extern bool track_wifi_busy;

extern uint16_t track_gnss_group_id;
extern uint32_t app_task_track_time_stamp;



void app_task_track_scan_send( void );

uint32_t app_task_track_get_utc( void );
void tracker_scan_type_set( uint8_t scan_type );


void app_task_track_set_scan_state( uint32_t bit );
void app_task_track_clear_scan_state( uint32_t bit );
void app_task_track_motion_index_update( void );
void app_task_track_motion_index_clear( void );

bool app_task_track_wifi_is_busy( void );
bool app_task_track_gnss_is_busy( void );

void gnss_group_id_init(void);
bool gnss_group_id_write(void);

#endif
