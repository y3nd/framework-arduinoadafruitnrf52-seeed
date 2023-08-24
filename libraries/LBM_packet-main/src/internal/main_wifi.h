
#ifndef _MAIN_WIFI_H_
#define _MAIN_WIFI_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "mw/geolocation_middleware/wifi/src/wifi_helpers_defs.h"
#include "mw/geolocation_middleware/wifi/src/wifi_middleware.h"

#define WIFI_AP_RSSI_EN     1

/**
 * @brief Size in bytes to store the RSSI of a detected WiFi Access-Point
 */
#if WIFI_AP_RSSI_EN
    #define WIFI_AP_RSSI_SIZE ( 1 )
#else
    #define WIFI_AP_RSSI_SIZE ( 0 )
#endif

/**
 * @brief Size in bytes of a WiFi Access-Point address
 */
#define WIFI_AP_ADDRESS_SIZE ( 6 )

/*!
 * @brief The buffer containing results to be sent over the air
 */
extern uint8_t wifi_result_buffer[4+( WIFI_AP_RSSI_SIZE + WIFI_AP_ADDRESS_SIZE ) * WIFI_MAX_RESULTS + 4];
extern uint8_t wifi_scan_status;   //0:none  1:init 2:start 3:stop



void app_wifi_scan_init();
bool app_wifi_scan_start( void );
bool app_wifi_get_results( uint8_t *result, uint8_t *size );
void app_wifi_scan_stop( void );
void app_wifi_display_results( void );

#endif
