
#ifndef _MAIN_SENSOR_H_
#define _MAIN_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>

#define sht40_sensor_type         0x01
#define mma7660_sensor_type       0x02

extern int16_t temperture_val;
extern uint16_t light_val;

void sensor_init_detect(void);

//get sensor data 
void sensor_datas_get(void);
#endif
