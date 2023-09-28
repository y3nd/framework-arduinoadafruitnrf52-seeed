#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>


#include <functional>
#include <tuple>
#include "internal/nrf_hal/gpio.hpp"
#include "internal/nrf_hal/gpiote.hpp"

#include "LIS3DHTR.h"
#include "SensirionI2CSht4x.h"
#include "SensirionI2CSgp41.h"
#include "VOCGasIndexAlgorithm.h"
#include "Dps310.h"
#include "Si115X.h"
#include "Ultrasonic.h"

//IIC slave device addr
#define LIS3DHTR_I2C_ADDRESS                (LIS3DHTR_ADDRESS_UPDATED)
#define SHT4X_I2C_ADDRESS                   (0x44)
#define SI1151_I2C_ADDRESS                  (0x53)
#define SGP41_I2C_ADDRESS                   (0x59)
#define DP310_IIC_ADDRESS                   (0x77)


uint8_t factory_sensor_data_len = 20;           
uint8_t factory_sensor_data[20] = { 0 };        // for factory default sensor 

uint8_t user_sensor_data_len = 0;               // for the sensor that define by user
uint8_t user_sensor_data[40] = { 0 };

uint16_t sound_sample_index = 0;                // sound sensor sampling times before data was uploaded to lorawan
uint16_t sound_average_data = 0;

volatile bool iic_mutex_status = false;         // Ensure that devices are mutually exclusive when using the iic bus

uint32_t lis3dhtr_irq_count = 0;
float mv_per_lsb = 3600.0F/1024.0F;             // 10-bit ADC with 3.6V input range 
float acc_convert = 9.80F;                      // 1g = 9.8m/s2

uint32_t sensor_types = 0;                  
static float sampling_interval = 10.f;          // sampling period for sgp41

bool relay_switch_on = false;                   // relay

//Sensor raw data
float sht4x_temperature = 0.0;
float sht4x_humidity = 0.0;
float lis3dhtr_x = 0.0;
float lis3dhtr_y = 0.0;
float lis3dhtr_z = 0.0;
int32_t sgp41_voc_index = 0;
float UV_val = 0.0;
uint16_t sound_val = 0;
uint16_t ultrasonic_distance_cm = 0;
uint32_t dps310_pressure_val = 0;
    
volatile uint8_t lis3dhtr_irq_flag = 0;
uint8_t move_trig_position = 0;
uint8_t move_trig_collect = 0;
//sensor handle
static SensirionI2CSht4x sht4x;
static LIS3DHTR<TwoWire> LIS; 
static SensirionI2CSgp41 sgp41;
static Si115X si1151;
Dps310 Dps310PressureSensor = Dps310();

static VOCGasIndexAlgorithm voc_algorithm(sampling_interval);   // Used to calculate voc_index

//sensor irq pin
static constexpr int lis3dhtr_irq_pin       = PIN_LIS3DHTR_INT2;   

//digital pin
static constexpr int ultrasonic_pin    = D0;
static Ultrasonic ultrasonic(ultrasonic_pin);

static constexpr int relay_pin    = D2;
//analog pin
static constexpr int sound_adc_pin    = A0;

//irq callback----------------------------------------------------
void lis3dhtr_irq_callback(void)
{
    lis3dhtr_irq_count ++;
    lis3dhtr_irq_flag = 1;
    if((move_trig_position == 0) && (move_trig_collect == 0))
    {
        move_trig_position = 1;
        move_trig_collect = 1;
    }
}

//grove power init -----------------------------------------------------
void grove_init_power(void)
{
    //power on 
    digitalWrite(PIN_POWER_SUPPLY_GROVE, HIGH);   //grove power on
    pinMode(PIN_POWER_SUPPLY_GROVE, OUTPUT);   
    delay(100);
}

