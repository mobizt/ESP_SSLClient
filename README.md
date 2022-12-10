# ESP SSLClient

The SSL Client for ESP8266 and ESP32 that supports many network interfaces e.g., WiFiClient, EthernetClient, GSMClient.

## Basic Usage
```cpp
  #include "ESP_SSLClient/esp32/MB_ESP32_SSLClient.h"

  MB_ESP32_SSLClient sslClient;
  EthernetClient basicClient;
  
  // ignore server ssl certificate verification
  sslClient.setInsecure();
  
  // assign the basic client
  sslClient.setClient(&basicClient);

  Serial.print("Connecting to server...");

  String payload = "{\"title\":\"hello\"}";

  if (sslClient.connect("reqres.in", 443))
  {
    Serial.println(" ok");
    Serial.println("Send POST request...");
    sslClient.print("POST /api/users HTTP/1.1\n");
    sslClient.print("Host: reqres.in\n");
    sslClient.print("Content-Type: application/json\n");
    sslClient.print("Content-Length: ");
    sslClient.print(payload.length());
    sslClient.print("\n\n");
    sslClient.print(payload);

    Serial.print("Read response...");

    while (!sslClient.available())
    {
    }
    Serial.println();
    while (sslClient.available())
    {
      Serial.print((char)sslClient.read());
    }
    Serial.println();
  }
  else
    Serial.println(" failed\n");

  sslClient.stop();

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
