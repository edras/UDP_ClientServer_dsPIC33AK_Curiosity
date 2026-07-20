/* ************************************************************************** */
/** UDP Heartbeat - LED sync between T1S server and client boards

  @Company
    Microchip Technology Inc.

  @File Name
    udp_heartbeat.h

  @Summary
    Server sends LED state over UDP; client mirrors it. RTT measured via ACK.

  @Description
    Protocol:
      - Server toggles LED every 250ms and sends heartbeat packet to client.
      - Client receives packet, updates its LED, sends ACK back.
      - Server measures round-trip time (RTT) from heartbeat to ACK.
      - Delay is printed on UART for diagnostics.
 */
/* ************************************************************************** */

#ifndef UDP_HEARTBEAT_H
#define UDP_HEARTBEAT_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Board role determined at boot by button state */
typedef enum {
    BOARD_ROLE_SERVER = 0,   /* Button NOT pressed at reset: server, nodeId=1 */
    BOARD_ROLE_CLIENT        /* Button pressed at reset: client, nodeId=0 */
} board_role_t;

/* UDP port for heartbeat communication */
#define HEARTBEAT_UDP_PORT      5000

/* Heartbeat payload: 1 byte LED state (server -> client) */
#define HEARTBEAT_PKT_SIZE      1

/* ACK payload: 1 byte acknowledgment (client -> server) */
#define HEARTBEAT_ACK_BYTE      0xAC
#define HEARTBEAT_ACK_SIZE      1

/* Heartbeat statistics */
typedef struct {
    uint32_t tx_count;       /* Heartbeats sent (server) or ACKs sent (client) */
    uint32_t rx_count;       /* ACKs received (server) or heartbeats received (client) */
    uint32_t last_rtt_us;    /* Last measured round-trip time in microseconds (server) */
    uint32_t avg_rtt_us;     /* Exponential moving average of RTT in microseconds (server) */
    uint32_t missed;         /* Missed heartbeats / ACKs */
} heartbeat_stats_t;

/**
 * @brief Initialize heartbeat module.
 * @param role BOARD_ROLE_SERVER or BOARD_ROLE_CLIENT
 * @param peer_ip Last octet of peer IP (server sends to client, client sends to server)
 */
void heartbeat_init(board_role_t role, uint8_t peer_ip_last_octet);

/**
 * @brief Called by server when LED is toggled. Sends heartbeat to client.
 * @param led_state Current LED state after toggle (0 or 1)
 */
void heartbeat_send(uint8_t led_state);

/**
 * @brief Service function - call from main loop. Handles RX processing.
 */
void heartbeat_service(void);

/**
 * @brief Pause/resume heartbeat LED updates.
 *        When paused, RX callback still processes packets but does NOT update LED.
 * @param paused true to pause LED updates, false to resume
 */
void heartbeat_set_paused(bool paused);

/**
 * @brief Get pointer to heartbeat statistics.
 */
const heartbeat_stats_t* heartbeat_get_stats(void);

/**
 * @brief Get the current board role.
 */
board_role_t heartbeat_get_role(void);

#ifdef __cplusplus
}
#endif

#endif /* UDP_HEARTBEAT_H */
