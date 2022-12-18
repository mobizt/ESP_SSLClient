# ESP SSLClient

The SSL Client for ESP8266 and ESP32 that supports many network interfaces e.g., WiFiClient, EthernetClient, GSMClient.

## Basic Usage
```cpp
  #include <Arduino.h>
  #if defined(ESP32)
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

    while (!ssl_client.available())
    {
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

MIT License

Copyright (c) 2022 mobizt

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
