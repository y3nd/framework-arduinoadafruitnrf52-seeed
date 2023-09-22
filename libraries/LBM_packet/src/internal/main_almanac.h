#ifndef _MAIN_ALMANAC_H_
#define _MAIN_ALMANAC_H_


/**
 * app_task_full_almanac_update() - Update full almanac to lr1110 
 *
 */
void app_task_full_almanac_update( void );

/**
 * app_task_radio_get_almanac_date() - Get almanac date from lr1110
 *
 * @return     almanac date
 *
 */
uint32_t app_task_radio_get_almanac_date( void );

/**
 * get_almanac_date_from_buffer() - Get almanac date from full_almanac buffer
 *
 * @return     almanac date
 *
 */
uint32_t get_almanac_date_from_buffer( void );


#endif
