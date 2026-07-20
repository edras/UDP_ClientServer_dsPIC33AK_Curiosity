/* ************************************************************************** */
/** UDP Heartbeat - LED sync between T1S server and client boards

  @Company
    Microchip Technology Inc.

  @File Name
    udp_heartbeat.c

  @Summary
    Implements heartbeat LED synchronization over UDP between two T1S boards.

  @Description
    Server board toggles LED and sends 1-byte UDP packet (LED state) to client.
    Client board receives byte, mirrors LED state, sends 1-byte ACK back.
    Server measures RTT locally using microsecond timestamps.
 */
/* ************************************************************************** */

#include "udp_heartbeat.h"
#include "T1S/t1s_lwip_hal.h"
#include "T1S/tc6-lwip.h"
#include "mcc_generated_files/system/pins.h"

#include "lwip/udp.h"
#include "lwip/pbuf.h"
#include "lwip/inet.h"

#include <string.h>
#include <stdio.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                          PRIVATE DATA                                */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static board_role_t m_role;
static struct udp_pcb *m_pcb = NULL;
static ip_addr_t m_peer_ip;
static bool m_paused = false;

/* Server-side: local timestamp recorded at send (microseconds) */
static uint64_t m_send_timestamp_us = 0;

/* Statistics - non-static so X2Cscope can access */
heartbeat_stats_t hb_stats = {0};

/* RTT reporting */
#define RTT_REPORT_INTERVAL_MS  1000
static uint32_t m_last_report_time = 0;
static uint32_t m_rtt_sum_us = 0;
static uint32_t m_rtt_count = 0;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTIONS                               */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 * @brief UDP receive callback - handles incoming heartbeat or ACK packets.
 */
static void heartbeat_recv_cb(void *arg, struct udp_pcb *pcb, struct pbuf *p,
                              const ip_addr_t *addr, u16_t port)
{
    if (p == NULL) return;

    if (m_role == BOARD_ROLE_CLIENT) {
        /* Client received 1-byte heartbeat from server (LED state) */
        if (p->tot_len >= HEARTBEAT_PKT_SIZE) {
            uint8_t led_state;
            pbuf_copy_partial(p, &led_state, HEARTBEAT_PKT_SIZE, 0);

            /* Update LED to match server state (skip if paused, e.g. during reset)
             * LED is active-low: SetLow = ON, SetHigh = OFF */
            if (!m_paused) {
                if (led_state) {
                    LED0_SetLow();
                } else {
                    LED0_SetHigh();
                }
            }

            hb_stats.rx_count++;

            /* Send 1-byte ACK back to server */
            uint8_t ack_byte = HEARTBEAT_ACK_BYTE;
            struct pbuf *ack_buf = pbuf_alloc(PBUF_TRANSPORT, HEARTBEAT_ACK_SIZE, PBUF_RAM);
            if (ack_buf != NULL) {
                if (pbuf_take(ack_buf, &ack_byte, HEARTBEAT_ACK_SIZE) == ERR_OK) {
                    udp_sendto(m_pcb, ack_buf, &m_peer_ip, HEARTBEAT_UDP_PORT);
                    hb_stats.tx_count++;
                }
                pbuf_free(ack_buf);
            }
        }
    }
    else {
        /* Server received 1-byte ACK from client */
        if (p->tot_len >= HEARTBEAT_ACK_SIZE) {
            /* Calculate RTT using local microsecond timestamps */
            uint64_t now_us = T1S_GetTickCountUs();
            uint32_t rtt_us = (uint32_t)(now_us - m_send_timestamp_us);

            hb_stats.last_rtt_us = rtt_us;

            /* Exponential moving average: avg = avg*7/8 + new*1/8 */
            if (hb_stats.avg_rtt_us == 0) {
                hb_stats.avg_rtt_us = rtt_us;
            } else {
                hb_stats.avg_rtt_us = (hb_stats.avg_rtt_us * 7 + rtt_us) / 8;
            }

            hb_stats.rx_count++;
            m_rtt_sum_us += rtt_us;
            m_rtt_count++;
        }
    }

    pbuf_free(p);
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                       PUBLIC FUNCTIONS                               */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void heartbeat_init(board_role_t role, uint8_t peer_ip_last_octet)
{
    char ip_str[16];
    m_role = role;
    memset(&hb_stats, 0, sizeof(hb_stats));

    /* Set peer IP address: 192.168.0.{peer_ip_last_octet} */
    sprintf(ip_str, "192.168.0.%d", peer_ip_last_octet);
    ipaddr_aton(ip_str, &m_peer_ip);

    /* Create UDP PCB */
    m_pcb = udp_new();
    if (m_pcb == NULL) {
        printf("Heartbeat: Failed to create UDP PCB\r\n");
        return;
    }

    /* Bind to heartbeat port to receive packets */
    if (udp_bind(m_pcb, IP_ADDR_ANY, HEARTBEAT_UDP_PORT) != ERR_OK) {
        printf("Heartbeat: Failed to bind port %d\r\n", HEARTBEAT_UDP_PORT);
        udp_remove(m_pcb);
        m_pcb = NULL;
        return;
    }

    /* Register receive callback */
    udp_recv(m_pcb, heartbeat_recv_cb, NULL);

    printf("Heartbeat: initialized as %s (peer=192.168.0.%d)\r\n",
           (role == BOARD_ROLE_SERVER) ? "SERVER" : "CLIENT",
           peer_ip_last_octet);
}

void heartbeat_send(uint8_t led_state)
{
    if (m_pcb == NULL) return;
    if (m_role != BOARD_ROLE_SERVER) return;

    /* Record send time locally in microseconds for RTT calculation */
    m_send_timestamp_us = T1S_GetTickCountUs();

    /* Send 1 byte: LED state */
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, HEARTBEAT_PKT_SIZE, PBUF_RAM);
    if (p != NULL) {
        if (pbuf_take(p, &led_state, HEARTBEAT_PKT_SIZE) == ERR_OK) {
            if (udp_sendto(m_pcb, p, &m_peer_ip, HEARTBEAT_UDP_PORT) == ERR_OK) {
                hb_stats.tx_count++;
            }
        }
        pbuf_free(p);
    }
}

void heartbeat_service(void)
{
    if (m_pcb == NULL) return;

    /* Server: periodic RTT report in microseconds */
    if (m_role == BOARD_ROLE_SERVER) {
        uint32_t now = (uint32_t)T1S_GetTickCountMs();
        if ((now - m_last_report_time) >= RTT_REPORT_INTERVAL_MS) {
            m_last_report_time = now;
            if (m_rtt_count > 0) {
                uint32_t avg_us = m_rtt_sum_us / m_rtt_count;
                printf("HB: tx=%ld rx=%ld RTT=%ld us (avg=%ld us)\r\n",
                       (long)hb_stats.tx_count, (long)hb_stats.rx_count,
                       (long)hb_stats.last_rtt_us,
                       (long)avg_us);
                m_rtt_sum_us = 0;
                m_rtt_count = 0;
            }
        }
    }
}

const heartbeat_stats_t* heartbeat_get_stats(void)
{
    return &hb_stats;
}

board_role_t heartbeat_get_role(void)
{
    return m_role;
}

void heartbeat_set_paused(bool paused)
{
    m_paused = paused;
}

/* *****************************************************************************
 End of File
 */
