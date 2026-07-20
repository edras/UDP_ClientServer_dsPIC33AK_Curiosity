#include "app.h"
#include "mcc_generated_files/system/pins.h"
#include "systick/systick.h"

// Button state tracking
typedef struct {
    bool last_state;
    bool debounced_state;
    uint32_t last_debounce_time;
    bool was_pressed;
    uint32_t press_start_time;
    bool long_press_detected;
} app_btn_t;

// Debounce interval in milliseconds
#define APP_BUTTON_DEBOUNCE_MS  20

static app_btn_t sw1_btn = {0};

static bool read_raw_sw1(void) { return (SW0_GetValue() == 0); } // active low

bool APP_Button_IsHeldAtBoot(void)
{
    /* Read raw pin - no debounce needed at boot since user holds button from power-on */
    return (SW0_GetValue() == 0);
}

void APP_Buttons_Init(void)
{
    sw1_btn = (app_btn_t){0};
}

static void debounce_button(app_btn_t* btn, bool raw_state)
{
    uint32_t now = SysTick_GetTickMs();
    if (raw_state != btn->last_state) {
        btn->last_debounce_time = now;
    }
    if ((now - btn->last_debounce_time) > APP_BUTTON_DEBOUNCE_MS) {
        // Detect rising edge (press start) - record time, don't fire yet
        if (btn->debounced_state == false && raw_state == true) {
            btn->press_start_time = now;
            btn->long_press_detected = false;
        }
        // Detect falling edge (release) - fire short press only if not long press
        if (btn->debounced_state == true && raw_state == false) {
            if (!btn->long_press_detected) {
                btn->was_pressed = true;  // short press confirmed on release
            }
        }
        btn->debounced_state = raw_state;
    }
    btn->last_state = raw_state;
}

// Call this periodically (e.g. from main loop)
void APP_Buttons_Task(void)
{
    debounce_button(&sw1_btn, read_raw_sw1());
}

bool APP_Button_IsPressed(APP_BTN_ID btn_id)
{
    if (btn_id == APP_BTN_SW1) {
        return sw1_btn.debounced_state;
    } 
    return false;
}

bool APP_Button_WasPressed(APP_BTN_ID btn_id)
{
    if (btn_id == APP_BTN_SW1 && sw1_btn.was_pressed) {
        sw1_btn.was_pressed = false;
        return true;
    } 
    return false;
}

bool APP_Button_LongPress(uint32_t hold_ms)
{
    if (sw1_btn.debounced_state && !sw1_btn.long_press_detected) {
        uint32_t now = SysTick_GetTickMs();
        if ((now - sw1_btn.press_start_time) >= hold_ms) {
            sw1_btn.long_press_detected = true;
            return true;
        }
    }
    return false;
}


