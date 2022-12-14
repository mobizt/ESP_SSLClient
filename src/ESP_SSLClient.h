/**
 *
 * The ESP SSL Client Class, ESP_SSLClient.h v1.1.1
 *
 * Created January 3, 2023
 *
 * The MIT License (MIT)
 * Copyright (c) 2023 K. Suwatchai (Mobizt)
 *
 *
 * Permission is hereby granted, free of charge, to any person returning a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef ESP_SSLCLIENT_H
#define ESP_SSLCLIENT_H

#include "ESP_SSLClient_FS.h"

#if defined(ESP32) || defined(USE_MBEDTLS_SSL_ENGINE)
#include "esp32/MB_ESP32_TCPClient.h"
#define MB_ESP_TCP_CLIENT MB_ESP32_TCPClient
#elif defined(USE_BSSL_SSL_ENGINE)
#include "esp8266/BSSL_SSLClient.h"
#define MB_ESP_TCP_CLIENT BSSL_SSLClient
#endif

class ESP_SSLClient : public MB_ESP_TCP_CLIENT
{
private:
    
public:
    ESP_SSLClient(){};
    ~ESP_SSLClient(){};
};

#endif