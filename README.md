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

### Arduino Boards (Flash > 128KB)

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

#include <ESP_SSLClient.h>

ESP_SSLClient ssl_client;
EthernetClient basic_client;

ssl_client.setInsecure();    // Skip certificate verification
ssl_client.setDebugLevel(1); // Debug: 0–4
ssl_client.setBufferSizes(1024, 1024);
ssl_client.setClient(&basic_client);

if (ssl_client.connect("reqres.in", 443))
{

    ssl_client.print("POST /api/users HTTP/1.1\r\n");
    ssl_client.print("Host: reqres.in\r\n");
    ssl_client.print("Content-Type: application/json\r\n");
    ssl_client.print("x-api-key: reqres-free-v1\r\n");
    ssl_client.print("Content-Length: 27\r\n\r\n");
    ssl_client.print("{\"title\":\"hello\"}");
    while (!ssl_client.available())
        delay(0);
    while (ssl_client.available())
        Serial.print((char)ssl_client.read());
}

ssl_client.stop();

```

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

> Full API: `src/client/BSSL_TCP_Client.h`

---

## ⚙️ Configuration Guide by Platform

| Platform | Notes |
|---|---|
| **ESP32 / ESP8266** | ESP8266 uses native BearSSL; ESP32 uses bundled BearSSL. Optional PSRAM and filesystem support |
| **RP2040** | Requires Arduino-Pico SDK. Compatible with WiFi/Ethernet. Supports ESP-Mail-Client |
| **AVR** | **Needs external SRAM** (e.g., 23LC1024). Use `STATIC_*` macros. ~9.8 KB RAM, ~178 KB Flash |
| **Teensy / STM32 / SAMD** | Use compatible networking libraries. Define `STATIC_*` macros if memory-constrained. Supports ESP-Mail-Client |

---

## 🧰 Macro Summary

Define these in your main sketch or `platformio.ini` to customize the build.

| Macro | Purpose |
|---|---|
| `ENABLE_DEBUG` | Enable debug printing |
| `ENABLE_ERROR_STRING` | Show detailed error messages |
| `DEBUG_PORT` | Define debug output port (default: `Serial`) |
| `SSLCLIENT_INSECURE_ONLY` | Disable cert/fingerprint checks entirely |
| `STATIC_IN_BUFFER_SIZE` | RX buffer size (stack/static allocation) |
| `STATIC_OUT_BUFFER_SIZE` | TX buffer size (stack/static allocation) |
| `STATIC_X509_CONTEXT` | Use static cert context (for low-RAM boards) |
| `STATIC_SSLCLIENT_CONTEXT` | Use static SSL context (for low-RAM boards) |
| `BSSL_BUILD_EXTERNAL_CORE` | Use external BearSSL library (advanced) |
| `ENABLE_FS` | Enable filesystem for CertStore |
| `ENABLE_PSTAM` | Enable PSRAM support (ESP32 only) |

---

## 🔌 AVR Mega 2560 + SRAM Wiring (23LC1024)

| Mega 2560 Pin | 23LC1024 Pin |
|---|---|
| 50 (MISO)     | SO           |
| 51 (MOSI)     | SI           |
| 52 (SCK)      | SCK          |
| 53 (SS)       | CS           |
| GND           | VSS          |
| 3.3V or 5V    | VCC          |

> ⚠️ Use level shifter if 3.3V SRAM with 5V Mega.

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