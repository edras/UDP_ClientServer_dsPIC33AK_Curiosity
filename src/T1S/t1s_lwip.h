/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef _T1S_LWIP_H    /* Guard against multiple inclusion */
#define _T1S_LWIP_H

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Configure T1S network parameters before calling T1S_init().
 * @param nodeId PLCA node ID (0=coordinator/client, 1=server)
 * @param ip_last_octet Last octet of IPv4 address (192.168.0.{octet})
 */
void  T1S_configure(uint8_t nodeId, uint8_t ip_last_octet);

bool  T1S_init(void);
bool  T1S_available(void);
void  T1S_execute(void);
void  T1S_set_bridge(bool state);
void  T1S_set_debug(bool state);
void  T1S_print_menu(void);
void  T1S_send_cmd(char* buffer);

#endif /* _T1S_LWIP_H */

/* *****************************************************************************
 End of File
 */
