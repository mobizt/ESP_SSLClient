# ESP SSLClient

The upgradable SSL Client for ESP8266, ESP32 and and Raspberry Pi (Arduino Pico RP2040) that supports external network interfaces e.g., WiFiClient, EthernetClient, and GSMClient.

This library is able to use in SMTP and IMAP applications and library with STARTTLS command.

The Arduino Client library buffer size should be large enough (1k or more) for transporting SSL data.

The RP2040 boards can be used by installing Arduino-Pico SDK from Earle F. Philhower https://github.com/earlephilhower/arduino-pico

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

## Raspberry Pi Pico Reliability Concerns

The Raspbery Pi Pico is really cheap board which is not 100% working due to defective board, flash chip and wiring as you can see some hardware issues from internet search. 

There are some devices hang included me when writing data to filesystem (LittleFS) while reading data from filesystem is ok.


```ini
[env:rpipicow]
platform = https://github.com/maxgerhardt/platform-raspberrypi.git
board = rpipicow
framework = arduino
board_build.core = earlephilhower
monitor_speed = 115200
```

See this Arduino-Pico SDK [documentation](https://arduino-pico.readthedocs.io/en/latest/) for more information.

## Known Issues

Due to unexpected behavior in ESP32 Arduino Client class when using in HTTP request that some data at the end of transaction can be lost especially large response payload case. It may happen after server close the connection after "Connection: Close" header was sent. This issue happens in ESP32 Arduino Core SDK v2.0.x.

This issue may not exist in case small response payload unless large response payload case that the Arduino Client closed before the payload reading to be completed.

You can avoid this issue by keeping the connection open by sending "Connection: Keep-Alive" header in HTTP request or with content type header "text/event-stream" in SSE API.


## Functions Interfaces

The ESP_SSLClient available functions are similare to the WiFiClientSecure.h class in ESP32 and ESP8266.

The PrivateKey, X509List and CertStoreBase classes for ESP8266 will be renamed to BearSSL_PrivateKey, BearSSL_X509List and BearSSL_CertStoreBase respectively.

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
