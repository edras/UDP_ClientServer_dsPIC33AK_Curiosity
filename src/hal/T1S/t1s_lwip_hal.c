#include "t1s_lwip_hal.h"

void T1S_EIC_CB_Register(void (* callback)(void))
{
    INT1_SetInterruptHandler(callback); 
}

