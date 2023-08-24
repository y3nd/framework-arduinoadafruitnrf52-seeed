
#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>
#include <math.h>


/*!
 * @brief Stack identifier
 */
uint8_t stack_id = 0;

uint8_t app_lora_port = LORAWAN_APP_PORT;

uint8_t adr_custom_list_eu868_default[16] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5 }; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
uint8_t adr_custom_list_us915_default[16] = { 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3 }; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
uint8_t adr_custom_list_au915_default[16] = { 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5 }; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
uint8_t adr_custom_list_as923_default[16] = { 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5 }; // SF9,SF9,SF9,SF9,SF9,SF8,SF8,SF8,SF8,SF8,SF7,SF7,SF7,SF7,SF7
uint8_t adr_custom_list_kr920_default[16] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5 }; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
uint8_t adr_custom_list_in865_default[16] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5 }; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7
uint8_t adr_custom_list_ru864_default[16] = { 0, 0, 0, 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 4, 5, 5 }; // SF12,SF12,SF12,SF11,SF11,SF11,SF10,SF10,SF10,SF9,SF9,SF9,SF8,SF8,SF7,SF7

uint8_t adr_custom_list_region[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


uint8_t app_lora_data_tx_buffer[LORAWAN_APP_DATA_MAX_SIZE];
uint8_t app_lora_data_tx_size = 0;

uint32_t app_lora_tx_recent_time = 0;


/*!
 * @brief First time sync status for application startup
 */
volatile bool is_first_time_sync = false;

/*!
 * @brief 
 */
uint8_t mw_gnss_event_state = 0;

/*!
 * @brief Wi-Fi output results
 */
wifi_mw_event_data_scan_done_t wifi_results;

float app_task_gnss_aiding_position_latitude = MODEM_EXAMPLE_ASSISTANCE_POSITION_LAT_DEFAULT;
float app_task_gnss_aiding_position_longitude = MODEM_EXAMPLE_ASSISTANCE_POSITION_LONG_DEFAULT;

void custom_lora_adr_compute( uint8_t min, uint8_t max, uint8_t *buf )
{
    uint8_t temp = max - min + 1;
    uint8_t num = 16 / temp;
    uint8_t remain = 16 % temp;
    uint8_t offset = 0;

    for( uint8_t i = 0; i < temp; i++ )
    {
        for( uint8_t j = 0; j < num; j++ )
        {
            buf[i * num + j + offset] = min + i;
        }

        if(( 16 % temp != 0 ) && ( i < remain ))
        {
            buf[( i + 1 ) * num + offset] = min + i;
            offset += 1;
        }
    }
}
/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS DEFINITION --------------------------------------------
 */

void app_lora_set_port( uint8_t port )
{
    app_lora_port = port;
}

bool app_lora_send_frame( uint8_t* buffer, uint8_t length, bool tx_confirmed, bool emergency )
{
    uint8_t tx_max_payload;
    int32_t duty_cycle;

    /* Check if duty cycle is available */
    ASSERT_SMTC_MODEM_RC( smtc_modem_get_duty_cycle_status( &duty_cycle ));
    if( duty_cycle < 0 )
    {
        hal_mcu_trace_print( "Duty-cycle limitation - next possible uplink in %d ms \n\n", duty_cycle );
        return false;
    }
    
    ASSERT_SMTC_MODEM_RC( smtc_modem_get_next_tx_max_payload( stack_id, &tx_max_payload ));
    if( length > tx_max_payload )
    {
        hal_mcu_trace_print( "Not enough space in buffer - send empty uplink to flush MAC commands \n" );
        ASSERT_SMTC_MODEM_RC( smtc_modem_request_empty_uplink( stack_id, true, app_lora_port, tx_confirmed ));
        return false;
    }
    else
    {
        smtc_modem_return_code_t result;
        hal_mcu_trace_print( "Request uplink at %u\n", smtc_modem_hal_get_time_in_ms( ));
        if( app_lora_is_idle( ))
        {
            if( emergency )
            {
                result = smtc_modem_request_emergency_uplink( stack_id, app_lora_port, tx_confirmed, buffer, length );
            }
            else
            {
                result = smtc_modem_request_uplink( stack_id, app_lora_port, tx_confirmed, buffer, length );
            }

            ASSERT_SMTC_MODEM_RC( result );

            if( result == SMTC_MODEM_RC_OK )
            {
                 app_lora_tx_recent_time = smtc_modem_hal_get_time_in_ms( );
                return true;
            }
            else return false;
        }
        else
        {
            hal_mcu_trace_print( "radio is not idle\n" );
            return false;
        }
    }
}

bool app_lora_is_idle( void )
{
    smtc_modem_stack_state_t state;
    smtc_modem_get_stack_state( stack_id, &state );
    if( state == SMTC_MODEM_STACK_STATE_IDLE ) return true;
    else return false;
}

bool app_task_lora_clock_is_synch( void )
{
    return is_first_time_sync;
}

bool app_task_radio_gnss_is_busy( void )
{
    return gnss_mw_custom_get_scan_busy();
}

bool app_task_radio_wifi_is_busy( void )
{
    return wifi_mw_custom_get_scan_busy();
}

void app_task_lora_clock_run_synch( void )
{
    smtc_modem_status_mask_t modem_status;
    smtc_modem_get_status( 0, &modem_status );
    if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED )
    {
        hal_mcu_trace_print( "start sync clock at %u\r\n", smtc_modem_hal_get_time_in_ms( ));
        smtc_modem_time_stop_sync_service( stack_id );
        smtc_modem_time_start_sync_service( stack_id, SMTC_MODEM_TIME_ALC_SYNC );
    }
}