//sensor available  detect-----------------------------------------------------
void sensor_available_detect(void)
{
    Wire.begin();
	for (int addr = 1; addr < 128; addr++)
	{
		Wire.beginTransmission(addr);
		if ( 0 == Wire.endTransmission() )
		{
			if(addr==SHT4X_I2C_ADDRESS)
			{
				sensor_types = sensor_types|sht4x_sensor_type;
			}
			else if(addr==LIS3DHTR_I2C_ADDRESS)
			{
				sensor_types = sensor_types|lis3dhtr_sensor_type;
			}
			else if(addr==SI1151_I2C_ADDRESS )
			{
				sensor_types = sensor_types|si1151_sensor_type;
			}
			else if(addr==SGP41_I2C_ADDRESS)
			{
				sensor_types = sensor_types|sgp41_sensor_type;
			}
			else if(addr==DP310_IIC_ADDRESS)
			{
				sensor_types = sensor_types|dps310_sensor_type;
			}
			else
			{

			}
            hal_mcu_trace_print("Found: 0x%02x   ",addr);
		}
	}
    hal_mcu_trace_print("\r\n");
}

bool iic_slave_available_detect(uint8_t slave_addr)
{
    if(slave_addr<1 || slave_addr >127)
    {
        return false;
    }
    Wire.beginTransmission(slave_addr);
    if ( 0 == Wire.endTransmission() )
    {
        return true;
    } 
    return false;  
}

bool sht4x_available_detect(void)
{
    if(iic_slave_available_detect(SHT4X_I2C_ADDRESS))
    {
        sensor_types = sensor_types|sht4x_sensor_type;
        return true;
    } 
    return false;  
}
bool lis3dhtr_available_detect(void)
{
    if(iic_slave_available_detect(LIS3DHTR_I2C_ADDRESS))
    {
        sensor_types = sensor_types|lis3dhtr_sensor_type;
        return true;
    } 
    return false;  
}
bool si1151_available_detect(void)
{
    if(iic_slave_available_detect(SI1151_I2C_ADDRESS))
    {
        sensor_types = sensor_types|si1151_sensor_type;
        return true;
    } 
    return false;  
}
bool sgp41_available_detect(void)
{
    if(iic_slave_available_detect(SGP41_I2C_ADDRESS))
    {
        sensor_types = sensor_types|sgp41_sensor_type;
        return true;
    } 
    return false;  
}
bool dps310_available_detect(void)
{
    if(iic_slave_available_detect(DP310_IIC_ADDRESS))
    {
        sensor_types = sensor_types|dps310_sensor_type;
        return true;
    } 
    return false; 
}

//hardwar init-----------------------------------------------------
void sensor_init_detect(void)
{
    //power on 
    grove_init_power();

    //found available sensor
    sensor_available_detect();

    #if(SOUND_SENSOR_AVAILABLE == 1)
    sensor_types = sensor_types|sound_sensor_type;
    #endif
    #if(ULTRASONIC_SENSOR_AVAILABLE == 1)
    sensor_types = sensor_types|ultrasonic_sensor_type;  
    #endif

    //sht4x
    sensor_sht4x_init();    

    //lis3dhtr 
    sensor_lis3dhtr_init(); 

    //sgp41
    sensor_sgp41_init();    

    //si1151
    sensor_si1151_init();

    //dps310
    sensor_dps310_init();
}

//sht4x
void sensor_sht4x_init(void)
{
    if(sensor_types&sht4x_sensor_type)
    {
        sht4x.begin(Wire);
    }
}

//lis3dhtr
void sensor_lis3dhtr_init(void)
{
    if(sensor_types&lis3dhtr_sensor_type)
    {
        pinMode(lis3dhtr_irq_pin, INPUT);
        attachInterrupt(lis3dhtr_irq_pin, lis3dhtr_irq_callback, ISR_DEFERRED | FALLING); //RISING

        LIS.begin(Wire, LIS3DHTR_ADDRESS_UPDATED); 
        LIS.setInterrupt();     
    }
}

//sgp41
void sensor_sgp41_init(void)
{
    if(sensor_types&sgp41_sensor_type) 
    {
	    sgp41.begin(Wire);
    }
}

