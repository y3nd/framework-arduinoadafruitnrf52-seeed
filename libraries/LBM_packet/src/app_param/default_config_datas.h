#ifndef _DEFAULT_CONFIG_DATAS_H
#define _DEFAULT_CONFIG_DATAS_H

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "app_config_param.h"


void lorawan_param_init(void);
int default_config_read (void *buffer, uint8_t size);

int default_config_write (uint8_t *buffer, uint8_t size);
bool save_Config(void);
void get_Config(void);
uint8_t crc8(uint8_t *data, uint8_t data_len);
#endif


