/* ************************************************************************** */
/** Raw Ethernet Heartbeat - LED sync bypassing UDP/IP for minimum RTT

  @Company
    Microchip Technology Inc.

  @File Name
    eth_heartbeat.c

  @Summary
    Implements heartbeat LED synchronization using raw Ethernet frames,
    completely bypassing the lwIP UDP/IP stack for minimum latency.

  @Description
    Server sends a raw Ethernet frame (EtherType 0x88B5) with 1-byte payload
    (LED state) directly via TC6_SendRawEthernetPacket(). Client receives the
    frame in the TC6 RX callback, updates the LED, and sends a 1-byte ACK
    back as another raw Ethernet frame. Server measures RTT.

    Compared to UDP heartbeat (~507 us), this eliminates:
      - IP header construction/parsing (20 bytes)
      - UDP header construction/parsing (8 bytes)
      - lwIP pbuf allocation for transport layer
      - UDP checksum computation
      - IP routing/ARP lookup
 */
/* ************************************************************************** */

#include "eth_heartbeat.h"
#include "tc6-lwip.h"
#include "T1S/t1s_lwip_hal.h"
#include "mcc_generated_files/system/pins.h"

#include <string.h>
#include <stdio.h>

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                          PRIVATE DATA                                */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

static board_role_t m_role;
static bool m_initialized = false;
static bool m_paused = false;

/* Our MAC address and peer MAC (broadcast for simplicity on 2-node bus) */
static uint8_t m_our_mac[6];
static const uint8_t m_broadcast_mac[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

/* Pre-built TX frame buffer (avoids allocation on every send) */
static uint8_t m_tx_frame[ETH_HB_FRAME_SIZE];

/* Server-side: timestamp at send for RTT measurement */
static uint64_t m_send_timestamp_us = 0;

/* Statistics */
eth_heartbeat_stats_t eth_hb_stats = {0};

/* RTT reporting - prints each exchange individually */

/* TC6 instance index (from TC6LwIP_Init) */
static int8_t m_tc6_idx = -1;

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                      PRIVATE FUNCTIONS                               */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

/**
 * @brief Build and send a raw Ethernet frame with 1-byte payload.
 * @param payload Single byte to send (LED state or ACK)
 */
static void eth_hb_send_frame(uint8_t payload)
{
    struct netif *netif = TC6LwIP_GetNetIf(m_tc6_idx);
    if (netif == NULL) return;

    /* Build Ethernet frame directly in pre-allocated buffer:
     * [0-5]   Destination MAC (broadcast)
     * [6-11]  Source MAC (our MAC)
     * [12-13] EtherType (0x88B5, big-endian)
     * [14]    Payload (1 byte)
     */
    memcpy(&m_tx_frame[0], m_broadcast_mac, 6);
    memcpy(&m_tx_frame[6], m_our_mac, 6);
    m_tx_frame[12] = (uint8_t)(ETH_HB_ETHERTYPE >> 8);
    m_tx_frame[13] = (uint8_t)(ETH_HB_ETHERTYPE & 0xFF);
    m_tx_frame[14] = payload;

    /* Send raw frame via TC6 - bypasses lwIP entirely */
    TC6_t *tc6 = TC6LwIP_GetTC6(m_tc6_idx);
    if (tc6 != NULL) {
        TC6_SendRawEthernetPacket(tc6, m_tx_frame, ETH_HB_FRAME_SIZE, 0, NULL, NULL);
    }
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
/*                       PUBLIC FUNCTIONS                               */
/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/

void eth_heartbeat_init(board_role_t role)
{
    m_role = role;
    memset(&eth_hb_stats, 0, sizeof(eth_hb_stats));

    /* Get our netif index (always 0 for single-instance) */
    m_tc6_idx = 0;

    /* Get our MAC address */
    uint8_t *mac_ptr = NULL;
    TC6LwIP_GetMac(m_tc6_idx, &mac_ptr);
    if (mac_ptr != NULL) {
        memcpy(m_our_mac, mac_ptr, 6);
    }

    m_initialized = true;

    printf("ETH: initialized as %s (raw EtherType=0x%04X)\r\n",
           (role == BOARD_ROLE_SERVER) ? "SERVER" : "CLIENT",
           ETH_HB_ETHERTYPE);
}

void eth_heartbeat_send(uint8_t led_state)
{
    if (!m_initialized) return;
    if (m_role != BOARD_ROLE_SERVER) return;

    /* Record send time for RTT calculation */
    m_send_timestamp_us = T1S_GetTickCountUs();

    /* Send LED state as raw Ethernet frame */
    eth_hb_send_frame(led_state);
    eth_hb_stats.tx_count++;
}

void eth_heartbeat_rx_handler(const uint8_t *pFrame, uint16_t len)
{
    if (!m_initialized) return;
    if (len < ETH_HB_FRAME_SIZE) return;

    /* Extract payload byte (offset 14 in Ethernet frame) */
    uint8_t payload = pFrame[ETH_HB_HDR_SIZE];

    if (m_role == BOARD_ROLE_CLIENT) {
        /* Client received heartbeat from server - payload is LED state */
        if (!m_paused) {
            if (payload) {
                LED0_SetLow();   /* ON (active-low) */
            } else {
                LED0_SetHigh();  /* OFF */
            }
        }

        eth_hb_stats.rx_count++;

        /* Send ACK back as raw Ethernet frame */
        eth_hb_send_frame(ETH_HB_ACK_BYTE);
        eth_hb_stats.tx_count++;
    }
    else {
        /* Server received ACK from client */
        /* Discard stale ACKs (timestamp cleared on mode switch) */
        if (m_send_timestamp_us == 0) return;

        uint64_t now_us = T1S_GetTickCountUs();
        uint32_t rtt_us = (uint32_t)(now_us - m_send_timestamp_us);
        m_send_timestamp_us = 0;  /* Consume - one ACK per send */

        eth_hb_stats.last_rtt_us = rtt_us;

        /* Exponential moving average: avg = avg*7/8 + new*1/8 */
        if (eth_hb_stats.avg_rtt_us == 0) {
            eth_hb_stats.avg_rtt_us = rtt_us;
        } else {
            eth_hb_stats.avg_rtt_us = (eth_hb_stats.avg_rtt_us * 7 + rtt_us) / 8;
        }

        eth_hb_stats.rx_count++;

        /* Report immediately for this exchange */
        printf("ETH: tx=%ld rx=%ld RTT=%ld us (avg=%ld us)\r\n",
               (long)eth_hb_stats.tx_count, (long)eth_hb_stats.rx_count,
               (long)rtt_us, (long)eth_hb_stats.avg_rtt_us);
    }
}

void eth_heartbeat_service(void)
{
    /* Reporting is now done per-exchange in the rx handler.
     * This function is kept for future periodic maintenance if needed. */
    (void)0;
}

void eth_heartbeat_set_paused(bool paused)
{
    m_paused = paused;
}

const eth_heartbeat_stats_t* eth_heartbeat_get_stats(void)
{
    return &eth_hb_stats;
}

void eth_heartbeat_reset_rtt(void)
{
    m_send_timestamp_us = 0;  /* Invalidate pending ACK */
}

/* *****************************************************************************
 End of File
 */