//si1151
void sensor_si1151_init(void)
{
    if(sensor_types&si1151_sensor_type) 
    {
        si1151.Begin();
    }
}
//dps310
void sensor_dps310_init(void)
{
    if(sensor_types&dps310_sensor_type) 
    {
	    Dps310PressureSensor.begin(Wire);
    }
}


//get sensor data  -----------------------------------------------------

//sht4x--------------------
//Consume  12ms
bool sht4x_data_get(float* temperature, float* humidity)    // 11ms
{
    if(iic_mutex_status == true)
        return false;
    iic_mutex_status = true;

    if(sensor_types&sht4x_sensor_type)
    {
        uint16_t error;    
        error = sht4x.measureHighPrecision(*temperature, *humidity);
        if (error == 0)
        {
            uint8_t humi = *humidity;
            if(humi >= 100) //Prevents the humidity calculation from being greater than 100%
            {
                *humidity = 100.f;
            }
            iic_mutex_status = false;
            return true;
        }
    }
    else
    {
        iic_mutex_status = false;
        return false;
    }
    iic_mutex_status = false;
    return true;
}

//lis3dhtr--------------------
//Consume  1ms
bool lis3dhtr_data_get(float* x, float* y, float* z)
{
    static uint32_t irq_count  = 0;
    if(iic_mutex_status == true)
        return false;
    iic_mutex_status = true;

    if(sensor_types&lis3dhtr_sensor_type)
    {
        uint8_t flag = 0;
        LIS.getIntStatus(&flag);
        LIS.getAcceleration(x, y, z);
        if(irq_count != lis3dhtr_irq_count)
        {
            irq_count = lis3dhtr_irq_count;
            state_all = state_all|TRACKER_STATE_BIT5_DEV_SHOCK;
            app_task_track_motion_index_update();
        }
        else
        {
            state_all = state_all|TRACKER_STATE_BIT4_DEV_STATIC;
        }
        iic_mutex_status = false;
        return true;   
    }
    else
    {
        iic_mutex_status = false;
        return false;
    }
    iic_mutex_status = false;
    return true;
}

//sgp41--------------------
//Consume  250ms
bool sgp41_data_get(float temperature, float humidity, int32_t* voc_index)
{
    uint16_t compensationT;
    uint16_t compensationRh;

    if(iic_mutex_status == true)
        return false;
    iic_mutex_status = true;

#if(SENSOR_HOT_SWAPPABLE_DETECT == 1)
    if(sgp41_available_detect())
    {
        sensor_sgp41_init();          
    }  
#endif

    //temperature and humidity for SGP internal compensation
    if(sensor_types&sht4x_sensor_type) 
    {
        // convert temperature and humidity to ticks as defined by sgp41
        // interface
        // NOTE: in case you read RH and T raw signals check out the
        // ticks specification in the datasheet, as they can be different for
        // different sensors
        compensationT = static_cast<uint16_t>((temperature + 45) * 65535 / 175);
        compensationRh = static_cast<uint16_t>(humidity * 65535 / 100);
    }
    else
    {
        compensationRh = 0x8000;  // initialized to default value in ticks as defined by sgp41
        compensationT = 0x6666;  // initialized to default value in ticks as defined by sgp41        
    }
    if(sensor_types&sgp41_sensor_type) 
    {
        uint16_t srawVoc = 0;
        uint16_t srawNox = 0;
        uint16_t error;
        // Request a first measurement to heat up the plate (ignoring the result)
        error = sgp41.measureRawSignals(compensationRh, compensationT, srawVoc,srawNox);
        if (error) 
        {
            iic_mutex_status = false;
            return false;
        }
        delay(140);
        // Request the measurement values
        error = sgp41.measureRawSignals(compensationRh, compensationT, srawVoc,srawNox);
        if (error) 
        {
            iic_mutex_status = false;
            return false;
        }  
        error = sgp41.turnHeaterOff(); //for low power consumption
        if (error) 
        {
            iic_mutex_status = false;
            return false;
        }
        // Process raw signals by Gas Index Algorithm to get the VOC index values
        *voc_index = voc_algorithm.process(srawVoc); 
        iic_mutex_status = false; 
        return true;       
    }
    else
    {
        iic_mutex_status = false;
        return false;
    }
    iic_mutex_status = false;
    return true;
}

