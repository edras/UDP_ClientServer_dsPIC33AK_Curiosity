#include "../../systick/systick.h"
#include "systick_hal.h"

#include "mcc_generated_files/timer/tmr1.h"

void SysTick_hal_Initialize(void (*CallbackHandler)(void))
{
    /* Register callback only if provided (not needed for free-running mode) */
    if (CallbackHandler != NULL) {
        Timer1.TimeoutCallbackRegister(CallbackHandler);
    }
    Timer1.Start();
}

uint32_t SysTick_hal_GetTick(void)
{
    return Timer1.CounterGet();
}

void SysTick_DelayMs(uint32_t milliseconds)
{
    __delay_ms(milliseconds);
}

void SysTick_DelayUs(uint32_t microseconds)
{
    __delay_us(microseconds);
}
