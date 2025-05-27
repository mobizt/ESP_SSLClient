/*
 * SPDX-FileCopyrightText: 2025 Suwatchai K. <suwatchai@outlook.com>
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef ESP_SSLCLIENT_H
#define ESP_SSLCLIENT_H


#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wvla"

#include <Arduino.h>
#include <Client.h>

#if (defined(ESP8266) || defined(ARDUINO_ARCH_RP2040)) && !defined(ARDUINO_NANO_RP2040_CONNECT)
#define USE_EMBED_SSL_ENGINE
#else
#define USE_LIB_SSL_ENGINE
#endif

#if defined(__AVR__)
#undef USE_LIB_SSL_ENGINE
#undef USE_EMBED_SSL_ENGINE
#error "Not support AVR architecture"
#endif

#include <memory>
#include <vector>


#if defined(USE_EMBED_SSL_ENGINE) && !defined(ARDUINO_ARCH_RP2040) && !defined(ARDUINO_NANO_RP2040_CONNECT)
#define EMBED_SSL_ENGINE_BASE_OVERRIDE override
#else
#define EMBED_SSL_ENGINE_BASE_OVERRIDE
#endif

#if defined(ENABLE_FS) && defined __has_include && __has_include(<FS.h>)
#include <FS.h>
#else
#undef ENABLE_FS
#endif

#define BSSL_SSL_CLIENT_MIN_SESSION_TIMEOUT_SEC 60
#define ESP_SSLCLIENT_VALID_TIMESTAMP 1690979919

#ifndef SSLCLIENT_CONNECTION_UPGRADABLE
#define SSLCLIENT_CONNECTION_UPGRADABLE
#endif

#ifdef ENABLE_DEBUG
#if !defined(DEBUG_PORT)
#define DEBUG_PORT Serial
#endif
#define ESP_SSLCLIENT_DEBUG_PRINT DEBUG_PORT.print
#else
#define ESP_SSLCLIENT_DEBUG_PRINT(...)
#endif

#if defined(USE_EMBED_SSL_ENGINE) || defined(USE_LIB_SSL_ENGINE)

enum esp_ssl_client_debug_level
{
    esp_ssl_debug_none = 0,
    esp_ssl_debug_error = 1,
    esp_ssl_debug_warn = 2,
    esp_ssl_debug_info = 3,
    esp_ssl_debug_dump = 4
};

enum esp_ssl_client_error_types
{
    esp_ssl_ok,
    esp_ssl_connection_fail,
    esp_ssl_write_error,
    esp_ssl_read_error,
    esp_ssl_out_of_memory,
    esp_ssl_internal_error
};

#if defined(ENABLE_DEBUG)

static void esp_ssl_debug_print_prefix(const char *func_name, int level)
{
    ESP_SSLCLIENT_DEBUG_PRINT(PSTR("> "));
    // print the debug level
    switch (level)
    {
    case esp_ssl_debug_info:
        ESP_SSLCLIENT_DEBUG_PRINT(PSTR("INFO."));
        break;
    case esp_ssl_debug_warn:
        ESP_SSLCLIENT_DEBUG_PRINT(PSTR("WARN."));
        break;
    case esp_ssl_debug_error:
        ESP_SSLCLIENT_DEBUG_PRINT(PSTR("ERROR."));
        break;
    default:
        break;
    }

    // print the function name
    ESP_SSLCLIENT_DEBUG_PRINT(PSTR(""));
    ESP_SSLCLIENT_DEBUG_PRINT(func_name);
    ESP_SSLCLIENT_DEBUG_PRINT(PSTR(": "));
}

static void esp_ssl_debug_print(PGM_P msg, int debug_level, int level, const char *func_name)
{
    if (debug_level >= level)
    {
        esp_ssl_debug_print_prefix(func_name, level);
        ESP_SSLCLIENT_DEBUG_PRINT(msg);
        ESP_SSLCLIENT_DEBUG_PRINT("\r\n");
    }
}

#endif

static uint8_t htoi(unsigned char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    else if (c >= 'A' && c <= 'F')
        return 10 + c - 'A';
    else if (c >= 'a' && c <= 'f')
        return 10 + c - 'a';
    else
        return 255;
}

// Helper function which aborts a TLS handshake by sending TLS
// ClientAbort and ClientClose messages.
static bool send_abort(Client *probe, bool supportsLen)
{
    // If we're still connected, send the appropriate notice that
    // we're aborting the handshake per RFCs.
    static const uint8_t clientAbort_P[] PROGMEM = {
        0x15 /*alert*/, 0x03, 0x03 /*TLS 1.2*/, 0x00, 0x02,
        1, 90 /* warning: user_cancelled */
    };
    static const uint8_t clientClose_P[] PROGMEM = {
        0x15 /*alert*/, 0x03, 0x03 /*TLS 1.2*/, 0x00, 0x02,
        1, 0 /* warning: close_notify */
    };
    if (probe->connected())
    {
        uint8_t msg[sizeof(clientAbort_P)];
        memcpy_P(msg, clientAbort_P, sizeof(clientAbort_P));
        probe->write(msg, sizeof(clientAbort_P));
        memcpy_P(msg, clientClose_P, sizeof(clientClose_P));
        probe->write(msg, sizeof(clientClose_P));
    }
    return supportsLen;
}

const uint16_t _secure_ports[26] = {443 /* HTTPS */, 465 /* SMTP */, 563 /* NNTP */, 636 /* LDAPS */, 695 /* IEEE-MMS-SSL */, 832 /* NETCONF */, 853 /* DNS */, 989 /* FTPS */, 990 /* FTPS */, 992 /* Telnet */, 993 /* IMAP */, 995 /* POP3 */, 4116 /* Smartcard */, 4843 /* OPC */, 5061 /* SIP */, 5085 /* LLIP */, 5349 /* NAT */, 5671 /* AMQP */, 5986 /* WinRM-HTTPS */, 6513 /* NETCONF */, 6514 /* Syslog */, 6515 /* Elipse RPC */, 6619 /* OFTP */, 8243 /* Apache Synapse */, 8403 /* GxFWD */, 8883 /* MQTT */};

#endif

extern "C"
{
#include "bssl/bearssl.h"
}

#include "client/BSSL_TCP_Client.h"
#include "client/BSSL_TCP_Client.cpp"

class ESP_SSLClient : public BSSL_TCP_Client
{
public:
    ESP_SSLClient() {};
    ~ESP_SSLClient() {};
};

class ESP_SSLClient2 : public BSSL_TCP_Client
{
public:
    explicit ESP_SSLClient2(Client &client, bool enableSSL = true) : _base_client(client)
    {
        setClient(&_base_client, enableSSL);
    };
    ~ESP_SSLClient2() {};

private:
    Client &_base_client;
};
#endif
