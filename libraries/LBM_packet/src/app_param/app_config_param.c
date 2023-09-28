#include "app_config_param.h"
#include "common/lorawan_key_config.h"
    
app_param_t app_param = { 
      .lora_info = {
            .ActiveRegion       = 5,                                // 0: AS923, 1: AU915, 2: CN470, 3: CN779, 4: EU433, 5: EU868, 6: KR920, 
                                                                    // 7: IN865, 8: US915, 9: RU864, 10: AS923_1, 11: AS923_2, 12: AS923_3, 13: AS923_4
            .ActivationType     = 2,                                // 0: none, 1: ABP, 2: OTAA
            .ChannelGroup       = 1,                                // 0 - 7, only for AU915 and US915
            .DevAddr            = 0x00000000,
            .DeviceCode         = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .DevEui             = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .JoinEui            = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .AppKey             = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},

            .DeviceKey          = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .AppSKey            = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
            .NwkSKey            = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
      },
      .hardware_info = {
            .Sn                 = {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},       
            .hw_ver             = 1
      },   
};
utc_param_t app_utc_param = 
{
    .sync_utc = 0,
};
adding_pos_param_t app_adding_pos_param = 
{
    .latitude = 22577064,
    .longitude = 113922456,
};

pos_msg_file_id_t pos_msg_file_id = 
{
  .reboot_cnt = 0,
  .log_cnt = 0,
}
;
pos_msg_param_t pos_msg_param =
{
    0
};

gnss_group_param_t app_gnss_group_param=
{
    .group_id = 0,
};


append_param_t app_append_param = 
{
    .Platform = IOT_PLATFORM_SENSECAP_TTN,         // 0: SENSECAP_TTN, 1: OTHER, 2: Helium, 3: TTN, 4:SENSECAP_HELIUM      
    .Retry = 1,
    .cache_en = true,
    .position_interval = 5,
    .sample_interval = 5,
};

