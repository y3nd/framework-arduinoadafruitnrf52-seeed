
#ifndef _MAIN_SENSOR_H_
#define _MAIN_SENSOR_H_

#include <stdint.h>
#include <stdbool.h>



//If defined as 1:detect iic slave device hot swap function
#ifndef SENSOR_HOT_SWAPPABLE_DETECT
#define SENSOR_HOT_SWAPPABLE_DETECT     1
#endif


//This parameter is defined based on whether the actual sensor is connected
#define SOUND_SENSOR_AVAILABLE          1       // for mission pack  the sound sensor is delivered with the system by default
#define ULTRASONIC_SENSOR_AVAILABLE     1       // for mission pack  the ultrasonic sensor is delivered with the system by default



//sensor type
#define sht4x_sensor_type           (0x01<<0)
#define lis3dhtr_sensor_type        (0x01<<1)
#define dps310_sensor_type          (0x01<<2)
#define sgp41_sensor_type           (0x01<<3)
#define si1151_sensor_type          (0x01<<4)
#define sound_sensor_type           (0x01<<5)
#define ultrasonic_sensor_type      (0x01<<6)



extern float sht4x_temperature;
extern float sht4x_humidity;
extern float lis3dhtr_x;
extern float lis3dhtr_y;
extern float lis3dhtr_z;
extern int32_t sgp41_voc_index;
extern float UV_val;
extern uint16_t sound_val;
extern uint16_t ultrasonic_distance_cm;
extern uint32_t dps310_pressure_val;


extern uint8_t factory_sensor_data_len;
extern uint8_t factory_sensor_data[20];

extern uint8_t user_sensor_data_len;
extern uint8_t user_sensor_data[40];

extern uint16_t sound_sample_index;
extern uint16_t sound_average_data;



/**
 * grove_init_power() - power on for grove.
 *
 */
void grove_init_power(void);

/**
 * sensor_available_detect() - detect sensor connect.
 *
 */
void sensor_available_detect(void);

/**
 * iic_slave_available_detect() - detect iic slave connect.
 *
 * @param slave_addr    iic slave device address.
 *
 * @return    true: connected, false: disconnected.  
 */
bool iic_slave_available_detect(uint8_t slave_addr);

/**
 * sht4x_available_detect() - detect iic slave device(sht4x) connect.
 *
 * @return    true: connected, false: disconnected.  
 */
bool sht4x_available_detect(void);

/**
 * lis3dhtr_available_detect() - detect iic slave device(lis3dhtr) connect.
 *
 * @return    true: connected, false: disconnected.  
 */
bool lis3dhtr_available_detect(void);

/**
 * si1151_available_detect() - detect iic slave device(si1151) connect
 *
 * @return    true: connected, false: disconnected  
 */
bool si1151_available_detect(void);

/**
 * sgp41_available_detect() - detect iic slave device(sgp41) connect
 *
 * @return    true: connected, false: disconnected  
 */
bool sgp41_available_detect(void);

/**
 * dps310_available_detect() - detect iic slave device(dps310) connect
 *
 * @return    true: connected, false: disconnected  
 */
bool dps310_available_detect(void);

/**
 * sensor_init_detect() - Initializes the sensor device
 *
 */ 
void sensor_init_detect(void);

/**
 * sensor_sht4x_init() - Initializes sht4x
 *
 */
void sensor_sht4x_init(void);      

/**
 * sensor_lis3dhtr_init() - Initializes lis3dhtr
 *
 */
void sensor_lis3dhtr_init(void);  

/**
 * sensor_sgp41_init() - Initializes sgp41
 *
 */
void sensor_sgp41_init(void);    

/**
 * sensor_si1151_init() - Initializes si1151
 *
 */
void sensor_si1151_init(void);      

/**
 * sensor_dps310_init() - Initializes dps310
 *
 */
void sensor_dps310_init(void);      

/**
 * sensor_datas_get() - Get all sensor data 
 *
 */
void sensor_datas_get(void); //missioin pack

