/*!
 * @file      smtc_hal_mcu.h
 *
 * @brief     Board specific package MCU API definition.
 *
 * The Clear BSD License
 * Copyright Semtech Corporation 2021. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Semtech corporation nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SEMTECH CORPORATION BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef SMTC_HAL_MCU_H
#define SMTC_HAL_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * -----------------------------------------------------------------------------
 * --- DEPENDENCIES ------------------------------------------------------------
 */

#include <stdint.h>
#include <stdbool.h>

// #include "smtc_hal.h"

#include "lbm/smtc_modem_hal/smtc_modem_hal.h"


/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC MACROS -----------------------------------------------------------
 */

// #define TRACE_PRINTF( ... ) hal_trace_print_var( __VA_ARGS__ )

#define TRACE_PRINTF( ... ) smtc_modem_hal_print_trace( __VA_ARGS__ )

/*!
 * Panic function for mcu issues
 */
#define mcu_panic( ... )                            \
    do                                              \
    {                                               \
        TRACE_PRINTF( "mcu_panic:%s\n", __func__ ); \
        TRACE_PRINTF( "-> "__VA_ARGS__ );           \
        hal_mcu_reset( );                           \
    } while( 0 );

/*!
 * @brief Begins critical section \note this call adds a mask variable in the context
 */
#define CRITICAL_SECTION_BEGIN( ) \
    uint32_t mask;                \
    hal_mcu_critical_section_begin( &mask )

/*!
 * @brief Ends critical section \note this shall be called in the same context as previous CRITICAL_SECTION_BEGIN( )
 */
#define CRITICAL_SECTION_END( ) hal_mcu_critical_section_end( &mask )

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC CONSTANTS --------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC TYPES ------------------------------------------------------------
 */

/*
 * -----------------------------------------------------------------------------
 * --- PUBLIC FUNCTIONS PROTOTYPES ---------------------------------------------
 */

/*!
 * @brief Disable interrupts, begins critical section
 *
 * @param [in] mask Pointer to a variable where to store the CPU IRQ mask
 */
void hal_mcu_critical_section_begin( uint32_t* mask );

/*!
 * @brief Ends critical section
 *
 * @param [in] mask Pointer to a variable where the CPU IRQ mask was stored
 */
void hal_mcu_critical_section_end( uint32_t* mask );


/*!
 * @brief Resets the MCU
 */
void hal_mcu_reset( void );

/*!
 * @brief Waits for delay microseconds
 *
 * @param [in] microseconds Delay to wait in microseconds
 */
void hal_mcu_wait_us( const int32_t microseconds );

/*!
 * @brief Waits for delay milliseconds
 *
 * @param [in] ms Delay to wait in milliseconds
 */
void hal_mcu_wait_ms( const int32_t ms );


/*!
 * @brief Prints debug trace
 *
 * @param variadics arguments
 */
void hal_mcu_trace_print( const char* fmt, ... );


#ifdef __cplusplus
}
#endif

#endif  // SMTC_HAL_MCU_H

/* --- EOF ------------------------------------------------------------------ */
