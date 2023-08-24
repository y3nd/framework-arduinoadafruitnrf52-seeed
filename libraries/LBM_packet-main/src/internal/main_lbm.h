
#ifndef _MAIN_LBM_H_
#define _MAIN_LBM_H_

#include <stdint.h>
#include <stdbool.h>

#include "main_lora.h"

#define LBM_LORA_TX_TEST    0

#define APP_TASK_LORA_TX_QUEUE_MAX  48

#define APP_TASK_TX_OFFLINE_CONFIRMED_LOST_MAX  1

#define APP_TASK_JOIN_RUN_MAX   600000
#define APP_TASK_JOIN_STOP_MAX  600000

#define APP_TASK_OFFLINE_RUN_24H_MAX    86400000

#define APP_TASK_LBM_TX_FAST_DELAY  20000


extern uint8_t app_task_lora_tx_in;


extern uint8_t app_task_lora_tx_out;

extern uint32_t app_lora_sync_in_rtc_second;
extern uint32_t app_lora_sync_in_utc_second;


extern uint8_t app_task_lora_tx_buffer_len[APP_TASK_LORA_TX_QUEUE_MAX] ;
extern bool app_task_lora_tx_buffer_confirmed[APP_TASK_LORA_TX_QUEUE_MAX];
extern uint8_t app_task_lora_tx_buffer[APP_TASK_LORA_TX_QUEUE_MAX][LORAWAN_APP_DATA_MAX_SIZE] ;


bool app_task_lora_tx_queue( uint8_t *buf, uint8_t len, bool confirmed, bool emergency );
uint32_t app_task_lora_get_timestamp( void );

void app_task_lora_tx_done_wakeup( void );

bool app_task_lora_tx_engine( void );

#endif