//In order to calculate the voc_index, it is necessary to periodically sample sgp41
float sgp41_sampling_period_get(void)
{
	return voc_algorithm.get_sampling_interval();
}


//si1151--------------------
//consume 4ms
bool  si1151_data_get(float* UV_val)
{
    if(iic_mutex_status == true)
        return false;
    iic_mutex_status = true;
#if(SENSOR_HOT_SWAPPABLE_DETECT == 1)
    if(si1151_available_detect())
    {
        sensor_si1151_init();          
    }  
#endif
    if(sensor_types&si1151_sensor_type) 
    {
        *UV_val = si1151.ReadHalfWord_UV();
        iic_mutex_status = false;
        return true;
    }
    else
    {
        iic_mutex_status = false;
        return false;
    }
    iic_mutex_status = false;
    return true;
}


//sound sensor--------------------
//consume 30ms
bool  sound_data_get(uint16_t* sound_val)
{
    static bool init_average = false;
    static uint8_t index = 1;   

    uint32_t sum = 0;
    int  adc_val = 0;
    uint16_t temp_val = 0;

    if(sensor_types&sound_sensor_type) 
    {
        for(int i=0; i<16; i++)
        {
            sum += analogRead(sound_adc_pin);
            delay(2);
        }
        adc_val = sum >>4;
        *sound_val = (uint16_t)((float)adc_val * mv_per_lsb);

        //average
        temp_val = *sound_val;
        sum = sound_average_data*sound_sample_index;

        sound_sample_index ++;
        sound_average_data = (sum+temp_val)/sound_sample_index;

        return true;
    }
    return false;    
}
uint16_t  sound_average_get(void)
{

    return sound_average_data;
}

//ultrasonic sensor--------------------
//less than 40ms
bool  ultrasonic_distance_get(uint16_t* distance_cm)
{
    if(sensor_types&ultrasonic_sensor_type) 
    {
        *distance_cm = ultrasonic.MeasureInCentimeters(40000);     //40ms timeout 4m  => 4*2 /340 *1000 = 23.5ms   Velocity of sound:340m/s
        if(*distance_cm > 400)
            *distance_cm = 0xFFFF;
        return true;
    }
    return false; 
}

//DPS310 sensor--------------------
//consume 219ms
bool  dps310_data_get(uint32_t* pressure_val)
{
	float Detection_pressure;
	uint8_t oversampling = 7;
	int16_t ret;
	int i;
	int size = 10;

    if(iic_mutex_status == true)
        return false;
    iic_mutex_status = true;

#if(SENSOR_HOT_SWAPPABLE_DETECT == 1)
    if(dps310_available_detect())
    {
        sensor_dps310_init();          
    }  
#endif
	if(sensor_types&dps310_sensor_type) 
    {
        ret = Dps310PressureSensor.measurePressureOnce(Detection_pressure, oversampling);
        *pressure_val = Detection_pressure;
        iic_mutex_status = false;
        return true;
    }
    else
    {
        iic_mutex_status = false;
        return false;
    }
    iic_mutex_status = false;
    return true;
}

