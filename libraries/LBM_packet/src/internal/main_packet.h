#ifndef _MAIN_PACKET_H_
#define _MAIN_PACKET_H_

#include <stdint.h>
#include <stdbool.h>
#include "main_lora.h"

/*--------------------------- uplink ---------------------------*/
#define DATA_ID_UP_PACKET_GNSS_RAW          0x12
#define DATA_ID_UP_PACKET_GNSS_END          0x13
#define DATA_ID_UP_PACKET_WIFI_RAW          0x14
#define DATA_ID_UP_PACKET_BLE_RAW           0x15
#define DATA_ID_UP_PACKET_POS_STATUS        0x16

#define DATA_ID_UP_PACKET_USER_SENSOR       0x17
#define DATA_ID_UP_PACKET_FACT_SENSOR       0x18
#define DATA_ID_UP_PACKET_BOOT_DATA         0x19
/*--------------------------- downlink ---------------------------*/
#define DATA_ID_DOWN_PACKET_USER_CODE       0x8E

extern uint8_t factory_sensor_temp_len;     // Manufacturer default sensor data len temp
extern uint8_t factory_sensor_data_temp[64];    // Manufacturer default sensor data buffer temp


/**
 * memcpyr() - Copy data in a format with high byte data in front
 * 
 * @param dst   Destination data buffer
 *
 * @param src   Source  data buffer
 *
 * @param size   Data size
 *
 */
void memcpyr( uint8_t *dst, const uint8_t *src, uint16_t size );

/**
 * default_param_load() - Load default configuration parameters
 * 
 */
void default_param_load(void);

/**
 * app_task_factory_sensor_data_send() - Send Manufacturer default sensor data
 * 
 * @param dst   Destination data buffer
 *
 * @param src   Source  data buffer
 *
 * @param size   Data size
 *
 */
void app_task_factory_sensor_data_send( void );

/**
 * app_task_user_sensor_data_send() - Send user sensor data
 * 
 */
void app_task_user_sensor_data_send( void );

/**
 * sensecap_lorawan_region() - get region from flash  
 * 
 */
smtc_modem_region_t sensecap_lorawan_region(void);

void app_task_booting_data_send( uint16_t position_interval, uint16_t sample_interval);

#endif
