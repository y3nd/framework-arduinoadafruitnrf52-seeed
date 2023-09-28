/**
 ****************************************************************************************
 *
 * @file key_value_fds.h
 *
 * @brief Key-value flash data storage.h
 *
 ****************************************************************************************
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

#ifndef __KEY_VALUE_FDS_H__
#define __KEY_VALUE_FDS_H__

#include <stdint.h>
#include <stdbool.h>

#include <cstdio>
#include <cstdlib>

#include <Adafruit_LittleFS.h>
#include <InternalFileSystem.h>


/**
 * @defgroup KEY_VALUE_FDS_MAROC Defines
 * @{
 */
#define  KEY_VALUE_FDS_KEY_STRING_TYPE   0    /**< 1: String type, 0: Int type. */

/**
 * @defgroup KEY_VALUE_FDS_ERROR_CODE Possible error codes
 * @{
 */
#define KEY_VALUE_FDS_ERR_OK             0   /**< No error. */
#define KEY_VALUE_FDS_ERR_NO_ENTRY      -2   /**< No entry. */
#define KEY_VALUE_FDS_ERR_IO            -5   /**< Error during device operation. */
#define KEY_VALUE_FDS_ERR_NO_MEM        -12  /**< No more memory available. */
#define KEY_VALUE_FDS_ERR_EXIST         -17  /**< Entry already exists . */
#define KEY_VALUE_FDS_ERR_INVAL         -22  /**< Invalid parameter. */
#define KEY_VALUE_FDS_ERR_VALUE_BIG     -27  /**< Value length too large. */
#define KEY_VALUE_FDS_ERR_NO_SPACE      -28  /**< No space left on device. */
#define KEY_VALUE_FDS_ERR_CORRUPT       -84  /**< Corrupted. */
/** @} */

int key_value_fds_init(void);

/**
 *****************************************************************************************
 * @brief Key-value fds write.
 *
 * @param[in] p_instance: Pointer to key-value fds instance, @note Can be NULL, means that there use deafult instance.
 * @param[in] p_key/key:  Value key.
 * @param[in] p_value:    Pointer to value.
 * @param[in] length:     Length of value.
 *
 * @return Number of bytes write, or a negative error code on failure.
 *****************************************************************************************
 */
int key_value_fds_value_write(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key,
#else
                              uint32_t key,
#endif
                              const void *p_value,
                              uint32_t length);

/**
 *****************************************************************************************
 * @brief Key-value fds read.
 *
 * @param[in] p_instance: Pointer to key-value fds instance, @note Can be NULL, means that there use deafult instance.
 * @param[in] p_key/key:  Value key.
 * @param[in] p_value:    Pointer to buffer.
 * @param[in] length:     Length of buffer.
 *
 * @return Number of bytes read, or a negative error code on failure.
 *****************************************************************************************
 */
int key_value_fds_value_read(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                             char *p_key,
#else
                             uint32_t key,
#endif
                             void *p_buffer,
                             uint32_t length);

/**
 *****************************************************************************************
 * @brief Key-value fds delete.
 *
 * @param[in] p_instance: Pointer to key-value fds instance, @note Can be NULL, means that there use deafult instance.
 * @param[in] p_key/key:  Value key.
 *
 * @return Result of operation.
 *****************************************************************************************
 */
int key_value_fds_value_delete(lfs_t *p_instance,
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                               char *p_key);
#else
                               uint32_t key);
#endif

int key_value_fds_get_file_size(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key);
#else
                              uint32_t key);
#endif

int key_value_fds_value_write_pos(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                              char *p_key,
#else
                              uint32_t key,
#endif
                              const void *p_value,
                              uint32_t length,
                              int32_t pos);

int key_value_fds_value_read_pos(lfs_t *p_instance, 
#if  KEY_VALUE_FDS_KEY_STRING_TYPE
                             char *p_key,
#else
                             uint32_t key,
#endif
                             void *p_buffer,
                             uint32_t length,
                             int32_t pos);

#endif
/** @} */
/** @} */

