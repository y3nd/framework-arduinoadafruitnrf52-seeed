#ifndef __AT_COMMAND_H__
#define __AT_COMMAND_H__

#include <stdint.h>
#include "at.h"


/*
 * AT Command Id errors. Note that they are in sync with ATError_description static array
 * in command.c
 */
typedef enum eATEerror
{
  AT_OK = 0,
  AT_ERROR,
  AT_PARAM_ERROR,
  AT_BUSY_ERROR,
  AT_TEST_PARAM_OVERFLOW,
  AT_NO_NET_JOINED,
  AT_RX_ERROR,
  AT_NO_CLASS_B_ENABLE,
  AT_DUTYCYCLE_RESTRICTED,
  AT_CRYPTO_ERROR,
  AT_SAVE_FAILED,
  AT_READ_ERROR,
  AT_DELETE_ERROR,
  AT_MAX,
} ATEerror_t;



/**
 * @brief  Structure defining an AT Command
 */
struct ATCommand_s 
{
    const char *string;                   /*< command string, after the "AT" */
    const int32_t size_string;            /*< size of the command string, not including the final \0 */
    ATEerror_t (*get)(const char *param); /*< =? after the string to get the current value*/
    ATEerror_t (*set)(const char *param); /*< = (but not =?\0) after the string to set a value */
    ATEerror_t (*run)(const char *param); /*< \0 after the string - run the command */                     /* !NO_HELP */
};


void parse_cmd(const char *cmd,uint16_t length);


#endif /* __AT_COMMAND_H__*/

