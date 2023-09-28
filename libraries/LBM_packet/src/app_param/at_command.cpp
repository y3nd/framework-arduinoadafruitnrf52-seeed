#include <stdint.h>
#include <string.h>

#include "at.h"
#include "at_command.h"
#include "default_config_datas.h"




/**
 * @brief  Array corresponding to the description of each possible AT Error
 */
static const char *const ATError_description[] =
{
        "\r\nOK\r\n",                      /* AT_OK */
        "\r\nAT_ERROR\r\n",                /* AT_ERROR */
        "\r\nAT_PARAM_ERROR\r\n",          /* AT_PARAM_ERROR */
        "\r\nAT_BUSY_ERROR\r\n",           /* AT_BUSY_ERROR */
        "\r\nAT_TEST_PARAM_OVERFLOW\r\n",  /* AT_TEST_PARAM_OVERFLOW */
        "\r\nAT_NO_NETWORK_JOINED\r\n",    /* AT_NO_NET_JOINED */
        "\r\nAT_RX_ERROR\r\n",             /* AT_RX_ERROR */
        "\r\nAT_NO_CLASS_B_ENABLE\r\n",    /* AT_NO_CLASS_B_ENABLE */
        "\r\nAT_DUTYCYCLE_RESTRICTED\r\n", /* AT_DUTYCYCLE_RESTRICTED */
        "\r\nAT_CRYPTO_ERROR\r\n",         /* AT_CRYPTO_ERROR */
        "\r\nAT_SAVE_FAILED\r\n",          /* AT_SAVE_FAILED */
        "\r\nAT_READ_FAILED\r\n",          /* AT_READ_ERROR */
        "\r\nAT_DELETE_ERROR\r\n",          /* AT_READ_ERROR */        
        "\r\nerror unknown\r\n",           /* AT_MAX */
};

/**
 * @brief  Array of all supported AT Commands
 */
static const struct ATCommand_s ATCommand[] =
{
        /* Keys, IDs and EUIs management commands */
        {
            .string = AT_JOINEUI,
            .size_string = sizeof(AT_JOINEUI) - 1,
            .get = AT_JoinEUI_get,
            .set = AT_JoinEUI_set,
            .run = AT_return_error,
        },
        {
            .string = AT_NWKKEY,
            .size_string = sizeof(AT_NWKKEY) - 1,
            .get = AT_NwkKey_get,
            .set = AT_NwkKey_set,
            .run = AT_return_error,
        },        
        {
            .string = AT_APPKEY,
            .size_string = sizeof(AT_APPKEY) - 1,
            .get = AT_AppKey_get,
            .set = AT_AppKey_set,
            .run = AT_return_error,
        },
        {
            .string = AT_NWKSKEY,
            .size_string = sizeof(AT_NWKSKEY) - 1,
            .get = AT_NwkSKey_get,
            .set = AT_NwkSKey_set,
            .run = AT_return_error,
        },  
        {
            .string = AT_APPSKEY,
            .size_string = sizeof(AT_APPSKEY) - 1,
            .get = AT_AppSKey_get,
            .set = AT_AppSKey_set,
            .run = AT_return_error,
        },        
        {
            .string = AT_DADDR,
            .size_string = sizeof(AT_DADDR) - 1,
            .get = AT_DevAddr_get,
            .set = AT_DevAddr_set,
            .run = AT_return_error,
        },

        {
            .string = AT_DEUI,
            .size_string = sizeof(AT_DEUI) - 1,
            .get = AT_DevEUI_get,
            .set = AT_DevEUI_set,
            .run = AT_return_error,
        },

        /* LoRaWAN network management commands */
        {
            .string = AT_VER,
            .size_string = sizeof(AT_VER) - 1,
            .get = AT_version_get,
            .set = AT_return_error,
            .run = AT_return_error,
        },
        {
            .string = AT_BAND,
            .size_string = sizeof(AT_BAND) - 1,
            .get = AT_Region_get,
            .set = AT_Region_set,
            .run = AT_return_error,
        },

        {
            .string = AT_TYPE,
            .size_string = sizeof(AT_TYPE) - 1,
            .get = AT_ActivationType_get,
            .set = AT_ActivationType_set,
            .run = AT_return_error,
        },

        {
            .string = AT_CHANNEL,
            .size_string = sizeof(AT_CHANNEL) - 1,
            .get = AT_ChannelGroup_get,
            .set = AT_ChannelGroup_set,
            .run = AT_return_error,
        },
        {
            .string = AT_CONFIG,
            .size_string = sizeof(AT_CONFIG) - 1,
            .get = AT_Config_get,
            .set = AT_return_error,
            .run = AT_return_error,
        },
        {
            .string = AT_DEFPARAM,
            .size_string = sizeof(AT_DEFPARAM) - 1,
            .get = AT_DefParam_get,
            .set = AT_return_error,
            .run = AT_return_error,
        },
        {
            .string = AT_DCODE,
            .size_string = sizeof(AT_DCODE) - 1,
            .get = AT_DevCODE_get,
            .set = AT_DevCODE_set,
            .run = AT_return_error,
        },

        {
            .string = AT_SN,
            .size_string = sizeof(AT_SN) - 1,
            .get = AT_Sn_get,
            .set = AT_Sn_set,
            .run = AT_return_error,
        },
        {
            .string = AT_DKEY,
            .size_string = sizeof(AT_DKEY) - 1,
            .get = AT_DeviceKey_get,
            .set = AT_DeviceKey_set,
            .run = AT_return_error,
        },
        {
            .string = AT_HARDWAREVER,
            .size_string = sizeof(AT_HARDWAREVER) - 1,
            .get = AT_HARDWAREVER_get,
            .set = AT_return_error,
            .run = AT_return_error,
        },
        {
            .string = AT_RESTORE,
            .size_string = sizeof(AT_RESTORE) - 1,
            .get = AT_return_error,
            .set = AT_return_error,
            .run = AT_Parameter_Reset,
        },
        {
            .string = AT_POS_INT,
            .size_string = sizeof(AT_POS_INT) - 1,
            .get = AT_POS_INT_get,
            .set = AT_POS_INT_set,
            .run = AT_return_error,
        },
        {
            .string = AT_SAMPLE_INT,
            .size_string = sizeof(AT_SAMPLE_INT) - 1,
            .get = AT_SAMPLE_INT_get,
            .set = AT_SAMPLE_INT_set,
            .run = AT_return_error,
        },
        {
            .string = AT_SENSOR,
            .size_string = sizeof(AT_SENSOR) - 1,
            .get = AT_SensorType_get,
            .set = AT_return_error,            
            .run = AT_return_error,
        },
        {
            .string = AT_PLATFORM,
            .size_string = sizeof(AT_PLATFORM) - 1,
            .get = AT_Platform_get,
            .set = AT_Platform_set,
            .run = AT_return_error,
        },  
        {
            .string = AT_MEA,
            .size_string = sizeof(AT_MEA) - 1,
            .get = AT_MeasurementValue_get,
            .set = AT_return_error,
            .run = AT_return_error,
        },
        {
            .string = AT_POS_DEL,
            .size_string = sizeof(AT_POS_DEL) - 1,
            .get = AT_return_error,    
            .set = AT_return_error,
            .run = AT_POS_DEL_run,
        },  
        {
            .string = AT_DISCONNECT,
            .size_string = sizeof(AT_DISCONNECT) - 1,
            .get = AT_return_error,
            .set = AT_return_error,
            .run = AT_Disconnect,
        },         
};

