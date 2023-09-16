#ifndef _APP_TASK_PACKET_H_
#define _APP_TASK_PACKET_H_

#include <stdint.h>
#include <stdbool.h>
#include "main_lora.h"

//uplink
#define DATA_ID_UP_PACKET_GNSS_RAW          0x12
#define DATA_ID_UP_PACKET_GNSS_END          0x13
#define DATA_ID_UP_PACKET_WIFI_RAW          0x14
#define DATA_ID_UP_PACKET_BLE_RAW           0x15
#define DATA_ID_UP_PACKET_POS_STATUS        0x16

#define DATA_ID_UP_PACKET_USER_SENSOR       0x17
#define DATA_ID_UP_PACKET_FACT_SENSOR       0x18

//downlink
#define DATA_ID_DOWN_PACKET_USER_CODE       0x8E



extern uint8_t app_lora_packet_buffer[LORAWAN_APP_DATA_MAX_SIZE];
extern uint8_t app_lora_packet_len;
extern uint8_t factory_sensor_temp_len;
extern uint8_t factory_sensor_data_temp[64];

void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );
void default_param_load(void);


void app_task_factory_sensor_data_send( void );
void app_task_user_sensor_data_send( void );

#endif
