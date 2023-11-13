
#ifndef _MAIN_LBM_H_
#define _MAIN_LBM_H_

#include <stdint.h>
#include <stdbool.h>

#include "main_lora.h"


#define APP_TASK_LORA_TX_QUEUE_MAX  48  



extern uint8_t send_retry_type;         // 0:don't need check confirm  1: at most confirm once;     2: at most confirm twice;

extern uint8_t app_task_lora_tx_in;     // lora sends data input number

extern uint8_t app_task_lora_tx_out;    // lora sends data output number


extern uint32_t app_send_confirmed_count;       // loracloud confirmed receive data count 
extern uint32_t app_task_lora_tx_confirmed_count;  // confirmed receive data count temp
extern bool app_task_lora_tx_cache;     // Whether to cache historical data

extern uint8_t app_task_lora_tx_buffer_len[APP_TASK_LORA_TX_QUEUE_MAX] ;    // lora tx data len
extern bool app_task_lora_tx_buffer_confirmed[APP_TASK_LORA_TX_QUEUE_MAX];  // Whether lora tx data need confirm
extern uint8_t app_task_lora_tx_buffer[APP_TASK_LORA_TX_QUEUE_MAX][LORAWAN_APP_DATA_MAX_SIZE] ; //lora tx data

/**
 * app_lora_confirmed_count_increment() - Receive confirm that loracloud receives the data
 *
 */
void app_lora_confirmed_count_increment( void );

/**
 * app_lora_get_confirmed_count() - Get confirmed count
 *
 * @return  confirmed count
 */
uint32_t app_lora_get_confirmed_count( void );

/**
 * app_task_lora_tx_queue() - Insert data to lora tx queue
 *
 * @param buf    Data buffer
 *
 * @param len    Data len
 *
 * @param confirmed     Whether need confirm
 *
 * @param emergency    Whether need emergency
 *
 * @return  true: success, false: fail
 */
bool app_task_lora_tx_queue( uint8_t *buf, uint8_t len, bool confirmed, bool emergency );

/**
 * app_task_lora_get_timestamp() - Get timestamp
 *
 * @return  Timestamp
 */
uint32_t app_task_lora_get_timestamp( void );

/**
 * app_task_lora_tx_engine() - Lora tx engine
 *
 * @return  true: success, false: fail
 */
bool app_task_lora_tx_engine( void );

/**
 * app_task_packet_downlink_decode() - Decode Lora downlink
 *
 * @param buf    Data buffer
 *
 * @param len    Data len
 *
 */
void app_task_packet_downlink_decode( uint8_t *buf, uint8_t len );

/**
 * app_task_lora_save_tx_data() - Save lora data that failed to send
 *
 * @param buf    Data buffer
 *
 * @param len    Data len
 *
 */
bool app_task_lora_save_tx_data( uint8_t *buf, uint8_t len );

/**
 * app_task_lora_off_line_send_and_check() - Check and resend historical data
 *
 * @param buf    Data buffer
 *
 * @param len    Data len
 *
 * @return  True: success false: fail
 */
bool app_task_lora_off_line_send_and_check( void );

void LBM_versions_print(ralf_t* ralf);
#endif
