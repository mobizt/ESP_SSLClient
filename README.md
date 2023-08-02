# ESP SSLClient

![Compile](https://github.com/mobizt/ESP_SSLClient/actions/workflows/compile_library.yml/badge.svg) ![Examples](https://github.com/mobizt/ESP_SSLClient/actions/workflows/compile_examples.yml/badge.svg)  [![Github Stars](https://img.shields.io/github/stars/mobizt/ESP_SSLClient?logo=github)](https://github.com/mobizt/ESP_SSLClient/stargazers) ![Github Issues](https://img.shields.io/github/issues/mobizt/ESP_SSLClient?logo=github)

![arduino-library-badge](https://www.ardu-badge.com/badge/ESP_SSLClient.svg) ![PlatformIO](https://badges.registry.platformio.org/packages/mobizt/library/ESP_SSLClient.svg)

The upgradable SSL Client for ESP8266, ESP32, Raspberry Pi Pico (RP2040) and all Arduino devices that supports external networking interfaces e.g., WiFiClient, EthernetClient, and GSMClient.

This library provided the Secure Layer Networking (SSL/TLS) TCP Client.

The Arduino Client library buffer size should be large enough (1k or more) for transporting SSL data.

The RP2040 boards required Arduino-Pico SDK from Earle F. Philhower https://github.com/earlephilhower/arduino-pico

This library used light weight SSL engine library BearSSL for secure data encryption and decryption.

In ESP8266 device, the native BearSSL in Core SDK will be used and built-in BearSSL library will be used for other Arduino devices.

This library is fully compatible and able to work with [ESP-Mail-Client](https://github.com/mobizt/ESP-Mail-Client) library seamlessly.


### Supposted Arduino Devices with flash size > 128k.

 * ESP32
 * ESP8266
 * Arduino SAMD
 * Arduino STM32
 * Arduino AVR
 * Teensy 3.1 to 4.1
 * Arduino Nano RP2040 Connect
 * Raspberry Pi Pico
 
 ### Supposted Networking Devices with Client (with driver) library.

 * WIZnet Wxxx series modules
 * All SPI Ethernet modules
 * All GSM modules
 * All WiFi modules

 
## Features

* **Using BearSSL SSL engine for all devices (V2.0.0).**

* **Supports all Arduino devices with enough program flash space (128k or more).**

* **First and only one SSL Client library that supports SSL/TLS connection upgrades.**

* **Support STARTTLS in Email application.**

* **Use Client pointer without copy to constructor then it can be changed seemlessly at run time.**

* **The receive and transmit buffer memory can be reserved as supported by BearSSL.**

* **Easy to use as it provides the same interface functions as in ESP8266's WiFiClientSecure.**

* **Supports the authentications as in WiFiClientSecure.**



## Basic Usage
```cpp
  #include <Arduino.h>
  #if defined(ESP32) || defined(PICO_RP2040)
  #include <WiFi.h>
  #elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #endif
  #include <ESP_SSLClient.h>

  ESP_SSLClient ssl_client;
  EthernetClient basic_client;
  
  // ignore server ssl certificate verification
  ssl_client.setInsecure();

  /** Call setDebugLevel(level) to set the debug
  * esp_ssl_debug_none = 0
  * esp_ssl_debug_error = 1
  * esp_ssl_debug_warn = 2
  * esp_ssl_debug_info = 3
  * esp_ssl_debug_dump = 4
  */
  ssl_client.setDebugLevel(1);

  // Set the receive and transmit buffers size in bytes for memory allocation (512 to 16384).
  // For server that does not support SSL fragment size negotiation, leave this setting the default value
  // by not set any buffer size or set the rx buffer size to maximum SSL record size (16384) and 512 for tx buffer size.  
  ssl_client.setBufferSizes(1024 /* rx */, 512 /* tx */);
  
  // assign the basic client
  ssl_client.setClient(&basic_client);

  Serial.print("Connecting to server...");

  String payload = "{\"title\":\"hello\"}";

  if (ssl_client.connect("reqres.in", 443))
  {
    Serial.println(" ok");
    Serial.println("Send POST request...");
    ssl_client.print("POST /api/users HTTP/1.1\n");
    ssl_client.print("Host: reqres.in\n");
    ssl_client.print("Content-Type: application/json\n");
    ssl_client.print("Content-Length: ");
    ssl_client.print(payload.length());
    ssl_client.print("\n\n");
    ssl_client.print(payload);

    Serial.print("Read response...");

    unsigned long ms = millis();
    while (!ssl_client.available() && millis() - ms < 3000)
    {
      delay(0);
    }
    
    Serial.println();
    while (ssl_client.available())
    {
      Serial.print((char)ssl_client.read());
    }
    Serial.println();
  }
  else
    Serial.println(" failed\n");

  ssl_client.stop();

```

## RP2040 Arduino SDK installation

For Arduino IDE, the Arduino-Pico SDK can be installed from Boards Manager by searching pico and choose Raspberry Pi Pico/RP2040 to install.

For PlatformIO, the Arduino-Pico SDK can be installed via platformio.ini

```ini
[env:rpipicow]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = rpipicow
framework = arduino
board_build.core = earlephilhower
monitor_speed = 115200
```

See this Arduino-Pico SDK [documentation](https://arduino-pico.readthedocs.io/en/latest/) for more information.



## Functions Interfaces

The ESP_SSLClient available functions are similare to the WiFiClientSecure.h class in ESP8266.

The Sessions and ServerSessions for ESP8266 BearSSL will be renamed to BearSSL_Sessions and BearSSL_ServerSessions respectively.

For ESP32, the certificate bundle is not currenty supported.

## MIT License

Copyright (c) 2023 mobizt

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
