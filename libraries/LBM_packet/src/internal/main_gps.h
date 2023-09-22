
#ifndef _MAIN_GPS_H_
#define _MAIN_GPS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "main_lora.h"
#include "mw/geolocation_middleware/common/mw_common.h"
#include <mw/geolocation_middleware/gnss/src/gnss_middleware.h>
#include "common/smtc_hal_dbg_trace.h"

#define GNSS_RAW_SCAN_SEND_MAX  2



extern uint8_t app_gnss_scan_mode; // 0: GNSS_MW_MODE_STATIC, 1: GNSS_MW_MODE_MOBILE

extern uint8_t gps_scan_status;   //0:none  1:init 2:start 3:stop

/**
 * app_gps_scan_init() - Initializes the gnss scan
 *
 */
void app_gps_scan_init( void );

/**
 * app_gps_scan_start() - Start the gnss scan
 *
 * @return    true: success, false: fail.  
 */
bool app_gps_scan_start( void );

/**
 * app_gps_get_results() - Get the gnss scan results
 *
 * @param buf    Data results 
 *
 * @param size    Size
 *
 */
bool app_gps_get_results( uint8_t *buf, uint8_t *size );

/**
 * app_gps_scan_stop() - Stop the gnss scan
 *
 */
void app_gps_scan_stop( void );

/**
 * app_gps_display_results() - Display the gnss scan results
 *
 */
void app_gps_display_results( void );

#endif