/**
 * @brief  Parse a command and process it
 * @param  cmd The command
 * @param  cmd length
 */

void parse_cmd(const char *cmd, uint16_t length) 
{
    ATEerror_t status = AT_OK;
    const struct ATCommand_s *Current_ATCommand;
    int32_t i;

    if (strncmp(cmd, "AT", 2)) 
    {
        status = AT_ERROR;

    //   trace_debug("Invail cmd:%s\r\n", cmd);
    } 
    else if (cmd[2] == '\0') 
    {
        /* status = AT_OK; */
    } 
    else 
    {
        /*excluding AT */
        if ((cmd[length - 1] != '\n') || (cmd[length - 2] != '\r')) 
        {
            status = AT_PARAM_ERROR;
        } 
        else 
        {
            status = AT_ERROR;
            cmd += 2;
            for (i = 0; i < (sizeof(ATCommand) / sizeof(struct ATCommand_s)); i++) 
            {
                if (strncmp(cmd, ATCommand[i].string, ATCommand[i].size_string) == 0) 
                {
                    Current_ATCommand = &(ATCommand[i]);
                    /* point to the string after the command to parse it */
                    cmd += Current_ATCommand->size_string;

                    /* parse after the command */
                    switch (cmd[0]) 
                    {
                        case '\r': /* nothing after the command */
                            if (cmd[1] == '\n')
                            {
                                status = Current_ATCommand->run(cmd);
                            }
                            break;
                        case '=':
                            // if ((cmd[1] == '?') && (cmd[2] == '\0'))
                            if ((cmd[1] == '?') && (cmd[2] == '\r') && (cmd[3] == '\n')) //)
                            {
                                status = Current_ATCommand->get(cmd + 1);
                            } 
                            else 
                            {
                                status = Current_ATCommand->set(cmd + 1);
                                if (status == AT_OK) 
                                {
                                    if (!write_current_param_config()) 
                                    {
                                        status = AT_SAVE_FAILED;
                                    }
                                }
                            }
                            break;
                        case '?':
                            status = AT_OK;
                            break;
                        default:
                            /* not recognized */
                            break;
                    }
                    break;
                }
            }
        }
    }
    AT_PPRINTF("%s", status < AT_MAX ? ATError_description[status] : ATError_description[AT_MAX]);
}


