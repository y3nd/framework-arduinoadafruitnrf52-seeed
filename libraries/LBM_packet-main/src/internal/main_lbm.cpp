#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

uint8_t app_task_lora_tx_buffer_len[APP_TASK_LORA_TX_QUEUE_MAX] = { 0 };
bool app_task_lora_tx_buffer_confirmed[APP_TASK_LORA_TX_QUEUE_MAX] = { false };
uint8_t app_task_lora_tx_buffer[APP_TASK_LORA_TX_QUEUE_MAX][LORAWAN_APP_DATA_MAX_SIZE] = { 0 };
uint8_t app_task_lora_tx_in = 0;
uint8_t app_task_lora_tx_out = 0;

uint32_t app_lora_sync_in_rtc_second = 0;
uint32_t app_lora_sync_in_utc_second = 0;


bool app_task_lora_tx_engine( void )
{
    static uint8_t send_count = 0;
    static uint32_t   app_task_lora_tx_toa = 0;
    static uint32_t tx_last_timestemp = 0;
    smtc_modem_status_mask_t modem_status;
    smtc_modem_region_t cur_region;
    uint8_t  dutycycle_enable = 0;

    uint32_t tx_now_timestemp = smtc_modem_hal_get_time_in_ms( );

    ASSERT_SMTC_MODEM_RC( smtc_modem_get_region(0, &cur_region));
    
    ASSERT_SMTC_MODEM_RC( smtc_modem_get_duty_cycle_enable(&dutycycle_enable));

    if(dutycycle_enable == 1)
    {
        if((cur_region == SMTC_MODEM_REGION_EU_868) || (cur_region == SMTC_MODEM_REGION_RU_864))
        {
            if(tx_now_timestemp<(tx_last_timestemp+app_task_lora_tx_toa * 99))  //Limit dutycycle to less than 1% 
            {
                return false;
            }
        }
    }

    uint8_t out = app_task_lora_tx_out % APP_TASK_LORA_TX_QUEUE_MAX;

    if( app_task_lora_tx_buffer_len[out] )
    {
        smtc_modem_get_status( 0, &modem_status );
        if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED 
            && app_lora_is_idle( ) 
            && app_task_radio_gnss_is_busy( ) == false && app_task_radio_wifi_is_busy( ) == false 
            && app_task_track_gnss_is_busy( ) == false && app_task_track_wifi_is_busy( ) == false )
        {
            bool result = app_lora_send_frame( app_task_lora_tx_buffer[out], app_task_lora_tx_buffer_len[out], app_task_lora_tx_buffer_confirmed[out], false );
            if( result )
            {
                ASSERT_SMTC_MODEM_RC( smtc_modem_get_toa_status(&app_task_lora_tx_toa,app_task_lora_tx_buffer_len[out] + 13));
                tx_last_timestemp = smtc_modem_hal_get_time_in_ms( );
                send_count++;
                hal_mcu_trace_print( "app_task_lora_tx_out: %u\r\n", app_task_lora_tx_out );
                if(send_count>=2)
                {
                    send_count = 0;
                    memset(app_task_lora_tx_buffer[out],0,app_task_lora_tx_buffer_len[out]);
                    app_task_lora_tx_buffer_len[out] = 0;
                    app_task_lora_tx_buffer_confirmed[out] = 0;
                    app_task_lora_tx_out = (++out)% APP_TASK_LORA_TX_QUEUE_MAX;
                }
            }
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
    return true;
}

bool app_task_lora_tx_queue( uint8_t *buf, uint8_t len, bool confirmed, bool emergency )
{
    bool result = false;
    smtc_modem_status_mask_t modem_status;
    smtc_modem_get_status( 0, &modem_status );
    if(( modem_status & SMTC_MODEM_STATUS_JOINED ) == SMTC_MODEM_STATUS_JOINED )
    {
        hal_mcu_trace_print( "app_task_lora_tx_in: %u\r\n", app_task_lora_tx_in );        
        uint8_t in = app_task_lora_tx_in  % APP_TASK_LORA_TX_QUEUE_MAX;
        app_task_lora_tx_buffer_len[in] = len;
        app_task_lora_tx_buffer_confirmed[in] = confirmed;
        for( uint8_t j = 0; j < len; j++ )
        {
            app_task_lora_tx_buffer[in][j] = buf[j];
        }
        app_task_lora_tx_in = (++app_task_lora_tx_in)%APP_TASK_LORA_TX_QUEUE_MAX;
    }
    else
    {
        result = false;
    }

    return result;
}

uint32_t app_task_lora_get_timestamp( void )
{
    uint32_t timestamp_now = 0;
    if( app_task_lora_clock_is_synch( ))
    {
        timestamp_now = apps_modem_common_get_utc_time( );
    }
    return timestamp_now;
}