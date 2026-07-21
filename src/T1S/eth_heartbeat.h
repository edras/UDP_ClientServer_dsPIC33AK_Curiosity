/* ************************************************************************** */
/** Raw Ethernet Heartbeat - LED sync bypassing UDP/IP for minimum RTT

  @Company
    Microchip Technology Inc.

  @File Name
    eth_heartbeat.h

  @Summary
    Server sends LED state as raw Ethernet frame; client mirrors it and ACKs.
    Bypasses the entire lwIP UDP/IP stack for minimum round-trip latency.

  @Description
    Protocol:
      - Uses custom EtherType 0x88B5 (IEEE local experimental)
      - Server sends 1-byte payload (LED state) directly via TC6
      - Client receives frame, updates LED, sends 1-byte ACK back
      - Server measures RTT from send to ACK reception
      - Eliminates IP (20B) + UDP (8B) headers and all stack processing
 */
/* ************************************************************************** */

#ifndef ETH_HEARTBEAT_H
#define ETH_HEARTBEAT_H

#include <stdint.h>
#include <stdbool.h>
#include "udp_heartbeat.h"  /* For board_role_t */

#ifdef __cplusplus
extern "C" {
#endif

/* Custom EtherType for raw heartbeat (IEEE 802 local experimental) */
#define ETH_HB_ETHERTYPE        0x88B5

/* Payload sizes (same as UDP heartbeat) */
#define ETH_HB_PKT_SIZE         1       /* LED state byte */
#define ETH_HB_ACK_BYTE         0xAC    /* ACK marker */
#define ETH_HB_ACK_SIZE         1

/* Complete frame sizes (Ethernet header + payload, no FCS - HW adds it) */
#define ETH_HB_HDR_SIZE         14      /* 6 dst + 6 src + 2 EtherType */
#define ETH_HB_FRAME_SIZE       (ETH_HB_HDR_SIZE + ETH_HB_PKT_SIZE)  /* 15 bytes */

/* Raw Ethernet heartbeat statistics */
typedef struct {
    uint32_t tx_count;
    uint32_t rx_count;
    uint32_t last_rtt_us;
    uint32_t avg_rtt_us;
} eth_heartbeat_stats_t;

/**
 * @brief Initialize raw Ethernet heartbeat module.
 * @param role BOARD_ROLE_SERVER or BOARD_ROLE_CLIENT
 * @note Must be called after T1S_init() so the netif and MAC are available.
 */
void eth_heartbeat_init(board_role_t role);

/**
 * @brief Called by server to send raw Ethernet heartbeat frame.
 * @param led_state Current LED state (0 or 1)
 */
void eth_heartbeat_send(uint8_t led_state);

/**
 * @brief Service function - call from main loop. Handles RTT reporting.
 */
void eth_heartbeat_service(void);

/**
 * @brief Raw frame RX handler - called from tc6-lwip.c when our EtherType is detected.
 * @param pFrame Pointer to complete Ethernet frame (starting at dst MAC)
 * @param len Total frame length
 */
void eth_heartbeat_rx_handler(const uint8_t *pFrame, uint16_t len);

/**
 * @brief Pause/resume raw heartbeat LED updates.
 * @param paused true to pause LED updates, false to resume
 */
void eth_heartbeat_set_paused(bool paused);

/**
 * @brief Get pointer to raw Ethernet heartbeat statistics.
 */
const eth_heartbeat_stats_t* eth_heartbeat_get_stats(void);

/**
 * @brief Reset RTT reporting accumulators.
 *        Call when switching modes to avoid stale transitional RTT reports.
 */
void eth_heartbeat_reset_rtt(void);

#ifdef __cplusplus
}
#endif

#endif /* ETH_HEARTBEAT_H */
