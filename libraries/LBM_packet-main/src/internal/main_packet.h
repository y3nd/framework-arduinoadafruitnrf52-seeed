#ifndef _APP_TASK_PACKET_H_
#define _APP_TASK_PACKET_H_

#include <stdint.h>
#include <stdbool.h>
#include "main_lora.h"

#define DATA_ID_UP_PACKET_WIFI_SEN_BAT      0x07
#define DATA_ID_UP_PACKET_BLE_SEN_BAT       0x08
#define DATA_ID_UP_PACKET_WIFI_BAT          0x0A
#define DATA_ID_UP_PACKET_BLE_BAT           0x0B

#define DATA_ID_UP_PACKET_GNSS_RAW          0x0E
#define DATA_ID_UP_PACKET_GNSS_SEN_BAT      0x0F
#define DATA_ID_UP_PACKET_GNSS_BAT          0x10
#define DATA_ID_UP_PACKET_POS_STATUS        0x11

extern uint8_t app_lora_packet_buffer[LORAWAN_APP_DATA_MAX_SIZE];
extern uint8_t app_lora_packet_len;


void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );
void default_param_load(void);

#endif
