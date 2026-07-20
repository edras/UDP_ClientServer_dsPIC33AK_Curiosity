![Microchip Logo](assets/logo.png "Microchip Technology")

# dsPIC33AK512MPS506 Curiosity - 10BASE-T1S Heartbeat Demo

**Two-board LED synchronization and throughput testing over 10BASE-T1S Single Pair Ethernet**

![Version](https://img.shields.io/badge/version-1.0.0-green)
![Status](https://img.shields.io/badge/status-active-success)
![Updated](https://img.shields.io/badge/updated-2026--07--20-informational)

---

## Table of Contents

- [About](#about)
- [Features](#features)
- [Hardware](#hardware)
- [Network Configuration](#network-configuration)
- [Protocol Design](#protocol-design)
- [Timing Diagram](#timing-diagram)
- [Packet Size Analysis](#packet-size-analysis)
- [Performance Results](#performance-results)
- [Usage](#usage)
- [Build](#build)
- [Project Structure](#project-structure)
- [License](#license)

---

## About

This project demonstrates real-time LED synchronization between two dsPIC33AK512MPS506 Curiosity boards connected via 10BASE-T1S (Single Pair Ethernet). A single firmware image runs on both boards - the role (server or client) is selected by holding the button during reset.

The server board blinks its LED at 500ms intervals and sends a 1-byte UDP heartbeat to the client. The client mirrors the LED state and sends a 1-byte ACK back. The server measures Round-Trip Time (RTT) with 10ns resolution, demonstrating sub-millisecond latency on the T1S link.

An iperf-style UDP throughput test is also included for bandwidth measurement between the two boards.

---

## Features

- **Single firmware** for both boards - role selected at boot via button
- **1-byte UDP heartbeat** with sub-millisecond RTT measurement (~507 us)
- **LED synchronization** over 10BASE-T1S using PLCA
- **UDP throughput test** (iperf) achieving ~9.5 Mbps board-to-board
- **Free-running 32-bit timer** with 10ns resolution (no ISR overhead)
- **Long-press reset** with visual feedback for role switching

---

## Hardware

| Component | Description |
|-----------|-------------|
| MCU | dsPIC33AK512MPS506 @ 200 MHz |
| Board | Curiosity Development Board |
| MAC-PHY | LAN8651 (10BASE-T1S with PLCA) |
| Interface | SPI (MCU to MAC-PHY) |
| LED | LED0 (active-low) |
| Switch | SW0 (active-low, with debounce) |

---

## Network Configuration

| Parameter | Server | Client |
|-----------|--------|--------|
| PLCA Node ID | 1 | 0 (coordinator) |
| IP Address | 192.168.0.101 | 192.168.0.100 |
| PLCA Node Count | 2 | 2 |
| Role Selection | Button NOT pressed at reset | Button held at reset |

---

## Protocol Design

### Heartbeat (UDP Port 5000)

```
SERVER                              CLIENT
  |                                    |
  |--- [1 byte: LED state] ---------->|  (Heartbeat)
  |                                    |  Client updates LED
  |<-- [1 byte: 0xAC] ----------------|  (ACK)
  |                                    |
  | RTT = time(ACK rx) - time(HB tx)  |
  |                                    |
```

### iperf Throughput Test (UDP Port 5001)

```
SERVER                              CLIENT
  |                                    |
  |--- [1460 bytes] ----------------->|
  |--- [1460 bytes] ----------------->|  Continuous UDP flood
  |--- [1460 bytes] ----------------->|
  |          ...                       |
  |                                    |  Client reports RX bandwidth
```

---

## Timing Diagram

```
         SERVER                    T1S Wire                    CLIENT
    ┌──────────────┐          ┌──────────────┐          ┌──────────────┐
    │              │          │              │          │              │
    │ Toggle LED   │          │              │          │              │
    │ Record t0    │          │              │          │              │
    │              │          │              │          │              │
    │ SPI TX ──────┼──~80us──>│              │          │              │
    │              │          │ PLCA wait ───┼──~30us──>│              │
    │              │          │ Wire TX ─────┼───5us───>│              │
    │              │          │              │          │ SPI RX       │
    │              │          │              │   ~80us  │ lwIP process │
    │              │          │              │          │ Update LED   │
    │              │          │              │          │ Send ACK     │
    │              │          │              │          │ SPI TX ──────┼──~80us──┐
    │              │          │              │<─~30us───┼─ PLCA wait   │         │
    │              │          │              │<──5us────┼─ Wire TX     │         │
    │              │   ~80us  │              │          │              │         │
    │ SPI RX <─────┼──────────┤              │          │              │         │
    │ Record t1    │          │              │          │              │         │
    │              │          │              │          │              │         │
    │ RTT = t1-t0  │          │              │          │              │         │
    │   ~507 us    │          │              │          │              │         │
    └──────────────┘          └──────────────┘          └──────────────┘

    |<─────────────────── ~507 us RTT ────────────────────>|
```

---

## Packet Size Analysis

### Heartbeat Packet (Server → Client)

| Layer | Field | Bytes |
|-------|-------|-------|
| Ethernet | Preamble + SFD | 8 |
| Ethernet | Destination MAC | 6 |
| Ethernet | Source MAC | 6 |
| Ethernet | EtherType | 2 |
| IP | Header | 20 |
| UDP | Header | 8 |
| **Application** | **LED state (payload)** | **1** |
| Ethernet | FCS | 4 |
| **Total on wire** | | **55 bytes** |

### ACK Packet (Client → Server)

| Layer | Field | Bytes |
|-------|-------|-------|
| Ethernet | Preamble + SFD | 8 |
| Ethernet | Destination MAC | 6 |
| Ethernet | Source MAC | 6 |
| Ethernet | EtherType | 2 |
| IP | Header | 20 |
| UDP | Header | 8 |
| **Application** | **ACK byte (0xAC)** | **1** |
| Ethernet | FCS | 4 |
| **Total on wire** | | **55 bytes** |

### iperf Packet (Throughput Test)

| Layer | Field | Bytes |
|-------|-------|-------|
| Ethernet | Preamble + SFD | 8 |
| Ethernet | Destination MAC | 6 |
| Ethernet | Source MAC | 6 |
| Ethernet | EtherType | 2 |
| IP | Header | 20 |
| UDP | Header | 8 |
| **Application** | **iperf data** | **1460** |
| Ethernet | FCS | 4 |
| **Total on wire** | | **1514 bytes** |

### Summary

| Packet Type | Payload | Total on Wire | Direction |
|-------------|---------|---------------|-----------|
| Heartbeat | 1 byte | 55 bytes | Server → Client |
| ACK | 1 byte | 55 bytes | Client → Server |
| iperf | 1460 bytes | 1514 bytes | Server → Client |

---

## Performance Results

| Metric | Value |
|--------|-------|
| Heartbeat RTT | ~507-513 us |
| RTT Jitter | ±3 us |
| Heartbeat Rate | 2 Hz (500ms period) |
| iperf Throughput | ~9.5 Mbps (95% link utilization) |
| PLCA Nodes | 2 |
| Timer Resolution | 10 ns |

---

## Usage

### Role Selection at Boot

1. **SERVER**: Power on the board normally (no button press)
2. **CLIENT**: Hold SW0 while powering on / resetting the board

### Normal Operation

| Action | Server | Client |
|--------|--------|--------|
| LED behavior | Blinks at 500ms | Mirrors server LED |
| Short press SW0 | Start/stop iperf | No action |
| Long press SW0 (3s) | LED solid ON → release → reset in 1s | Same |
| UART output | RTT stats every 1s | Throughput stats when receiving iperf |

### Reset & Role Change

1. Hold SW0 for 3 seconds → LED turns ON solid
2. Release → "Resetting in 1s..." message
3. During that 1 second: hold SW0 for CLIENT, or leave released for SERVER
4. Board resets and boots into selected role

### UART Output Examples

**Server (heartbeat):**
```
=== dsPIC33AK512MPS506 Curiosity + T1S ===
Role: SERVER (nodeId=1, IP=192.168.0.101)
  LED blinks at 500ms, heartbeat sent to client
  Short press: toggle iperf | Long press (3s): reset
Heartbeat: initialized as SERVER (peer=192.168.0.100)
HB: tx=10 rx=10 RTT=507 us (avg=507 us)
HB: tx=12 rx=12 RTT=512 us (avg=510 us)
```

**Client (receiving iperf):**
```
=== dsPIC33AK512MPS506 Curiosity + T1S ===
Role: CLIENT (nodeId=0, IP=192.168.0.100)
  LED mirrors server heartbeat
  Short press: toggle iperf | Long press (3s): reset
Heartbeat: initialized as CLIENT (peer=192.168.0.101)
IPERF RX bandwidth=9470 kbit/s packets=810 1/s
```

---

## Build

### Prerequisites

| Tool | Version |
|------|---------|
| MPLAB X IDE | v6.30+ |
| XC-DSC Compiler | v4.00+ |
| dsPIC33AK-MP DFP | 1.4.260+ |

### Build Steps

1. Open the project in VS Code with MPLAB extension
2. Select configuration: `dspic33AK_curiosity_T1S / default`
3. Build (F7 or MPLAB extension build button)
4. Program both boards with the same `out/dspic33AK_curiosity_T1S/default.elf`

---

## Project Structure

| Path | Purpose |
|------|---------|
| `src/main.c` | Main application, role selection, heartbeat/iperf orchestration |
| `src/app.c/.h` | Button debounce, long-press detection, boot role check |
| `src/T1S/udp_heartbeat.c/.h` | UDP heartbeat protocol (1-byte LED sync + RTT) |
| `src/T1S/udp_perf_client.c/.h` | iperf UDP throughput test (sender + receiver) |
| `src/T1S/t1s_lwip.c/.h` | T1S/PLCA/lwIP initialization with configurable nodeId/IP |
| `src/T1S/tcp_server.c` | TCP server |
| `src/systick/systick.c/.h` | Free-running 32-bit timer (10ns resolution, no ISR) |
| `src/hal/` | Hardware abstraction (SPI DMA, systick HAL, T1S HAL) |

| `cmake/` | CMake build system (auto-generated by MPLAB) |
| `libs/` | Pre-compiled libraries (lwIP, TC6, X2Cscope) |

---

## License

Copyright (c) 2026 Microchip Technology Inc. and its subsidiaries. All rights reserved.

Subject to your compliance with these terms, you may use Microchip software and any derivatives exclusively with Microchip products.
