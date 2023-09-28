#include "default_config_datas.h"
#include "app_config_param.h"

#include "internal/nrf_hal/system.hpp"
#include "internal/nrf_hal/trace.hpp"
#include "internal/Wm1110Hardware.hpp"
#include "Lbm_packet.hpp"

//it is lorawan factory setting,be Be carefully don't erase
static constexpr uintptr_t FLASH_FACT_END_ADDR = 0xE9000;    // Look at nrf52840_s140_v6.ld

static const uint32_t FlashFactConfigPages = FLASH_FACT_END_ADDR / Wm1110Hardware::getInstance().flash.PAGE_SIZE - 1;


void lorawan_param_init(void)
{
    app_param_t param_temp;
    uint8_t check_crc = 0;
    uint8_t check_len = 0;

    memset(&param_temp,0,sizeof(app_param_t));

    default_config_read(&param_temp,sizeof(app_param_t));
    check_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    if(param_temp.param_len != check_len)
    {
        save_Config();
        return;
    }
    check_crc = crc8((uint8_t *)&param_temp, param_temp.param_len);
    if(param_temp.crc != check_crc )
    {
        save_Config(); 
        return;
    }
    memcpy(&app_param,&param_temp,check_len);
}

int default_config_read (void *buffer, uint8_t size)
{
    const uint32_t page = FlashFactConfigPages;

    Wm1110Hardware::getInstance().flash.read(page, buffer, size);
    return 0;
}


int default_config_write (uint8_t *buffer, uint8_t size)
{
    const uint32_t page = FlashFactConfigPages;
    Wm1110Hardware::getInstance().flash.write(page, buffer, size);
    return 0;
}

bool save_Config(void)
{
    app_param.param_len = ((uint8_t *)&app_param.crc-(uint8_t *)&app_param.param_len);
    app_param.crc = crc8((uint8_t *)&app_param, app_param.param_len);
    default_config_write((uint8_t*)&app_param,sizeof(app_param_t));
    return true;
}

void get_Config(void)
{
    default_config_read(&app_param,sizeof(app_param_t));
}


uint8_t crc8(uint8_t *data, uint8_t data_len)
{
	uint8_t data_in;
	uint8_t i;
	uint8_t crc = 0x00;//CRC
	uint8_t crc_poly = 0x07;//CRC Ployֵ
	while (data_len--)
	{
		data_in = *data++;
		crc = crc ^ data_in;//CRC
		for (i = 0; i < 8; i++)
		{
			if (crc & 0x80)//
			{
				crc = (crc << 1) ^ crc_poly;
			}
			else //
			{
				crc = crc << 1;
			}
		}
	}
	
	return crc ^0x00;
}














