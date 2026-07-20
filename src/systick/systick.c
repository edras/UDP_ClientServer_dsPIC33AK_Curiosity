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

#include "systick.h"
#include "systick/systick_hal.h"

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      DEFINES AND LOCAL VARIABLES                     */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static uint32_t last_raw = 0;
static uint32_t ms_offset = 0;     /* Accumulated ms from past timer periods */

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTIONS                               */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 * @brief Read raw timer tick and detect wrap-around.
 *        Must be called at least once per 60s period to detect wraps.
 * @return Current raw tick value (0 to ~3,000,000,000)
 */
static uint32_t read_tick_with_wrap_detect(void)
{
    uint32_t raw;

    ENTER_CRITICAL();
    raw = SysTick_hal_GetTick();
    if (raw < last_raw) {
        /* Timer wrapped - accumulate one full period */
        ms_offset += SYSTICK_PERIOD_MS;
    }
    last_raw = raw;
    LEAVE_CRITICAL();

    return raw;
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                         PUBLIC FUNCTIONS                             */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void SysTick_Initialize(void)
{
    last_raw = 0;
    ms_offset = 0;
    SysTick_hal_Initialize(NULL);
}

uint32_t SysTick_GetTickMs(void)
{
    uint32_t raw = read_tick_with_wrap_detect();
    return ms_offset + raw / SYSTICK_TIMER_SCALER_MS;
}

uint64_t SysTick_GetTickUs(void)
{
    uint32_t raw = read_tick_with_wrap_detect();
    return (uint64_t)ms_offset * 1000UL + (uint64_t)(raw / SYSTICK_TIMER_SCALER_US);
}

void SysTick_StartTimeOut(SYSTICK_TIMEOUT* timeout, uint32_t delay_ms)
{
    timeout->start = SysTick_GetTickMs();
    timeout->count = delay_ms;
}

void SysTick_ResetTimeOut(SYSTICK_TIMEOUT* timeout)
{
    timeout->start = SysTick_GetTickMs();
}

bool SysTick_IsTimeoutReached(SYSTICK_TIMEOUT* timeout)
{
    return ((SysTick_GetTickMs() - timeout->start) >= timeout->count);
}
