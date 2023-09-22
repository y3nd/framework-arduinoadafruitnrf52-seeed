#ifndef _USER_STORED_DATA_H
#define _USER_STORED_DATA_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "app_config_param.h"
#include "key_value_fds.h"

#define DEV_INFO_FILE           (0x100)
#define DEF_DEV_INFO_FILE       (0x101)

#define POS_INFO_FILE           (0x104)

#define GROUP_ID_INFO_FILE      (0x109)
#define APPEND_PARAM_INFO_FILE      (0x10A)

#define POS_SAVE_COUNT_MAX      120   
#define POS_SAVE_IN_FILE_MAX    60 
#define POS_FILE_ID_START       (0x400)
#define POS_FILE_ID_END         (0x400+POS_SAVE_COUNT_MAX)


/**
 * read_lfs_file() - read lfs file datas
 *
 * @param file_name   file name
 *
 * @param data   data buffer
 *
 * @param len   data len
 *
 * @return    0:success, others:fail
 */
uint8_t read_lfs_file(uint16_t file_name, uint8_t *data, uint16_t *len) ;

/**
 * write_lfs_file() - write lfs file datas
 *
 * @param file_name   file name
 *
 * @param data   data buffer
 *
 * @param len   data len
 *
 * @return    0:success, others:fail
 */
uint8_t write_lfs_file(uint16_t file_name, uint8_t *data, uint16_t len);

/**
 * read_lfs_file_one() - read position file datas 
 *
 * @param file_name   file name
 *
 * @param data   data buffer
 *
 * @param len   data len
 *
 * @return    0:success, others:fail
 */
uint8_t read_lfs_file_one( uint16_t file_name, uint8_t *data, uint16_t *len );

/**
 * write_lfs_file_one() - read position file datas
 *
 * @param file_name   file name
 *
 * @param data   data buffer
 *
 * @param len   data len
 *
 * @return    0:success, others:fail
 */
uint8_t write_lfs_file_one( uint16_t file_name, uint8_t *data, uint16_t len );


/**
 * delete_lfs_file() - delete lfs file datas
 *
 * @param file_name   file name
 *
 * @return    0:success, others:fail
 */
uint8_t delete_lfs_file(uint16_t file_name);


/**
 * param_init_func() - init lfs file datas
 *
 */
void param_init_func(void);

/**
 * write_default_param_config() - write default parameters config
 *
 * @return    true:success, false:fail
 */
bool write_default_param_config(void);

/**
 * read_default_param_config() - read default parameters config
 *
 * @return    true:success, false:fail
 */
bool read_default_param_config(void);

/**
 * write_current_param_config() - write current parameters config
 *
 * @return    true:success, false:fail
 */
bool write_current_param_config(void);

/**
 * read_current_param_config() - read current parameters config
 *
 * @return    true:success, false:fail
 */
bool read_current_param_config(void);

/**
 * default_factory_eui_get() - get default DevEui 
 * 
 * @param def_eui   eui buffer
 *
 * @return    true:success, false:fail
 */
bool default_factory_eui_get(uint8_t *def_eui);

/**
 * reset_factory_param() - Restore all parameters to default
 * 
 * @return    true:success, false:fail
 */
bool reset_factory_param(void);

/**
 * restore_lorawan_param_config() - Restore LoRaWAN parameters to default
 * 
 * @return    true:success, false:fail
 */
bool restore_lorawan_param_config(void);


/**
 * write_position_msg() - write position message datas to lfs
 * 
 * @return    0:success, others:fail
 */
uint8_t write_position_msg(void);

/**
 * get_pos_msg_data() - print position message datas
 * 
 * @return    0:success, others:fail
 */
void get_pos_msg_data(void);

/**
 * read_pos_data() - read single position data
 * 
 * @param pos_msg_temp   position data
 *
 * @param is_old    old one or new
 *
 * @return    true:success, false:fail
 */
bool read_pos_data(pos_msg_param_t *pos_msg_temp,bool is_old);

/**
 * read_sequence_pos_data() - read  position data
 * 
 * @param pos_msg_temp   position data
 *
 * @param is_old    old one or new
 *
 * @param sequence_num    sequence number
 *
 * @return    true:success, false:fail
 */
bool read_sequence_pos_data(pos_msg_param_t *pos_msg_temp,bool is_old,uint16_t sequence_num);

/**
 * get_pos_msg_cnt() - get the number of location data items
 * 
 * @return    number of location data items
 */
uint16_t get_pos_msg_cnt(void);

/**
 * get_pos_msg_cnt() - get the number of location data items
 * 
 * @return    number of location data items
 */
uint8_t delete_position_msg(void);

/**
 * delete_pos_msg_datas() - delete position datas
 * 
 * @param del_cnt   delete cnt
 *
 * @param is_old    old one or new
 *
 * @return    0:success others:fail
 */
uint8_t delete_pos_msg_datas(uint16_t del_cnt,bool is_old);

/**
 * reflash_pos_msg_info() - reflash position message info
 * 
 * @return    0:success others:fail
 */
uint8_t reflash_pos_msg_info(void);

/**
 * get_pos_msg_info() - get position message info
 * 
 * @return    true:success false:fail
 */
bool get_pos_msg_info(void);

/**
 * print_pos_msg_data() - print position data
 * 
 * @return    true:success false:fail
 */
void print_pos_msg_data(void);

/**
 * read_gnss_group_id_param() - read group id
 * 
 * @return    true:success false:fail
 */
bool read_gnss_group_id_param(void);

/**
 * write_gnss_group_id_param() - write group id
 * 
 * @return    true:success false:fail
 */
uint8_t write_gnss_group_id_param(void);

/**
 * gnss_group_id_param_init() - init group id
 * 
 * @return    true:success false:fail
 */
void gnss_group_id_param_init(void);

/**
 * read_app_append_param() - app append param
 * 
 * @return    true:success false:fail
 */
bool read_app_append_param(void);
/**
 * write_app_append_param() - write app append param
 * 
 * @return    0:success other:fail
 */
uint8_t write_app_append_param(void);
/**
 * app_append_param_init() - init app append param
 * 
 */
void app_append_param_init(void);

#endif


