/*------------------------------------------------------------------------------------------------*/
/* System Tick Handler                                                                            */
/* Copyright 2021, Microchip Technology Inc. and its subsidiaries.                                */
/*                                                                                                */
/* Redistribution and use in source and binary forms, with or without                             */
/* modification, are permitted provided that the following conditions are met:                    */
/*                                                                                                */
/* 1. Redistributions of source code must retain the above copyright notice, this                 */
/*    list of conditions and the following disclaimer.                                            */
/*                                                                                                */
/* 2. Redistributions in binary form must reproduce the above copyright notice,                   */
/*    this list of conditions and the following disclaimer in the documentation                   */
/*    and/or other materials provided with the distribution.                                      */
/*                                                                                                */
/* 3. Neither the name of the copyright holder nor the names of its                               */
/*    contributors may be used to endorse or promote products derived from                        */
/*    this software without specific prior written permission.                                    */
/*                                                                                                */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"                    */
/* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE                      */
/* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE                 */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE                   */
/* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL                     */
/* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR                     */
/* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER                     */
/* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,                  */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE                  */
/* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                           */
/*------------------------------------------------------------------------------------------------*/
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#include <stdint.h>
#include <stdbool.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                         PUBLIC FUNCTIONS                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

// Free-running timer: 100MHz clock = 10ns per tick, full 32-bit counter
// Wraps at 2^32 ticks = 4,294,967,296 * 10ns = ~42.95 seconds
#define SYSTICK_TICK_NS          10UL        // Nanoseconds per tick
#define SYSTICK_TIMER_SCALER_MS  100000UL    // Divide ticks by this to get milliseconds (10ns * 100000 = 1ms)
#define SYSTICK_TIMER_SCALER_US  100UL       // Divide ticks by this to get microseconds (10ns * 100 = 1us)
#define SYSTICK_PERIOD_MS        42949UL     // Timer wraps every ~42.95 seconds

typedef struct
{ 
    uint32_t start; 
    uint32_t count; 
} SYSTICK_TIMEOUT;

/// @brief Initialize the Systick subsystem.
void SysTick_Initialize(void);

/// @brief Get the system tick count in milliseconds (monotonically increasing).
/// @return uint32_t count in milliseconds.
uint32_t SysTick_GetTickMs(void);

/// @brief Get the system tick count in microseconds (monotonically increasing).
/// @return uint64_t count in microseconds.
uint64_t SysTick_GetTickUs(void);

/// @brief Delay for the specified number of milliseconds.
/// @param milliseconds The number of milliseconds to delay.
void SysTick_DelayMs(uint32_t milliseconds);

/// @brief Delay for the specified number of microseconds.
/// @param microseconds The number of microseconds to delay.
void SysTick_DelayUs(uint32_t microseconds);

/// @brief Start a timeout period.
/// @param timeout Pointer to the SYSTICK_TIMEOUT structure.
void SysTick_StartTimeOut(SYSTICK_TIMEOUT* timeout, uint32_t delay_ms);

/// @brief Reset a timeout period.
/// @param timeout Pointer to the SYSTICK_TIMEOUT structure.
void SysTick_ResetTimeOut(SYSTICK_TIMEOUT* timeout);

/// @brief Check if the timeout period has been reached.
/// @param timeout Pointer to the SYSTICK_TIMEOUT structure.
/// @return true if the timeout period has been reached, false otherwise.
bool SysTick_IsTimeoutReached(SYSTICK_TIMEOUT* timeout);

#endif /* __SYSTICK_H__ */