//factory sensor--------------------
void sensor_datas_get(void)
{
    //temperture & humidity
    if(sht4x_data_get(&sht4x_temperature, &sht4x_humidity))
    {
        int16_t temp = sht4x_temperature*10;
        uint8_t humi = sht4x_humidity;
        memcpyr( factory_sensor_data, ( uint8_t * )( &temp ), 2 );  //temperature

        factory_sensor_data[2] = humi;                              //humidity
    }
    else
    {
        factory_sensor_data[0] = 0x80;
        factory_sensor_data[1] = 0x00;

        factory_sensor_data[2] = 0x80;
    }

    //Atmospheric pressure
    if(dps310_data_get(&dps310_pressure_val))
    {
        factory_sensor_data[3] = (dps310_pressure_val>>16)&0xFF;    //pressure
        factory_sensor_data[4] = (dps310_pressure_val>>8)&0xFF;
        factory_sensor_data[5] = dps310_pressure_val&0xFF;        
    }
    else
    {
        //pressure
        factory_sensor_data[3] = 0x80;
        factory_sensor_data[4] = 0x00;
        factory_sensor_data[5] = 0x00;
    }

    //ultrasonic
    if(ultrasonic_distance_get(&ultrasonic_distance_cm))
    {
        memcpyr( &factory_sensor_data[6], ( uint8_t * )( &ultrasonic_distance_cm ), 2 );    //distance
    } 
    else
    {
        //distance
        factory_sensor_data[6] = 0x80;
        factory_sensor_data[7] = 0x00;
    }

    //UV
    if(si1151_data_get(&UV_val))
    {
        uint16_t temp_val = UV_val*100;
        memcpyr( &factory_sensor_data[8], ( uint8_t * )( &temp_val ), 2 );  //UV
    }
    else
    {
        //uv
        factory_sensor_data[8] = 0x80;
        factory_sensor_data[9] = 0x00;
    }

    //voc index
    if(sgp41_data_get(sht4x_temperature, sht4x_humidity, &sgp41_voc_index))
    {
        factory_sensor_data[10] = (sgp41_voc_index>>8)&0xFF;        //voc_index
        factory_sensor_data[11] = sgp41_voc_index&0xFF;
    }
    else
    {
        
        factory_sensor_data[10] = 0x80;
        factory_sensor_data[11] = 0x00;
    }

    //sound
    if(sound_data_get(&sound_val))
    {
        sound_val = sound_average_get();
        memcpyr( &factory_sensor_data[12], ( uint8_t * )( &sound_val ), 2 );    //sound vcc
    }
    else
    {
        
        factory_sensor_data[12] = 0x80;
        factory_sensor_data[13] = 0x00;
    }

    //3-axis
    if(lis3dhtr_data_get(&lis3dhtr_x, &lis3dhtr_y, &lis3dhtr_z))
    {
        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;

        acc_x = 100*lis3dhtr_x*acc_convert;
        acc_y = 100*lis3dhtr_y*acc_convert;
        acc_z = 100*lis3dhtr_z*acc_convert;

        memcpyr( &factory_sensor_data[14], ( uint8_t * )( &acc_x ), 2 );    //x-axis
        memcpyr( &factory_sensor_data[16], ( uint8_t * )( &acc_y ), 2 );    //y-axis
        memcpyr( &factory_sensor_data[18], ( uint8_t * )( &acc_z ), 2 );    //z-axis
    }
    else
    {
        
        factory_sensor_data[14] = 0x80;
        factory_sensor_data[15] = 0x00;
        
        factory_sensor_data[16] = 0x80;
        factory_sensor_data[17] = 0x00;
        //z-axis
        factory_sensor_data[18] = 0x80;
        factory_sensor_data[19] = 0x00;
    }
    factory_sensor_data_len = 20;
}


