# 🛡️ ESP_SSLClient: Universal BearSSL/TLS Client for Arduino

**A flexible and secure SSL/TLS client library for Arduino, enabling encrypted communication (HTTPS, SMTPS, etc.) on virtually *any* board or external network interface (Ethernet, GSM, WiFiClient, etc.).**

Supports ESP32, ESP8266, RP2040, STM32, SAMD, Teensy, and AVR (with external SRAM). Compatible with WiFiClient, EthernetClient, GSMClient, and ESP-Mail-Client.

---

![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/Platform-ESP32%2C%20AVR%2C%20RP2040%2C%20STM32-green)
![BearSSL](https://img.shields.io/badge/SSL-BearSSL-lightgrey)

---

## 📖 Table of Contents

1.  [🔧 Features](#-features)
2.  [📦 Supported Platforms](#-supported-platforms)
3.  [🚀 Getting Started](#-getting-started)
    * [✅ Installation](#-installation)
    * [📋 Requirements](#-requirements)
4.  [🧠 Memory Usage Discrepancy Note](#-memory-usage-discrepancy-note)
5.  [📦 Buffer Configuration Guide](#-buffer-configuration-guide)
    * [🧠 Notes](#-notes)
6.  [🧪 Basic Usage](#-basic-usage)
7.  [🧰 API Highlights](#-api-highlights)
8.  [🔬 Performance Profile Summary (Insecure Mode)](#-performance-profile-summary-insecure-mode)
9.  [🧠 Memory Expansion Guide (AVR)](#-memory-expansion-guide-avr)
10.  [🧰 Macro Summary](#-macro-summary)
11.  [🌐 Duplex Mode Guide: When to Use Half-Duplex](#-duplex-mode-guide-when-to-use-half-duplex)
12.  [🧪 Diagnostic Tips](#-diagnostic-tips)
13.  [📊 Supported Algorithms](#-supported-algorithms)
14.  [📄 License](#-license)

---

## 🔧 Features

- 🛡️ **Secure Data:** Data encryption via BearSSL (native or bundled)
- **⬆️ Duplex Modes:** Supports **Full-Duplex** (simultaneous RX/TX) and **Half-Duplex** (sequential RX or TX) operation for advanced memory optimization.
- 🔄 **TLS Upgrade:** Support for protocol negotiation (e.g., STARTTLS)
- 🌟 **Supported Protocols:** TLS1.0, TLS1.1, and TLS1.2.
- 🔐 **Validation:** Full Certificate validation or quick testing via `setInsecure()` mode
- 🔁 **Runtime Flexibility:** Client switching at runtime via pointer assignment
- 📦 **Configurable Buffers:** Adjust RX/TX buffer sizes (512–16384 bytes)
- 🧪 **Debugging:** Multiple debugging levels and detailed error string support
- 📁 **CertStore:** Root Certificate management via filesystem APIs
- 🧩 **Integration:** Seamless integration with ESP-Mail-Client
- 🧠 **Memory Management:** Static and dynamic memory allocation support

---

## 📦 Supported Platforms

### Arduino Boards

- **ESP32, ESP8266**
- **Raspberry Pi Pico / RP2040**
- **Arduino Nano RP2040 Connect**
- **STM32, SAMD, Teensy 3.1–4.1**
- **Arduino UNO R4 WiFi (Renesas)**
- **AVR boards** (e.g., Arduino Mega 2560 with external SRAM)

### Networking Interfaces

- **WIZnet W5100 / W5500 / Wxxx** series (Ethernet)
- **ENC28J60** (via lwIP)
- **GSM modules**
- **Standard WiFi modules**

> ⚠️ **Note on Native Ethernet:** Native PHY Ethernet chips (LAN8720, TLK101, etc.) are supported by ESP32 Core via `WiFiClientSecure` and `ETH.h`. This library is generally **not required** in those specific cases.

---

## 🚀 Getting Started

### ✅ Installation

You can install **ESP_SSLClient** via:

- **Arduino Library Manager**: Search for `ESP_SSLClient` and click **Install**
- **Manual Installation**:
  1. Download the ZIP from [GitHub Releases](https://github.com/mobizt/ESP_SSLClient/releases)
  2. Extract to your `libraries/` folder
  3. Restart the Arduino IDE

### 📋 Requirements

| Platform         | SRAM Usage (Insecure Mode) | Notes                                                                 |
|------------------|----------------------------|-----------------------------------------------------------------------|
| ESP8266          | ~34 KB                     | BearSSL built-in; efficient with static buffers                      |
| ESP32            | ~48 KB                     | Highly capable. Provides a memory-optimized alternative to the default client's large dynamic allocation (approx. 50 KB).|
| RP2040 Pico W    | ~75 KB                     | Excellent performance with static buffers                            |
| STM32F103C8      | ~7 KB                      | Requires aggressive optimization; insecure mode recommended          |
| **Arduino Mega 2560** | **15.6 KB (194.8%)**         | Native 8 KB SRAM is insufficient; **external SRAM required** (e.g., XMEM+) |
| Teensy 3.x–4.x   | 10–117 KB                  | Varies by model; supports full-duplex and large buffers              |

---

## 🧠 Memory Usage Discrepancy Note

### Why Usage Varies on 32-bit Platforms

Users observing memory usage on modern 32-bit platforms (e.g., ESP32, Teensy 4.x) may see higher total **RAM Usage** (variables + code) compared to highly constrained microcontrollers like SAMD or AVR.

This difference is primarily due to the platform's execution model, not the library's overhead:

1.  **Low-SRAM Devices (AVR/SAMD):** On these chips, the memory usage reported (often below 10 KB) accurately reflects the library's core **static data and global variables**. The executable code remains primarily in Flash.
2.  **High-Performance Devices (ESP/Teensy):** On these architectures, the compiler and linker are optimized for speed. They often map large portions of the **executable code** (up to 120 KB, as seen on Teensy 4.x) from Flash memory directly into high-speed **Data RAM (ITCM/OCRAM)** for faster execution.

Therefore, while this library's core data overhead remains small and stable (e.g., $\approx 7-15\text{ KB}$ on all chips), the Total RAM Usage figure is inflated by the platform's aggressive performance optimization.

**Always refer to the *Variable/Static* usage metrics to gauge the library's true memory footprint.**

## 📦 Buffer Configuration Guide

This table summarizes recommended buffer sizes based on duplex mode and expected payload size. Adjust these macros in your sketch or `platformio.ini` for optimal performance.

| Mode         | Macro(s) Required                | RX Buffer | TX Buffer | Notes                                                                 |
|--------------|----------------------------------|--------------------------------------|--------------------------------------|-----------------------------------------------------------------------|
| **Half-Duplex** | `SSLCLIENT_HALF_DUPLEX`          | 2048–16709 bytes                     | *Ignored*                            | RX buffer used for both directions. Ideal for HTTP/1.1 and low-RAM boards. |
| **Full-Duplex** | *(default)*                     | 2048–16384 bytes                     | 512–16469 bytes                      | Separate buffers for RX and TX. Required for streaming or pipelined protocols. |
| **Minimal Setup** | `SSLCLIENT_INSECURE_ONLY`<br>`STATIC_X509_CONTEXT`<br>`STATIC_SSLCLIENT_CONTEXT` | 512 bytes                            | 512 bytes                            | For testing or ultra-low RAM boards. No cert validation. Debug disabled. |

### 🧠 Notes

- Minimum RX size: 2048 bytes recommended to handle TLS record + headers.
- TX buffer: 512 bytes is sufficient for most request headers.
- Fragmentation: If server supports Maximum Fragment Length Negotiation (MFLN), smaller buffers may work.
- BearSSL: Can internally fragment large writes, but RX must still accommodate full record.

---
## 🧪 Basic Usage

```cpp
// --- 1. OPTIMIZATION MACROS ---
// Define these globally (before any library includes) to enable compile-time memory optimization.
// NOTE: These flags define the physical memory reserved by the client object.

#define SSLCLIENT_HALF_DUPLEX 

// The Receive (RX/In) buffer size should ideally be 16709 bytes (16384 max payload + 325 overhead)
// to safely accommodate any standard TLS record.
// 
// NOTE: A minimum size of 2048 bytes is required here because the specific application 
// (HTTP response headers/body) is expected to contain a large payload, factoring in TLS overhead.
#define STATIC_IN_BUFFER_SIZE 2048

// The Transmit (TX/Out) buffer size should ideally be 16469 bytes (16384 max payload + 85 overhead)
// to safely handle the largest possible outgoing TLS record.
// NOTE: This macro is ignored when SSLCLIENT_HALF_DUPLEX is defined.
#define STATIC_OUT_BUFFER_SIZE 512

#include <ESP_SSLClient.h>
#include <EthernetClient.h> 

EthernetClient basic_client; 
ESP_SSLClient ssl_client;

void setup() {
    Serial.begin(115200);

    // Starting network connection
    Ethernet.begin();

    Serial.print(F("ESP_Client version "));
    Serial.println(ESP_SSLCLIENT_VERSION);

    // 2. Assign the underlying network interface immediately.
    ssl_client.setClient(&basic_client);
    
    // 3. Set buffer sizes (Conditionally called only if buffers are NOT static)
    // NOTE: If the STATIC macros above were defined, setBufferSizes() is either ignored or
    // its parameters are used to determine the static buffer size.
#if !defined(STATIC_IN_BUFFER_SIZE) || !defined(STATIC_OUT_BUFFER_SIZE)
    ssl_client.setBufferSizes(2048, 512);
#endif

    // 4. Set security policy 
    ssl_client.setInsecure();
    ssl_client.setDebugLevel(3); 

    Serial.println(F("Connecting to the server..."));

    // --- Connection ---
    if (ssl_client.connect("reqres.in", 443))
    {
        Serial.println(F("Connected via TLS. Sending request..."));
        
        // NOTE: This standard HTTP/1.1 request-response pattern is strictly sequential, 
        // making it ideally suited for the RAM-saving Half-Duplex mode. 
        ssl_client.print(F("POST /api/users HTTP/1.1\r\n"));
        ssl_client.print(F("Host: reqres.in\r\n"));
        ssl_client.print(F("Content-Type: application/json\r\n"));
        ssl_client.print(F("Connection: close\r\n")); 
        ssl_client.print(F("Content-Length: 34\r\n")); 
        ssl_client.print(F("\r\n")); 
        ssl_client.print(F("{\"name\":\"morpheus\",\"job\":\"leader\"}"));
    } else {
        Serial.println(F("Connection failed."));
    }

    // --- Response Reading & Cleanup ---
    
    // 5. Normally for effectiveness, the response reading should be done in loop()
    // If the response is processed in the local function, it should be wait for 
    // its available before reading 
    while (!ssl_client.available())
        delay(0); // WARNING: tight loop for minimal systems. Use millis() timer for production.
        
    while (ssl_client.available())
        Serial.print((char)ssl_client.read());

    if (!ssl_client.connected()) {
        ssl_client.stop();
        Serial.println(F("\nConnection closed."));
        while(true) delay(100); 
    }
}

void loop() {
    // loop() remains empty as the example completes in setup().
}
```

---

## 🧰 API Highlights

| Method | Description |
|--------|-------------|
| `setClient(Client *client)` | Assigns the underlying transport layer |
| `setBufferSizes(rx, tx)` | Configure memory buffers |
| `connectSSL()` | Upgrade an existing connection to SSL/TLS |
| `validate(host, port)` | Verify connection target |
| `setInsecure()` | Disable certificate validation |
| `setDebugLevel(level)` | Set debug verbosity (0–4) |


> Full API: [`src/Readme.md`](https://github.com/mobizt/ESP_SSLClient/blob/main/src/Readme.md) or [`src/client/TCPClient.h`](https://github.com/mobizt/ESP_SSLClient/blob/main/src/client/TCPClient.h)


---

## 🔬 Performance Profile Summary (Insecure Mode)


This table shows the resource consumption when built with maximum resource conservation using the following static, minimized, and unsecured configuration:

```cpp
#define SSLCLIENT_INSECURE_ONLY
#define STATIC_IN_BUFFER_SIZE 512
#define STATIC_OUT_BUFFER_SIZE 512
#define STATIC_X509_CONTEXT
#define STATIC_SSLCLIENT_CONTEXT
```
By allowing the debug and the debug information string by defining ENABLE_DEBUG and ENABLE_ERROR_STRING will increase the program space.

**Note: Debug and error string options are disabled and no netork client library included in the compilation result below for minimum program space.**


| Platform | RAM Usage | Flash Usage | Notes |
| :--- | :--- | :--- | :--- |
| **ESP32 (Arduino Core v3.3.3)** | 15.1% (48.46 KB) | 75.3% (964.2 KB) | **Highly Efficient.** The aggressive static allocation provides performance stability, though dynamic allocation (if memory fragmentation is managed) remains viable on this platform. |
| **ESP8266 (Wemos D1)** | 42.6% (34.1 KB) | 34.2% (348.37 KB) | **RAM Conscious.** The static/insecure configuration is crucial here, reserving adequate free RAM for the heap and network stack. |
| **RP2040 Pico W** | 29.1% (74.6 KB) | 16% (162.92 KB) | **Excellent Performance.** Even with static reservation, ample resources remain. The configuration ensures deterministic memory usage. |
| **Nano RP2040 Connect** | 18.3% (46.8 KB) | 0.2% (4.09 KB) | **Minimal Flash Footprint.** The tiny flash usage figure suggests effective removal of unnecessary code sections by the linker. |
| **STM32F103C8** | 36.4% (7.27 KB) | 61.6% (78.8 KB) | **Memory Critical.** This minimal configuration is essential for viability, as this MCU has very little onboard RAM. |
| **STM32F411CEU6** | 5.7% (7.29 KB) | 15.3% (78.51 KB) | **SRAM Optimized.** Very low RAM usage, providing extensive space for application data and heap operations. |
| **STM32F401CC** | 11.4% (7.29 KB) | 30.7% (78.5 KB) | **SRAM Optimized.** Excellent RAM margin, well-suited for stable heap operations and extensive network handling. |
| **STM32H743** | 1.5% (7.5 KB) | 4.4% (81.98 KB) | **Flagship Performance.** Minimal RAM usage relative to massive available resources, ideal for complex applications. |
| **UNO R4 WiFi** | 28.3% (9.05 KB) | 43.4% (111.06 KB) | **Stable Operation.** The optimization ensures the SSL layer fits well within the 32 KB SRAM provided by the R4. |
| **Mega 2560** | ⚠️ **194.8% (15.58 KB)** | 70.2% (174.16 KB) | **Requires External Memory.** Confirms that even the most optimized configuration exceeds the native 8 KB SRAM. External memory (XMEM+) is mandatory. |
| **MKR1000 USB** | 26.6% (8.49 KB) | 31.0% (79.29 KB) | **Secure and Efficient.** The static allocation uses minimal SRAM relative to the total available. |
| **Teensy 3.0** | **61.7% (9.87 KB)** | 69.5% (89.01 KB) | **RAM Critical.** Uses most available SRAM (16 KB); suitable only for small applications with minimal heap use. |
| **Teensy 3.1** | 15.7% (19.07 KB) | 34.8% (89.07 KB) | **Solid Performance.** Uses a moderate amount of RAM (128 KB SRAM total), leaving ample space for networking buffers. |
| **Teensy 3.5** | 3.9% (10.04 KB) | 17.4% (88.8 KB) | **High RAM Margin.** Low resource consumption relative to its 192 KB SRAM capacity, highly stable. |
| **Teensy 3.6** | 3.9% (10.1 KB) | 8.7% (89.01 KB) | **High RAM Margin.** Excellent stability due to minimal memory usage relative to 256 KB SRAM. |
| **Teensy 4.0** | 22.09% (117.19 KB) | 5.03% (103.00 KB) | **High-Speed Profile.** RAM usage reflects executable code placed in high-speed OCRAM for peak performance. |
| **Teensy 4.1** | 22.88% (117.22 KB) | 1.24% (101.99 KB) | **High-Performance.** The high RAM usage is primarily due to the linker placing executable code into high-speed OCRAM for peak M7 performance. |

---

## 🧠 Memory Expansion Guide (AVR)

For **Arduino Mega 2560**, native 8 KB SRAM is insufficient for SSL/TLS. Use external memory:

- **Recommended**: Parallel SRAM shields (e.g., XMEM+, QuadRAM)
- **Benefits**:
  - External memory is mapped into CPU address space
  - Standard `malloc()` and `new` use external RAM
  - Stack relocation eliminates overflow risks

---

## 🧰 Macro Summary

Define these in your main sketch or `platformio.ini` to customize the build.

| Macro | Purpose |
| :--- | :--- |
| `SSLCLIENT_HALF_DUPLEX` | **Enables Half-Duplex TLS mode.** Uses only the RX buffer for both sending and receiving; saves significant RAM but requires sequential communication. |
| `STATIC_IN_BUFFER_SIZE` | Defines the size (in bytes) for the static **Receive Buffer (RX)**. This buffer is also used for the Transmit Buffer (TX) when `SSLCLIENT_HALF_DUPLEX` is enabled. It must accommodate the largest expected incoming TLS record plus the application payload|
| `STATIC_OUT_BUFFER_SIZE` | Defines the size (in bytes) for the static **Transmit Buffer (TX)**. **Ignored** when `SSLCLIENT_HALF_DUPLEX` is enabled. |
| `SSLCLIENT_INSECURE_ONLY` | Disable cert/fingerprint checks entirely |
| `STATIC_X509_CONTEXT` | Use static cert context (for low-RAM boards) |
| `STATIC_SSLCLIENT_CONTEXT` | Use static SSL context (for low-RAM boards) |
| `ENABLE_DEBUG` | Enable debug printing |
| `ENABLE_ERROR_STRING` | Show detailed error messages |
| `DEBUG_PORT` | Define debug output port (default: `Serial`) |
| `BSSL_BUILD_EXTERNAL_CORE` | Use external BearSSL library (advanced) |
| `ENABLE_FS` | Enable filesystem for CertStore |
| `ENABLE_PSRAM` | Enable PSRAM support (ESP32 only) |

---

## 🌐 Duplex Mode Guide: When to Use Half-Duplex

| Mode | Memory Footprint | Suitable Application Protocols | Notes |
| :--- | :--- | :--- | :--- |
| **Full Duplex (Default)** | Larger (Requires separate RX & TX buffers). | **HTTP/2, WebSockets, MQTT, Long-lived interactive sessions.** | Allows simultaneous sending and receiving, essential for modern streaming and high-throughput protocols. |
| **Half Duplex (`SSLCLIENT_HALF_DUPLEX`)** | **Smallest** (Uses a single shared buffer). | **HTTP/1.1 (Standard Request/Response), SMTPS, POP3, IMAPS.** | Ideal for memory-critical boards. Only one direction of data transfer (read OR write) can be active at any moment. **Do not use for pipelined HTTP or streaming.** |

---

## 🧪 Diagnostic Tips

- **Use `setDebugLevel(4)`** to trace the BearSSL handshake step-by-step for connection issues.
- **Define `ENABLE_ERROR_STRING`** to get readable error messages instead of just error codes.
- **Use `setInsecure()`** if testing connectivity is your first goal (before implementing cert validation).
- **Check buffer sizes** if handshake fails (try increasing them, e.g., 2048/1024).
- **Validate transport client lifetime**—ensure the client pointer you pass to `setClient()` remains valid.

---

## 📊 Supported Algorithms

The core BearSSL implementation included in this library supports a comprehensive suite of modern and necessary cryptographic primitives:

| Component | Supported Algorithms |
| :--- | :--- |
| **TLS Protocols** | **TLS 1.2, TLS 1.1, TLS 1.0** |
| **Key Exchange (Kx)** | **ECDHE** (Ephemeral), **ECDH** (Static), **RSA** (Static) |
| **Authentication (Auth)** | **ECDSA** (Elliptic Curve), **RSA** |
| **Bulk Encryption** | **AES-128, AES-256, ChaCha20, 3DES** |
| **Encryption Modes** | **GCM** (Galois/Counter Mode), **CCM** (Counter with CBC-MAC), **CBC** (Cipher Block Chaining) |
| **Hash Functions** | **SHA-256, SHA-384** (Modern/Required), **SHA1, MD5** (Legacy/Compatibility) |
---

## 📄 License

MIT License

Copyright (c) 2025 Suwatchai K.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.