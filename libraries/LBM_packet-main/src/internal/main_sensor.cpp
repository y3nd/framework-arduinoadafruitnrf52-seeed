#include "Lbm_packet.hpp"

#include <cstdio>
#include <cstdlib>

#include <grove_sensor.hpp>

#include <functional>
#include <tuple>
#include "internal/nrf_hal/gpio.hpp"
#include "internal/nrf_hal/gpiote.hpp"

int16_t temperture_val = 0;
uint16_t light_val = 0;

SensirionI2CSht4x sht4x;
MMA7660 accelemeter;

int adcin    = D6;
int adcvalue = 0;
float mv_per_lsb = 3600.0F/1024.0F; // 10-bit ADC with 3.6V input range
uint8_t sensor_types=0;
static constexpr int mma7660_IRQ_PIN        = D0;   // 
void mma7660_irq_callback(void)
{
	MMA7660_DATA data;
	accelemeter.getAllData(&data);
	// hal_mcu_trace_print("mma7660_irq:%x\r\n",data.TILT);
    state_all = state_all|TRACKER_STATE_BIT5_DEV_SHOCK;
}


void sensor_init_detect(void)
{
    void* context = 0;
    //power on 
    digitalWrite(PIN_POWER_SUPPLY_GROVE, HIGH);   //grove power on
    pinMode(PIN_POWER_SUPPLY_GROVE, OUTPUT);   

    delay(1000);
    Wire.begin();
    delay(1000);
    Wire.beginTransmission(SHT4X_I2C_ADDRESS);
    if ( 0 == Wire.endTransmission() )
    {
        sensor_types = sensor_types|sht40_sensor_type;
        sht4x.begin(Wire);
        delay(1000);
    }
    Wire.beginTransmission(MMA7660_ADDR);
    if ( 0 == Wire.endTransmission() )
    {
        sensor_types = sensor_types|mma7660_sensor_type;
        accelemeter.init(MMA7660_SHINTX|MMA7660_SHINTY|MMA7660_SHINTZ);  

        pinMode(mma7660_IRQ_PIN, INPUT_PULLUP);
        attachInterrupt(mma7660_IRQ_PIN, mma7660_irq_callback, ISR_DEFERRED | FALLING);        
    }
}
//get sensor data 
void sensor_datas_get(void)
{
    float light_vcc;
    if(sensor_types&sht40_sensor_type)
    {
        uint16_t error;    
        float temperature;
        float humidity;

        error = sht4x.measureHighPrecision(temperature, humidity);
        if (error == 0)
        {
            temperture_val = temperature*10;
        }
    }
    if(sensor_types&mma7660_sensor_type)
    {
        MMA7660_DATA data;
        accelemeter.getAllData(&data);
        if(data.TILT&0x80)
        {
            state_all = state_all|TRACKER_STATE_BIT5_DEV_SHOCK;
        }
    }
    adcvalue = analogRead(adcin);
    light_vcc = (float)(adcvalue * mv_per_lsb);
    light_val = 100*(light_vcc-30)/2000;        //percentage
    if(light_val<0) light_val = 0;
    if(light_val>100) light_val = 100;
    // hal_mcu_trace_print("Temperature:%d,light_val:%d\r\n",temperture_val,light_val);
}