//
bool single_fact_sensor_data_get(uint8_t type)
{
    switch(type)
    {
        case  sht4x_sensor_type:
            //temperture & humidity
            if(sht4x_data_get(&sht4x_temperature, &sht4x_humidity))
            {
                return true;
            }
            break;
        case  lis3dhtr_sensor_type:
            if(lis3dhtr_data_get(&lis3dhtr_x, &lis3dhtr_y, &lis3dhtr_z))
            {
                return true;
            }
            break;
        case  dps310_sensor_type:
            if(dps310_data_get(&dps310_pressure_val))
            {
                return true;
            }
            break;
        case  sgp41_sensor_type:
            if(sgp41_data_get(sht4x_temperature, sht4x_humidity, &sgp41_voc_index))
            {
                return true;
            }
            break;
        case  si1151_sensor_type:
            if(si1151_data_get(&UV_val))
            {
                return true;
            }
            break;
        case sound_sensor_type:
            if(sound_data_get(&sound_val))
            {
                return true;
            }
            break;
        case ultrasonic_sensor_type:
            if(ultrasonic_distance_get(&ultrasonic_distance_cm))
            {
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}


void single_fact_sensor_display_results(uint8_t type)
{
    switch(type)
    {
        case  sht4x_sensor_type:
            //temperture & humidity
            if(sensor_types&sht4x_sensor_type)
            {
                printf("temperature:%0.2f, humidity:%0.2f%%\r\n",sht4x_temperature, sht4x_humidity);        
            }
            break;
        case  lis3dhtr_sensor_type:
            //3-axis
            if(sensor_types&lis3dhtr_sensor_type)
            {
                printf("x:%0.2f y:%0.2f z:%0.2f\r\n",lis3dhtr_x,lis3dhtr_y,lis3dhtr_z);
            }
            break;
        case  dps310_sensor_type:
            //pressure
            if(sensor_types&dps310_sensor_type)
            {
                printf("pressure value:%u\r\n",dps310_pressure_val);
            }
            break;
        case  sgp41_sensor_type:
            //voc index
            if(sensor_types&sgp41_sensor_type)
            {
                printf("voc_index:%d\r\n",sgp41_voc_index); 
            }
            break;
        case  si1151_sensor_type:
            //uv
            if(sensor_types&si1151_sensor_type )
            {
                printf("UV val:%0.2f\r\n",UV_val); 
            }
            break;
        case sound_sensor_type:
            //sound
            printf("sound_val:%d mV\r\n",sound_val);
            break;
        case ultrasonic_sensor_type:
            //distance
            printf("distance:%d cm\r\n",ultrasonic_distance_cm);
            break;
        default:
            break;
    }
}

void factory_sensor_data_combined(void)
{
    //temperture & humidity
    if(sensor_types&sht4x_sensor_type)
    {
        int16_t temp = sht4x_temperature*10;
        uint8_t humi = sht4x_humidity;
        memcpyr( factory_sensor_data, ( uint8_t * )( &temp ), 2 );  //temperature

        factory_sensor_data[2] = humi;                              //humidity
    }
    else
    {
        factory_sensor_data[0] = 0x80;
        factory_sensor_data[1] = 0x00;

        factory_sensor_data[2] = 0x80;
    }

    //Atmospheric pressure
	if(sensor_types&dps310_sensor_type) 
    {
        factory_sensor_data[3] = (dps310_pressure_val>>16)&0xFF;    //pressure
        factory_sensor_data[4] = (dps310_pressure_val>>8)&0xFF;
        factory_sensor_data[5] = dps310_pressure_val&0xFF;        
    }
    else
    {
        //pressure
        factory_sensor_data[3] = 0x80;
        factory_sensor_data[4] = 0x00;
        factory_sensor_data[5] = 0x00;
    }

    //ultrasonic
    if((sensor_types&ultrasonic_sensor_type) && (ultrasonic_distance_cm != 0xFFFF)) 
    {
        memcpyr( &factory_sensor_data[6], ( uint8_t * )( &ultrasonic_distance_cm ), 2 );    //distance  
    }
    else
    {
        //distance
        factory_sensor_data[6] = 0x80;
        factory_sensor_data[7] = 0x00;
    }

    //UV
    if(sensor_types&si1151_sensor_type) 
    {
        uint16_t temp_val = UV_val*100;
        memcpyr( &factory_sensor_data[8], ( uint8_t * )( &temp_val ), 2 );  //UV
    }
    else
    {
        //uv
        factory_sensor_data[8] = 0x80;
        factory_sensor_data[9] = 0x00;
    }

    //voc index
    if(sensor_types&sgp41_sensor_type) 
    {
        factory_sensor_data[10] = (sgp41_voc_index>>8)&0xFF;        //voc_index
        factory_sensor_data[11] = sgp41_voc_index&0xFF;
    }
    else
    {
        
        factory_sensor_data[10] = 0x80;
        factory_sensor_data[11] = 0x00;
    }

    //sound
    if(sensor_types&sound_sensor_type) 
    {
        sound_val = sound_average_get();
        memcpyr( &factory_sensor_data[12], ( uint8_t * )( &sound_val ), 2 );    //sound vcc
    }
    else
    {
        factory_sensor_data[10] = 0x80;
        factory_sensor_data[11] = 0x00;
    }

    //3-axis
    if(sensor_types&lis3dhtr_sensor_type)
    {
        int16_t acc_x;
        int16_t acc_y;
        int16_t acc_z;

        acc_x = 100*lis3dhtr_x*acc_convert;
        acc_y = 100*lis3dhtr_y*acc_convert;
        acc_z = 100*lis3dhtr_z*acc_convert;

        memcpyr( &factory_sensor_data[14], ( uint8_t * )( &acc_x ), 2 );    //x-axis
        memcpyr( &factory_sensor_data[16], ( uint8_t * )( &acc_y ), 2 );    //y-axis
        memcpyr( &factory_sensor_data[18], ( uint8_t * )( &acc_z ), 2 );    //z-axis
    }
    else
    {
        
        factory_sensor_data[14] = 0x80;
        factory_sensor_data[15] = 0x00;
        
        factory_sensor_data[16] = 0x80;
        factory_sensor_data[17] = 0x00;
        //z-axis
        factory_sensor_data[18] = 0x80;
        factory_sensor_data[19] = 0x00;
    }    
}

void app_sensor_buffer_display_results( void )
{
    printf("sensor datas:");
    for(uint8_t u8i = 0; u8i < 20; u8i++ )
    {
        printf("%02x ",factory_sensor_data[u8i]);
    }
    printf("\r\n");  
}

void app_sensor_data_display_results( void )
{
    //temperture & humidity
    if(sensor_types&sht4x_sensor_type)
    {
        printf("temperature:%0.2f, humidity:%0.2f%%\r\n",sht4x_temperature, sht4x_humidity);        
    }

    //pressure
    if(sensor_types&dps310_sensor_type)
    {
        printf("pressure value:%u\r\n",dps310_pressure_val);
    }

    //distance
    printf("distance:%d cm\r\n",ultrasonic_distance_cm);

    //uv
    if(sensor_types&si1151_sensor_type )
    {
        printf("UV val:%0.2f\r\n",UV_val); 
    }

    //voc index
    if(sensor_types&sgp41_sensor_type)
    {
        printf("voc_index:%d\r\n",sgp41_voc_index); 
    }

    //sound
    
    printf("sound_val:%d mV\r\n",sound_val);

    //3-axis
    if(sensor_types&lis3dhtr_sensor_type)
    {
        printf("x:%0.2f y:%0.2f z:%0.2f\r\n",lis3dhtr_x,lis3dhtr_y,lis3dhtr_z);
    }
}

void relay_status_control( bool switch_on )
{
    if(switch_on)
    {
        digitalWrite(relay_pin, HIGH);   
        pinMode(relay_pin, OUTPUT); 
        relay_switch_on = true;
    }
    else
    {
        digitalWrite(relay_pin, LOW);   
        pinMode(relay_pin, OUTPUT); 
        relay_switch_on = false;        
    }
}
bool relay_status_on( void )
{
    return relay_switch_on;
}


//user sensor--------------------

bool user_sensor_datas_set(uint8_t *buff, uint8_t len)
{
    //it's must be 4bytes/group
    if((len > 40) || (len%4!=0))
    {
        return false;
    }
    else if(len != 0)
    {
        memcpy(user_sensor_data,buff,len);
        user_sensor_data_len = len;  
        return true;   
    }
    else
    {
        return true; 
    }
    return true;   
}

void app_user_data_display_results( void )
{
    printf("user sensor datas:");
    for(uint8_t u8i = 0; u8i < user_sensor_data_len; u8i++ )
    {
        printf("%02x",user_sensor_data[u8i]);
    }
    printf("\r\n");  
}




















