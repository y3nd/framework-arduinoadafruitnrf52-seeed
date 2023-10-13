#ifndef __AT_H__
#define __AT_H__

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "Lbm_packet.hpp"

#include "at_command.h"

extern uint8_t parse_cmd_type;  
extern void hal_mcu_trace_print( const char* fmt, ... );
extern void hal_ble_trace_print( const char* fmt, ... );

#define AT_PRINTF(...)     do{   if(parse_cmd_type == 1)\
                                {\
                                   hal_ble_trace_print (  __VA_ARGS__ );\
                                }\
                                hal_mcu_wait_ms(1);\
                            }while(0)
#define AT_PPRINTF(...)     do{   if(parse_cmd_type == 1)\
                                 {\
                                   hal_ble_trace_print (  __VA_ARGS__ );\
                                 }\
                                 hal_mcu_wait_ms(1);\
                            }while(0)


/* AT Command strings. Commands start with AT */
/* General commands */
#define AT_VER                "+VER"        

#define AT_CONFIG             "+CONFIG" 
#define AT_DEFPARAM           "+DEFPARAM" 

#define AT_SN                 "+SN"

/* LoRaWAN network management commands */
#define AT_TYPE       "+TYPE"       
#define AT_BAND       "+BAND"       
#define AT_CHANNEL    "+CHANNEL"   
#define AT_RETEY      "+RETRY" 
#define AT_DCODE      "+DCODE" 

/* Keys, IDs and EUIs management commands */
#define AT_JOINEUI    "+APPEUI"      
#define AT_NWKKEY     "+NWKKEY"      
#define AT_APPKEY     "+APPKEY"
#define AT_NWKSKEY    "+NWKSKEY"     
#define AT_APPSKEY    "+APPSKEY"     
#define AT_DADDR      "+DADDR"       
#define AT_DEUI       "+DEUI" 
#define AT_DKEY       "+DKEY"

#define AT_HARDWAREVER        "+HARDWAREVER"


#define AT_RESTORE          "+RESTORE"
#define AT_POS_INT          "+POS_INT"
#define AT_SAMPLE_INT       "+SAMPLE_INT"
#define AT_SENSOR           "+SENSOR" 
#define AT_PLATFORM         "+PLATFORM"
#define AT_MEA              "+MEA"  
#define AT_POS_DEL          "+POS_DEL"
#define AT_POS_MSG          "+POS_MSG"

#define AT_DISCONNECT       "+DISCONNECT" 


/* USER CODE BEGIN EM */
void hexTonum(unsigned char *out_data, unsigned char *in_data, unsigned short Size);
void numTohex(unsigned char *out_data, unsigned char *in_data, unsigned short Size);
uint8_t Char2Nibble(char Char);

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
/**
  * @brief  Return AT_OK in all cases
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_return_ok(const char *param);

/**
  * @brief  Return AT_ERROR in all cases
  * @param  param string of the AT command - unused
  * @retval AT_ERROR
  */
ATEerror_t AT_return_error(const char *param);

/* --------------- Application events --------------- */

/* --------------- General commands --------------- */

/* --------------- Keys, IDs and EUIs management commands --------------- */
/**
  * @brief  Print Join Eui
  * @param  param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_get(const char *param);

/**
  * @brief  Set Join Eui
  * @param  param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_JoinEUI_set(const char *param);

/**
  * @brief  Print Network Key
  * @param  param string of the AT command
  * @retval AT_OK
  */
ATEerror_t AT_NwkKey_get(const char *param);

/**
  * @brief  Set Network Key
  * @param  param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NwkKey_set(const char *param);

/**
  * @brief  Print Application Key
  * @param  param string of the AT command
  * @retval AT_OK
  */
ATEerror_t AT_AppKey_get(const char *param);

/**
  * @brief  Set Application Key
  * @param  param string of the AT command
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_AppKey_set(const char *param);

/**
  * @brief  Print Network Session Key
  * @param  param string of the AT command
  * @retval AT_OK
  */
ATEerror_t AT_NwkSKey_get(const char *param);

