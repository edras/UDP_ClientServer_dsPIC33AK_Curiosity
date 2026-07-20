#ifndef APP_H
#define APP_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    APP_BTN_NONE = 0,
    APP_BTN_SW1,
} APP_BTN_ID;

void APP_Buttons_Init(void);
void APP_Buttons_Task(void);
bool APP_Button_IsPressed(APP_BTN_ID btn_id);
bool APP_Button_WasPressed(APP_BTN_ID btn_id);

/**
 * @brief Check if button is held at boot (for role selection).
 *        Must be called BEFORE APP_Buttons_Init() or very early in startup.
 *        Reads the raw pin state without debounce.
 * @return true if button is pressed (active low, pin=0)
 */
bool APP_Button_IsHeldAtBoot(void);

/**
 * @brief Check if button has been held for a long press.
 *        Call from main loop after APP_Buttons_Task().
 * @param hold_ms Duration in milliseconds to consider a long press
 * @return true if button has been held continuously for >= hold_ms
 */
bool APP_Button_LongPress(uint32_t hold_ms);

#endif // APP_H
