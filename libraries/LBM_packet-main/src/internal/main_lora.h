
#ifndef _MAIN_TASK_H_
#define _MAIN_TASK_H_

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

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

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */
#define MODEM_EXAMPLE_ASSISTANCE_POSITION_LAT_DEFAULT ( 22.576814 )
#define MODEM_EXAMPLE_ASSISTANCE_POSITION_LONG_DEFAULT ( 113.922068 )
/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE MACROS-----------------------------------------------------------
 */

/*!
 * @brief Stringify constants
 */
#define xstr( a ) str( a )
#define str( a ) #a

/*
 * -----------------------------------------------------------------------------
 * --- PRIVATE CONSTANTS -------------------------------------------------------
 */
/**
 * @brief Duration in second after last ALC sync response received to consider the local clock time invalid
 *
 * Set time valid for 1 day (to be fine tuned depending on board properties)
 */
#define APP_ALC_TIMING_INVALID ( 3600 * 24 )

/**
 * @brief Interval in second between two consecutive ALC sync requests
 *
 * 3 time sync requests per day
 */
#define APP_ALC_TIMING_INTERVAL ( APP_ALC_TIMING_INVALID / 3 )

#define APP_LBM_IRQ_ERROR_CHECK 0

/*!
 * @brief Defines the application data transmission duty cycle. 60s, value in [s].
 */
#define APP_TX_DUTYCYCLE 60

/*!
 * @brief LoRaWAN application port
 */
#define LORAWAN_APP_PORT 5

/*!
 * @brief User application data buffer size
 */
#define LORAWAN_APP_DATA_MAX_SIZE 242

/*
 * -----------------------------------------------------------------------------
 * --- LoRaWAN Configuration ---------------------------------------------------
 */

/*!
 * @brief LoRaWAN confirmed messages
 */
#define LORAWAN_CONFIRMED_MSG_ON false

/*!
 * @brief Default datarate
 *
 * @remark See @ref smtc_modem_adr_profile_t
 */
#define LORAWAN_DEFAULT_DATARATE SMTC_MODEM_ADR_PROFILE_CUSTOM

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

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */
extern uint8_t stack_id;

// extern ralf_t* modem_radio;


extern uint8_t app_lora_port;


extern bool adr_user_enable;


extern float app_task_gnss_aiding_position_latitude;
extern float app_task_gnss_aiding_position_longitude;

extern uint8_t adr_custom_list_region[16];

extern uint32_t app_lora_tx_recent_time;
extern uint8_t mw_gnss_event_state;
extern uint8_t app_lora_data_tx_buffer[LORAWAN_APP_DATA_MAX_SIZE];
extern uint8_t app_lora_data_tx_size;


extern uint8_t adr_custom_list_eu868_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_us915_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_au915_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_as923_default[16]; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
extern uint8_t adr_custom_list_kr920_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_in865_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
extern uint8_t adr_custom_list_ru864_default[16]; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7


extern volatile bool is_first_time_sync;


extern wifi_mw_event_data_scan_done_t wifi_results;


/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */


void app_lora_set_port( uint8_t port );
bool app_lora_send_frame( uint8_t* buffer, uint8_t length, bool tx_confirmed, bool emergency );

bool app_lora_is_idle( void );

bool app_task_lora_clock_is_synch( void );
bool app_task_radio_gnss_is_busy( void );
bool app_task_radio_wifi_is_busy( void );
void app_task_lora_clock_run_synch( void );
void custom_lora_adr_compute( uint8_t min, uint8_t max, uint8_t *buf );


#endif



