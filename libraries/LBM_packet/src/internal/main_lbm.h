
#ifndef _MAIN_LBM_H_
#define _MAIN_LBM_H_

#include <stdint.h>
#include <stdbool.h>

#include "main_lora.h"


#define APP_TASK_LORA_TX_QUEUE_MAX  48



extern uint8_t send_retry_type;         // 0:don't need check confirm  1: at most confirm once;     2: at most confirm twice;

extern uint8_t app_task_lora_tx_in;     

extern uint8_t app_task_lora_tx_out;

extern uint32_t app_lora_sync_in_rtc_second;
extern uint32_t app_lora_sync_in_utc_second;
extern uint32_t app_send_confirmed_count;
extern uint32_t app_task_lora_tx_confirmed_count;  


extern uint8_t app_task_lora_tx_buffer_len[APP_TASK_LORA_TX_QUEUE_MAX] ;
extern bool app_task_lora_tx_buffer_confirmed[APP_TASK_LORA_TX_QUEUE_MAX];
extern uint8_t app_task_lora_tx_buffer[APP_TASK_LORA_TX_QUEUE_MAX][LORAWAN_APP_DATA_MAX_SIZE] ;

void app_lora_confirmed_count_increment( void );
uint32_t app_lora_get_confirmed_count( void );

bool app_task_lora_tx_queue( uint8_t *buf, uint8_t len, bool confirmed, bool emergency );
uint32_t app_task_lora_get_timestamp( void );

void app_task_lora_tx_done_wakeup( void );

bool app_task_lora_tx_engine( void );

void app_task_packet_downlink_decode( uint8_t *buf, uint8_t len );

#endif