/**
 * sht4x_data_get() - Get sht4x data 
 *
 * @param temperature   Temperature in degrees celsius.
 *
 * @param humidity      Humidity in percent relative humidity.
 *
 * @return    true: success, false: fail  
 */
bool sht4x_data_get(float* temperature, float* humidity);

/**
 * lis3dhtr_data_get() - Get lis3dhtr data.
 *
 * @param x   X-Axis Acceleration Data of LIS3DHTR.
 *
 * @param y   Y-Axis Acceleration Data of LIS3DHTR.
 *
 * @param z   Z-Axis Acceleration Data of LIS3DHTR.
 *
 * @return    true: success, false: fail  .
 */
bool lis3dhtr_data_get(float* x, float* y, float* z);

/**
 * sgp41_data_get() - Get sgp41 data,use Algorithm to calculate the voc index.
 *
 * @param temperature   temperature compensation in degrees celsius.
 *
 * @param humidity   humidity compensation in percent relative humidity.
 *
 * @param voc_index   VOC index values.
 *
 * @return    true: success, false: fail  
 */
bool sgp41_data_get(float temperature, float humidity, int32_t* voc_index);

/**
 * sgp41_sampling_period_get() - Get sgp41 sampling period 
 *
 * @return    sgp41 sampling period
 */
float sgp41_sampling_period_get(void);

/**
 * si1151_data_get() - Get si1151 data
 *
 * @param UV_val   Ultraviolet index value.
 *
 * @return    true: success, false: fail  
 */
bool  si1151_data_get(float* UV_val);

/**
 * sound_data_get() - Get sound data
 *
 * @param sound_val   Sound voltage value 
 *
 * @return    true: success, false: fail  
 */
bool  sound_data_get(uint16_t* sound_val);

/**
 * sound_average_get() - Get sound average data
 *
 * @return    sound average voltage data
 */
uint16_t  sound_average_get(void);

/**
 * ultrasonic_distance_get() - Get ultrasonic distance data
 *
 * @param distance_cm   Distance, in cm
 *
 * @return    true: success, false: fail  
 */
bool  ultrasonic_distance_get(uint16_t* distance_cm);

/**
 * dps310_data_get() - Get dps310 data
 *
 * @param pressure_val   Air pressure value
 *
 * @return    true: success, false: fail  
 */
bool  dps310_data_get(uint32_t* pressure_val);

/**
 * single_fact_sensor_data_get() - Get individual sensor data
 *
 * @param type   sensor type
 *
 * @return    true: success, false: fail  
 */
bool single_fact_sensor_data_get(uint8_t type);

/**
 * factory_sensor_data_combined() - Combine all sensor data
 *
 * @return    true: success, false: fail  
 */
void factory_sensor_data_combined(void);

/**
 * relay_status_control() - control relay on/off
 *
 * @param switch_on   true:on false:off
 *
 * @return    true: success, false: fail  
 */
void relay_status_control( bool switch_on );

/**
 * relay_status_on() - get relay on/off status
 *
 * @return    true: on, false: off
 */
bool relay_status_on( void );


/**
 * app_sensor_buffer_display_results() - print sensor buffer
 *
 */
void app_sensor_buffer_display_results( void );

/**
 * app_sensor_data_display_results() - print all sensor raw data
 *
 */
void app_sensor_data_display_results( void );

/**
 * app_sensor_data_display_results() - print sensor raw data by sensor type
 *
 * @param type   sensor type
 *
 */
void single_fact_sensor_display_results(uint8_t type);


//user data

/**
 * user_sensor_datas_set() - set user data to user data buffer
 *
 * @param buff   user datas
 *
 * @param len   user datas len(must be a 4bytes/group)
 *
 * @return    true: success, false: fail
 *
 */
bool user_sensor_datas_set(uint8_t *buff, uint8_t len);

/**
 * app_user_data_display_results() - print user data buffer
 *
 */
void app_user_data_display_results( void );

#endif
