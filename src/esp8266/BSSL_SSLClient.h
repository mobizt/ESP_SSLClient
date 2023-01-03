/**
 *
 * The TCP Client Class that supports external network interfaces for ESP8266 and Raspberry Pi v1.0.1
 * 
 * This work is based on Arduino-Pico https://github.com/earlephilhower/arduino-pico by Earle F. Philhower.
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

#ifndef BSSL_SSLCLIENT_H 
#define BSSL_SSLCLIENT_H

#include "BSSL_SSLClient_FS.h"

#if defined(USE_BSSL_SSL_ENGINE)

#include "MB_BSSL_TCPClient.h"

class BSSL_SSLClient : public MB_BSSL_TCPClient
{
private:
    
public:
    BSSL_SSLClient(){};
    ~BSSL_SSLClient(){};
};

#endif

#endif