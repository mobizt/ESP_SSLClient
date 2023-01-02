/**
 *
 * The TCP Client Class that supports external network interfaces for ESP8266 and Raspberry Pi v1.0.0
 * 
 * This work is based on Arduino-Pico https://github.com/earlephilhower/arduino-pico by Earle F. Philhower.
 *
 * Created january 2, 2023
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

/*
    WiFiClientBearSSL- SSL client/server for esp8266 using BearSSL libraries
    - Mostly compatible with Arduino WiFi shield library and standard
    WiFiClient/ServerSecure (except for certificate handling).

    Copyright (c) 2018 Earle F. Philhower, III

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#pragma once

#include "BSSL_SSLClient_FS.h"

#if defined(USE_BSSL_SSL_ENGINE)

#if defined(ESP8266)

#ifdef __GNUC__
#if __GNUC__ > 4 || __GNUC__ == 10
#if defined(ARDUINO_ESP8266_GIT_VER)
#if ARDUINO_ESP8266_GIT_VER > 0
#define ESP8266_CORE_SDK_V3_X_X
#endif
#endif
#endif
#endif

#include <Arduino.h>
#include <bearssl/bearssl.h>
#include <vector>
#include <StackThunk.h>
#include <sys/time.h>
#include <IPAddress.h>
#include <Client.h>
#include <FS.h>
#include <time.h>
#include <ctype.h>
#include <vector>
#include <algorithm>

#else

#include <Arduino.h>
#include <bearssl/bearssl.h>
#include <Updater.h>
#include <StackThunk.h>

#endif

#ifdef DEBUG_ESP_SSL
#ifndef BSSL_SSLCLIENT_ENABLE_DEBUG
#define BSSL_SSLCLIENT_ENABLE_DEBUG
#endif
#endif

#ifdef DEBUG_ESP_PORT
#define BSSL_SSLCLIENT_DEBUG_PORT DEBUG_ESP_PORT
#endif

#ifndef BSSL_SSLCLIENT_DEBUG_PORT
#define BSSL_SSLCLIENT_DEBUG_PORT Serial
#endif

#ifdef BSSL_SSLCLIENT_ENABLE_DEBUG
#define BSSL_SSLCLIENT_DEBUG_PRINTF(fmt, ...) BSSL_SSLCLIENT_DEBUG_PORT.printf_P((PGM_P)PSTR("BSSL:" fmt), ##__VA_ARGS__)
#else
#define BSSL_SSLCLIENT_DEBUG_PRINTF(...)
#endif

namespace BearSSL
{

    // Opaque object which wraps the BearSSL SSL session to make repeated connections
    // significantly faster.  Completely optional.
    class MB_BSSL_SSL_Client;

    // Cache for a TLS session with a server
    // Use with BearSSL::WiFiClientSecure::setSession
    // to accelerate the TLS handshake
    class BearSSL_Session
    {
        friend class MB_BSSL_SSL_Client_Ctx;

    public:
        BearSSL_Session()
        {
            memset(&_session, 0, sizeof(_session));
        }

    private:
        br_ssl_session_parameters *getSession()
        {
            return &_session;
        }
        // The actual BearSSL session information
        br_ssl_session_parameters _session;
    };

    // Represents a single server session.
    // Use with BearSSL::ServerSessions.
    typedef uint8_t MB_ServerSession[100];

    // Cache for the TLS sessions of multiple clients.
    // Use with BearSSL::WiFiServerSecure::setCache
    class BearSSL_ServerSessions
    {
        friend class MB_BSSL_SSL_Client_Ctx;

    public:
        // Uses the given buffer to cache the given number of sessions and initializes it.
        BearSSL_ServerSessions(MB_ServerSession *sessions, uint32_t size) : BearSSL_ServerSessions(sessions, size, false) {}

        // Dynamically allocates a cache for the given number of sessions and initializes it.
        // If the allocation of the buffer wasn't successful, the value
        // returned by size() will be 0.
        BearSSL_ServerSessions(uint32_t size) : BearSSL_ServerSessions(size > 0 ? new MB_ServerSession[size] : nullptr, size, true) {}

        ~BearSSL_ServerSessions()
        {
            if (_isDynamic && _store != nullptr)
            {
                delete _store;
            }
        }

        // Returns the number of sessions the cache can hold.
        uint32_t size()
        {
            return _size;
        }

    private:
        BearSSL_ServerSessions(MB_ServerSession *sessions, uint32_t size, bool isDynamic) : _size(sessions != nullptr ? size : 0),
                                                                                               _store(sessions), _isDynamic(isDynamic)
        {
            if (_size > 0)
            {
                br_ssl_session_cache_lru_init(&_cache, (uint8_t *)_store, size * sizeof(MB_ServerSession));
            }
        }

        // Returns the cache's vtable or null if the cache has no capacity.
        const br_ssl_session_cache_class **getCache()
        {
            return _size > 0 ? &_cache.vtable : nullptr;
        }

        // Size of the store in sessions.
        uint32_t _size;
        // Store where the information for the sessions are stored.
        MB_ServerSession *_store;
        // Whether the store is dynamically allocated.
        // If this is true, the store needs to be freed in the destructor.
        bool _isDynamic;

        // Cache of the server using the _store.
        br_ssl_session_cache_lru _cache;
    };

};

#endif
