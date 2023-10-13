/**
 *****************************************************************************************
 *
 * @file key_value_fds.c
 *
 * @brief Key Value Flash Data Storage Implementation.
 *
 *****************************************************************************************
 * @attention
  #####Copyright (c) 2019 GOODIX
  All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the name of GOODIX nor the names of its contributors may be used
    to endorse or promote products derived from this software without
    specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 *****************************************************************************************
 */

/*
 * INCLUDE FILES
 *****************************************************************************************
 */
#include "key_value_fds.h"

#include "Lbm_packet.hpp"

using namespace Adafruit_LittleFS_Namespace;



#define KEY_VALUE_FDS_VERIFY(ret)     \
do                                    \
{                                     \
    if (ret)                          \
    {                                 \
        return ret;                   \
    }                                 \
} while(0)


#define KEY_VALUE_FDS_CACHE_SIZE        256

static __attribute__ ((aligned (4))) uint8_t s_file_buffer[KEY_VALUE_FDS_CACHE_SIZE];



static lfs_t     s_default_instance;
static lfs_t    *s_op_instance_ptr;

#if !KEY_VALUE_FDS_KEY_STRING_TYPE
static char     s_key_char[10] = {0};
#endif

#if !KEY_VALUE_FDS_KEY_STRING_TYPE
uint32_t key_value_fds_char_key_convert(const char *str)
{
    uint32_t num = 0;
    const char *p = str;

    while (*p >= '0' && *p <= '9')
    {
        num = num * 10 + (*p - '0');
        p++;
    }

    return num;
}

static char * key_value_fds_int_key_convert(uint32_t key)
{
    uint16_t digit = 0;
    uint32_t temp = key;

    do
    {
        temp /= 10;
        digit++;
    } while (temp > 0);

    s_key_char[digit] = '\0';

    do {
        digit--;
        s_key_char[digit] = key % 10 + '0';
        key /= 10;
    } while (key > 0);

    return s_key_char;
}
#endif


int key_value_fds_init(void)
{
    if(ExternalFS.begin())
    {
        s_op_instance_ptr = ExternalFS._getFS();      
        s_default_instance = *s_op_instance_ptr;
        return KEY_VALUE_FDS_ERR_OK;
    }
    return KEY_VALUE_FDS_ERR_IO;
}

int key_value_fds_value_write(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key,
#else
                              uint32_t key,
#endif
                              const void *p_value,
                              uint32_t length)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;

#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, p_key, LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#else
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#endif

    ret = lfs_file_rewind(s_op_instance_ptr, &file_id);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_write(s_op_instance_ptr, &file_id, p_value, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}

int key_value_fds_value_read(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                             char *p_key,
#else
                             uint32_t key,
#endif
                             void *p_buffer,
                             uint32_t length)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, p_key, LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#else
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#endif
    ret = lfs_file_rewind(s_op_instance_ptr, &file_id);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_read(s_op_instance_ptr, &file_id, p_buffer, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}


int key_value_fds_value_delete(lfs_t *p_instance,
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                               char *p_key)
#else
                               uint32_t key)
#endif
{
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    return lfs_remove(s_op_instance_ptr, p_key);
#else
     return lfs_remove(s_op_instance_ptr, key_value_fds_int_key_convert(key));
#endif
}




int key_value_fds_get_file_size(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key)
#else
                              uint32_t key)
#endif
{
    int ret;
    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};
    file_config.buffer = s_file_buffer;
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, p_key, LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#else
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#endif
    ret = lfs_file_size(s_op_instance_ptr, &file_id);
    lfs_file_close(s_op_instance_ptr, &file_id);

    return ret;
}

int key_value_fds_value_write_pos(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key,
#else
                              uint32_t key,
#endif
                              const void *p_value,
                              uint32_t length,
                              int32_t pos)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, p_key, LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#else
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#endif
    
    ret = lfs_file_seek(s_op_instance_ptr, &file_id, pos, LFS_SEEK_SET);
    if (ret < 0)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_write(s_op_instance_ptr, &file_id, p_value, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}

int key_value_fds_value_read_pos(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                             char *p_key,
#else
                             uint32_t key,
#endif
                             void *p_buffer,
                             uint32_t length,
                             int32_t pos)
{
    int ret;

    lfs_file_t              file_id;
    struct lfs_file_config  file_config = {0};

    file_config.buffer = s_file_buffer;
 
    // s_op_instance_ptr = NULL == p_instance ? &s_default_instance : p_instance;
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, p_key, LFS_O_RDWR | LFS_O_CREAT, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#else
    ret = lfs_file_opencfg(s_op_instance_ptr, &file_id, key_value_fds_int_key_convert(key), LFS_O_RDONLY, &file_config);
    KEY_VALUE_FDS_VERIFY(ret);
#endif
    
    ret = lfs_file_seek(s_op_instance_ptr, &file_id, pos, LFS_SEEK_SET);
    if (ret < 0)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_read(s_op_instance_ptr, &file_id, p_buffer, length);
    if (ret)
    {
        lfs_file_close(s_op_instance_ptr, &file_id);
        return ret;
    }

    ret = lfs_file_close(s_op_instance_ptr, &file_id);
    KEY_VALUE_FDS_VERIFY(ret);

    return KEY_VALUE_FDS_ERR_OK;
}