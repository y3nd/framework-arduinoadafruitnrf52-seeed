#ifndef _MAIN_LORA_H_
#define _MAIN_LORA_H_

#include <stdint.h>
#include <stdbool.h>

#include "common/lorawan_key_config.h"
#include "common/apps_modem_common.h"
#include "common/apps_modem_event.h"
#include "common/apps_utilities.h"


#include "lbm/smtc_modem_api/smtc_modem_api.h"
#include "lbm/smtc_modem_core/device_management/device_management_defs.h"
#include "mw/geolocation_middleware/wifi/src/wifi_middleware.h"

#include "lbm/smtc_modem_api/smtc_modem_utilities.h"


#define MODEM_EXAMPLE_ASSISTANCE_POSITION_LAT_DEFAULT ( 22.576814 )
#define MODEM_EXAMPLE_ASSISTANCE_POSITION_LONG_DEFAULT ( 113.922068 )


#define xstr( a ) str( a )
#define str( a ) #a


#define LORAWAN_APP_PORT 5          //The default LoRaWAN application port for uploading data to the sensecap platform 

#define LORAWAN_APP_DATA_MAX_SIZE 242       //Maximum lora data length

#define LORAWAN_DEFAULT_DATARATE SMTC_MODEM_ADR_PROFILE_CUSTOM


//Default lora data DR Control 
//It must ensure that the minimum device can send 51 bytes of data,So that no subcontracting is required to send data
#define LORAWAN_EU868_DR_MIN    0
#define LORAWAN_EU868_DR_MAX    6

#define LORAWAN_US915_DR_MIN    1
#define LORAWAN_US915_DR_MAX    4

#define LORAWAN_AU915_DR_MIN    3
#define LORAWAN_AU915_DR_MAX    6

#define LORAWAN_AS923_DR_MIN    3
#define LORAWAN_AS923_DR_MAX    6

#define LORAWAN_KR920_DR_MIN    0
#define LORAWAN_KR920_DR_MAX    5

#define LORAWAN_IN865_DR_MIN    0
#define LORAWAN_IN865_DR_MAX    5

#define LORAWAN_RU864_DR_MIN    0
#define LORAWAN_RU864_DR_MAX    6


extern uint8_t stack_id;    // Lora stack id

extern uint8_t app_lora_port;   // lora data sending port

//aiding position coordinate
extern float app_task_gnss_aiding_position_latitude; 
extern float app_task_gnss_aiding_position_longitude;

extern uint8_t adr_custom_list_region[16];  // custom adr lists of different regions 

extern uint8_t mw_gnss_event_state;         // gnss event see gnss_mw_event_type_t
extern uint8_t app_lora_data_tx_buffer[LORAWAN_APP_DATA_MAX_SIZE];  //lora tx buffer
extern uint8_t app_lora_data_tx_size;   //lora tx buffer size

extern uint8_t app_lora_data_rx_buffer[LORAWAN_APP_DATA_MAX_SIZE];  //lora rx buffer
extern uint8_t app_lora_data_rx_size;   //lora rx buffer size


//default adr list for different regions 
extern uint8_t adr_custom_list_eu868_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_us915_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_au915_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_as923_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_kr920_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_in865_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_ru864_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7


extern volatile bool is_first_time_sync;    // the time synchronization flag


extern wifi_mw_event_data_scan_done_t wifi_results;     //wifi result

/**
 * app_lora_set_port() - Set LoRaWAN application port
 *
 * @param port  port
 *
 */
void app_lora_set_port( uint8_t port );

/**
 * app_lora_send_frame() - Send lora data frame
 *
 * @param buffer  Data buffer
 *
 * @param length  Data length 
 *
 * @param tx_confirmed  Whether need confirm
 *
 * @param emergency  Whether need emergency
 *
 */
bool app_lora_send_frame( uint8_t* buffer, uint8_t length, bool tx_confirmed, bool emergency );

/**
 * app_lora_is_idle() - Check whether lora is idle
 *
 * @return  true: idle, false: busy
 */
bool app_lora_is_idle( void );

/**
 * app_task_lora_clock_is_synch() - Check whether time synchronization is complete
 *
 * @return  true: success, false: fail
 */
bool app_task_lora_clock_is_synch( void );

/**
 * app_task_radio_gnss_is_busy() - Check whether gnss scanning
 *
 * @return  true: busy, false: idle
 */
bool app_task_radio_gnss_is_busy( void );

/**
 * app_task_radio_wifi_is_busy() - Check whether Wifi scanning
 *
 * @return  true: idle, false: busy
 */
bool app_task_radio_wifi_is_busy( void );

/**
 * custom_lora_adr_compute() - Generate an adr list based on the DR Range
 *
 * @param min   DR min
 *
 * @param max   DR max
 *
 * @param buf   adr list 
 *
 * @return  true: idle, false: busy
 */
void custom_lora_adr_compute( uint8_t min, uint8_t max, uint8_t *buf );

/**
 * app_get_profile_list_by_region() - Get the default adr list of the region
 *
 * @param REGION   region
 *
 * @param buf   adr list
 *
 */
void app_get_profile_list_by_region(smtc_modem_region_t REGION,uint8_t *buf);

#endif



