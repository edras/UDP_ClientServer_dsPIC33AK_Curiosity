#ifndef _SYSTICK_HAl
#define _SYSTICK_HAl

// Support a delay method based on system peripheral clock.
#ifndef FCY
    #include "mcc_generated_files/system/clock.h"
    #define FCY CLOCK_FastPeripheralFrequencyGet()
#endif
#include <libpic30.h>
#include <stddef.h>

#include "mcc_generated_files/system/interrupt.h"

#define ENTER_CRITICAL()   (INTERRUPT_GlobalDisable())
#define LEAVE_CRITICAL()   (INTERRUPT_GlobalEnable())

/// @brief Initialize the systick hardware timer.
/// @param CallbackHandler Optional callback (can be NULL for free-running mode).
void SysTick_hal_Initialize(void (*CallbackHandler)(void));

/// @brief Read the raw hardware timer counter value.
/// @return Raw tick count (20ns per tick, wraps at period boundary).
uint32_t SysTick_hal_GetTick(void);

#endif
