#ifndef _APP_CONFIG_PARAM_H
#define _APP_CONFIG_PARAM_H


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#define LORAWAN_APP_PARAM_ENABLE

typedef enum eLoRaMacRegion
{
    LORAMAC_REGION_TTN_AS923_2 = 0, //TTN AS923_2
    LORAMAC_REGION_AU915,  //Australian band on 915MHz
    LORAMAC_REGION_CN470, //Chinese band on 470MHz
    LORAMAC_REGION_CN779, //Chinese band on 779MHz
    LORAMAC_REGION_EU433, //European band on 433MHz
    LORAMAC_REGION_EU868, //European band on 868MHz
    LORAMAC_REGION_KR920, //South korean band on 920MHz
    LORAMAC_REGION_IN865, //India band on 865MHz
    LORAMAC_REGION_US915, //North american band on 915MHz
    LORAMAC_REGION_RU864, //Russia band on 864MHz

#ifdef LORAWAN_APP_PARAM_ENABLE
    LORAMAC_REGION_AS923_1, //AS band on 923MHz
    LORAMAC_REGION_AS923_2, //AS band on 923MHz
    LORAMAC_REGION_AS923_3, //AS band on 923MHz
    LORAMAC_REGION_AS923_4, //AS band on 923MHz
    LORAMAC_REGION_AS923_1B, //Helium AS923_1B
    LORAMAC_REGION_TTN_AS923_1, //TTN AS923_1
    LORAMAC_REGION_AS_920_923_LBT,
#endif

    LORAMAC_REGION_MAX = 0xFF//none 
}LoRaMacRegion_t;

typedef enum eActivationType
{
    ACTIVATION_TYPE_NONE = 0,  //None
    ACTIVATION_TYPE_ABP = 1,  //Activation By Personalization (ACTIVATION_TYPE_ABP)
    ACTIVATION_TYPE_OTAA = 2, //Over-The-Air Activation (ACTIVATION_TYPE_OTAA)
}ActivationType_t;

//

typedef struct lora_info
{
  //LoRaWAN
  uint8_t  DevEui[8];
  uint8_t  JoinEui[8];
  uint8_t  DeviceKey[16];
  uint8_t  AppKey[16];      

  uint8_t  ActiveRegion;                 //Regional spectrum in LoRaMacRegion_t
  uint8_t  ActivationType;               //access net type 0: none,1: ABP 2: OTAA
  uint8_t  ChannelGroup;
    
  uint32_t  DevAddr;
  uint8_t  DeviceCode[8];

  uint8_t  NwkKey[16];
  uint8_t  AppSKey[16];
  uint8_t  NwkSKey[16]; 

}lora_info_t;

typedef struct hardware_info
{
    uint8_t   Sn[9];
    uint8_t   hw_ver;
}hardware_info_t;

#pragma pack(4)
typedef struct app_param 
{
    uint8_t param_len;
    lora_info_t lora_info;
    hardware_info_t hardware_info;
    uint8_t crc;
}app_param_t;

typedef struct utc_param 
{
  uint32_t   sync_utc;
}utc_param_t;

typedef struct gnss_group_param 
{
  uint32_t   group_id;
}gnss_group_param_t;


typedef struct adding_pos_param
{
  int32_t   latitude;
  int32_t   longitude;  
}adding_pos_param_t;

#pragma pack()

extern app_param_t app_param;

extern utc_param_t app_utc_param; 
extern adding_pos_param_t app_adding_pos_param;
extern gnss_group_param_t app_gnss_group_param;

void hexTonum(unsigned char *out_data, unsigned char *in_data, unsigned short Size);
void numTohex(unsigned char *out_data, unsigned char *in_data, unsigned short Size);
uint8_t Char2Nibble(char Char);


#endif

