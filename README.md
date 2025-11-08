# 🛡️ ESP_SSLClient: Universal BearSSL/TLS Client for Arduino

**A flexible and secure SSL/TLS client library for Arduino, enabling encrypted communication (HTTPS, SMTPS, etc.) on virtually *any* board or external network interface (Ethernet, GSM, WiFiClient, etc.).**

Supports ESP32, ESP8266, RP2040, STM32, SAMD, Teensy, and AVR (with external SRAM). Compatible with WiFiClient, EthernetClient, GSMClient, and ESP-Mail-Client.

---

![Arduino Library](https://img.shields.io/badge/Arduino-Library-blue)
![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![Platform](https://img.shields.io/badge/Platform-ESP32%2C%20AVR%2C%20RP2040%2C%20STM32-green)
![BearSSL](https://img.shields.io/badge/SSL-BearSSL-lightgrey)

---

## 🔧 Features

- 🛡️ **Secure Data:** Data encryption via BearSSL (native or bundled)
- **⬆️ Duplex Modes:** Supports **Full-Duplex** (simultaneous RX/TX) and **Half-Duplex** (sequential RX or TX) operation for advanced memory optimization.
- 🔄 **TLS Upgrade:** Support for protocol negotiation (e.g., STARTTLS)
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



### Installation



Use Arduino Library Manager or clone this repo:



```bash

git clone https://github.com/mobizt/ESP_SSLClient.git

```



### RP2040 Setup



For Arduino IDE:  

Install Arduino-Pico SDK via Boards Manager → search “Pico”.



For PlatformIO:



```ini

[env:rpipicow]  

platform = https://github.com/maxgerhardt/platform-raspberrypi.git  
board = rpipicow  
framework = arduino  
board_build.core = earlephilhower  
monitor_speed = 115200

```


## 🧪 Basic Usage

```cpp
// --- 1. OPTIMIZATION MACROS ---
// Define these globally (before any library includes) to enable compile-time memory optimization.
// NOTE: When defined, the subsequent ssl_client.setBufferSizes() call is ignored or guarded.
#define SSLCLIENT_HALF_DUPLEX

// The Receive (RX/In) buffer size should ideally be 16709 bytes (16384 max payload + 325 overhead)
// to safely accommodate any standard TLS record.
// 
// NOTE: A minimum size of 2048 bytes is required here because the specific application 
// (HTTP response headers/body) is expected to contain a large payload, factoring in TLS overhead.
// This size is viable only if the server supports Maximum Fragment Length Negotiation (MFLN), 
// which limits the record size to fit this buffer, or if the application ensures smaller payloads.
#define STATIC_IN_BUFFER_SIZE 2048

// The Transmit (TX/Out) buffer size should ideally be 16469 bytes (16384 max payload + 85 overhead)
// to safely handle the largest possible outgoing TLS record.
// The Transmit (TX/Out) buffer primarily handles outbound request headers and body.
// Since BearSSL can fragment large writes internally, a minimum size of 512 bytes 
// is typically sufficient for most application protocols (e.g., HTTP requests).
// For this case that SSLCLIENT_HALF_DUPLEX is defined, the following macro is ignored.
#define STATIC_OUT_BUFFER_SIZE 512

#include <ESP_SSLClient.h>
#include <EthernetClient.h> 

EthernetClient basic_client; 
ESP_SSLClient ssl_client;

void setup() {
    Serial.begin(115200);

    Serial.print("ESP_Client version ");
    Serial.println(ESP_SSLCLIENT_VERSION);

    // Starting network connection
    Ethernet.begin();

    // 2. Assign the underlying network interface immediately.
    ssl_client.setClient(&basic_client);
    
    // 3. Set buffer sizes (Conditionally called only if buffers are NOT static)
#if !defined(STATIC_IN_BUFFER_SIZE) || !defined(STATIC_OUT_BUFFER_SIZE)
    // NOTE: This call only performs buffer size adjustment if the corresponding 
    // STATIC_IN/OUT_BUFFER_SIZE macros were NOT defined at compile time.
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

     // 5.Normally for effectiveness, the response reading should be done in loop()
     // If the response is processed in the local function, it should be wait for 
     // its available before reading 
    while (!ssl_client.available())
        delay(0);
    while (ssl_client.available())
        Serial.print((char)ssl_client.read());

    if (!ssl_client.connected()) {
        ssl_client.stop();
        Serial.println(F("\nConnection closed."));
        while(true) delay(100); 
    }
}

void loop() {
}
```

---

## 🔍 API Highlights

| Method | Description |
|---|---|
| `**setClient(Client *client)**` | **CRITICAL: Assigns the underlying transport layer (e.g., `WiFiClient`, `EthernetClient`) for secure connection.** |
| `setClient(Client *client, bool enableSSL)` | Assign transport client and optionally enable/disable SSL immediately |
| `setDebugLevel(int level)` | Set debug verbosity (0-4) |
| `setBufferSizes(size_t rx, size_t tx)` | Configure memory buffers |
| `connectSSL()` | Upgrade an existing connection to SSL/TLS (e.g., for STARTTLS) |
| `validate(host/IP, port)` | Verify connection target |
| `enableSSL(bool)` | Toggle SSL layer |
| `setTimeout(uint32_t)` | TCP timeout |
| `setSessionTimeout(uint32_t)` | Session timeout (ESP32 only) |

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
| **ESP32** | 15.1% (48.46 KB) | 75.3% (964.2 KB) | **Highly Efficient.** The aggressive static allocation provides performance stability, though dynamic allocation (if memory fragmentation is managed) remains viable on this platform. |
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

## 💾 External RAM for Arduino Mega 2560 (Recommended)

The Arduino Mega 2560's built-in 8 kB of SRAM is insufficient for SSL/TLS operations. You must use external RAM. We **strongly recommend** the **Parallel SRAM** method for its reliability and ease of use.

### Recommended Solution: Parallel SRAM Shields (≥ 512 kB) 🥇

This approach uses the Mega's built-in **External Memory Interface (EMI)** to seamlessly map external memory into the CPU's address space.

* **Hardware:** Dedicated peripheral shields like **QuadRAM** or **XMEM+**. These boards connect to the Mega's parallel Address and Data buses.
* **Key Advantage:** The external memory is treated as internal RAM. Standard C functions like **`malloc()` and `new` use the external RAM**, and the **Stack is safely relocated**, virtually eliminating memory overflow issues common with the small internal 8 kB of SRAM.
* **Setup:** Requires minimal two-line hardware register setup to enable the EMI, followed by relocating the C runtime pointers (`__malloc_heap_start`, etc.) to the shield's address range. **No complex drivers are needed.**

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