/**
  * @brief  Set Network Session Key
  * @param  param String pointing to provided DevAddr
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_NwkSKey_set(const char *param);

/**
  * @brief  Print Application Session Key
  * @param  param string of the AT command
  * @retval AT_OK
  */
ATEerror_t AT_AppSKey_get(const char *param);

/**
  * @brief  Set Application Session Key
  * @param  param String pointing to provided DevAddr
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_AppSKey_set(const char *param);

/**
  * @brief  Print the DevAddr
  * @param  param String pointing to provided DevAddr
  * @retval AT_OK
  */
ATEerror_t AT_DevAddr_get(const char *param);

/**
  * @brief  Set DevAddr
  * @param  param String pointing to provided DevAddr
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */
ATEerror_t AT_DevAddr_set(const char *param);

/**
  * @brief  Print Device EUI
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_DevEUI_get(const char *param);

/**
  * @brief  Set Device EUI
  * @param  param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_DevEUI_set(const char *param);

/* --------------- LoRaWAN join and send data commands --------------- */

/* --------------- LoRaWAN network management commands --------------- */
/**
  * @brief  Print the version of the AT_Slave FW
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_version_get(const char *param);
/**
  * @brief  Print the version of the device
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_config_ver_get(const char *param);

/**
  * @brief  Print actual Active Region
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_Region_get(const char *param);

/**
  * @brief  Set Active Region
  * @param  param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_Region_set(const char *param);

/**
  * @brief  Print actual Active Type
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_ActivationType_get(const char *param);

/**
  * @brief  Set Active Type
  * @param  param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_ActivationType_set(const char *param);

/**
  * @brief  Print channel group
  * @param  param string of the AT command - unused
  * @retval AT_OK
  */
ATEerror_t AT_ChannelGroup_get(const char *param);

/**
  * @brief  Set channel group
  * @param  param string of the AT command
  * @retval AT_OK if OK
  */
ATEerror_t AT_ChannelGroup_set(const char *param);

/**
  * @brief  Set Sensor Type
  * @param  param String pointing to provided param
  * @retval AT_OK if OK, or an appropriate AT_xxx error code
  */

ATEerror_t AT_DeviceClass_get(const char *param);

/**
  * @brief  get device paramenter
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Config_get(const char *param);

/**
  * @brief  get device paramenter
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DefParam_get(const char *param);
/**
  * @brief  set device code
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DevCODE_set(const char *param);
/**
  * @brief  get device code
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DevCODE_get(const char *param);

/**
  * @brief  set device serial number
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Sn_set(const char *param);
/**
  * @brief  get device serial number
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Sn_get(const char *param);
/**
  * @brief  set lora device key
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DeviceKey_set(const char *param);
/**
  * @brief  get lora device key
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DeviceKey_get(const char *param);

/**
  * @brief  get hardware version
  * @param  param String parameter
  * @retval AT_OK
  */

ATEerror_t AT_HARDWAREVER_get(const char *param);

/**
  * @brief  restore param
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Parameter_Reset(const char *param); 

/**
  * @brief  get position interval
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_POS_INT_get(const char *param);

/**
  * @brief  set position interval
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_POS_INT_set(const char *param);

/**
  * @brief  get sample interval
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_SAMPLE_INT_get(const char *param);

/**
  * @brief  set sample interval
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_SAMPLE_INT_set(const char *param); 

/**
  * @brief  get device type
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_SensorType_get(const char *param);

/**
  * @brief  set lora join net platform
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Platform_set(const char *param);
/**
  * @brief  get lora join net platform
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Platform_get(const char *param);

/**
  * @brief  get default DevEui
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_DefEUI_get(const char *param);

/**
  * @brief  collect sensor data
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_MeasurementValue_get(const char *param);

/**
  * @brief  delete history data
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_POS_DEL_run(const char *param);

/**
  * @brief  control ble disconnect 
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_Disconnect(const char *param);

/**
  * @brief  print position data 
  * @param  param String parameter
  * @retval AT_OK
  */
ATEerror_t AT_POS_MSG_run(const char *param);

#endif /* __AT_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
