/*
 * The Mobizt ESP32 TCP Client Class, MB_ESP32_TCPClient.cpp v1.0.3
 *
 * Created December 18, 2022
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 K. Suwatchai (Mobizt)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
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
  WiFiClientSecure.cpp - Client Secure class for ESP32
  Copyright (c) 2016 Hristo Gochkov  All right reserved.
  Additions Copyright (C) 2017 Evandro Luis Copercini.

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

#ifndef MB_ESP32_TCPClient_CPP
#define MB_ESP32_TCPClient_CPP

#if defined(ESP32) || defined(USE_MBEDTLS_SSL_ENGINE)

#include "MB_ESP32_TCPClient.h"
#include <lwip/sockets.h>
#include <lwip/netdb.h>
#include <errno.h>

#undef connect
#undef write
#undef read

MB_ESP32_TCPClient::MB_ESP32_TCPClient()
{
    _ssl = new ssl_ctx;
    ssl_init(_ssl);

    setClient(nullptr);

    _ssl->handshake_timeout = 120000;
    _use_insecure = false;
    _CA_cert = NULL;
    _cert = NULL;
    _private_key = NULL;
    _pskIdent = NULL;
    _psKey = NULL;
    next = NULL;
}

MB_ESP32_TCPClient::~MB_ESP32_TCPClient()
{
    stop();
    _ssl->basic_client = nullptr;
    delete _ssl;
    _ssl = nullptr;
}

void MB_ESP32_TCPClient::setClient(Client *client, bool enableSSL)
{
    _ssl->basic_client = client;
    _isSSL = enableSSL;
}

MB_ESP32_TCPClient &MB_ESP32_TCPClient::operator=(const MB_ESP32_TCPClient &other)
{
    stop();
    _ssl->basic_client = other._ssl->basic_client;
    _use_insecure = other._use_insecure;
    return *this;
}

void MB_ESP32_TCPClient::stop()
{
    if (!_ssl->basic_client)
        return;

    stop_tcp_connection(_ssl, _CA_cert, _cert, _private_key);

    if (_ssl->basic_client)
        _ssl->basic_client->stop();
}

int MB_ESP32_TCPClient::connect(IPAddress ip, uint16_t port)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    if (_pskIdent && _psKey)
        return connect(ip, port, _pskIdent, _psKey);
    return connect(ip, port, _CA_cert, _cert, _private_key);
}

int MB_ESP32_TCPClient::connect(IPAddress ip, uint16_t port, int32_t timeout)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    _timeout = timeout;
    return connect(ip, port);
}

int MB_ESP32_TCPClient::connect(const char *host, uint16_t port)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    if (_pskIdent && _psKey)
        return connect(host, port, _pskIdent, _psKey);
    return connect(host, port, _CA_cert, _cert, _private_key);
}

int MB_ESP32_TCPClient::connect(const char *host, uint16_t port, int32_t timeout)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    _timeout = timeout;
    return connect(host, port);
}

int MB_ESP32_TCPClient::connect(IPAddress ip, uint16_t port, const char *CA_cert, const char *cert, const char *private_key)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    return connect(ip.toString().c_str(), port, CA_cert, cert, private_key);
}

int MB_ESP32_TCPClient::_connect(const char *host, uint16_t port)
{

    if (!_ssl->basic_client)
        return -1;

    if (_timeout > 0)
    {
        _ssl->handshake_timeout = _timeout;
    }

    _ssl->basic_client->setTimeout(_timeout);

    _ssl->basic_client->connect(host, port);

    if (!_ssl->basic_client->connected())
    {
        stop();
        return -1;
    }

    _host = host;
    _port = port;

    return 1;
}

int MB_ESP32_TCPClient::connect(const char *host, uint16_t port, const char *CA_cert, const char *cert, const char *private_key)
{
    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    _withCert = true;

    if (!_ssl->basic_client->connected())
    {
        if (!_connect(host, port))
            return 0;
    }

    _CA_cert = CA_cert;
    _cert = cert;
    _private_key = private_key;

    if (_isSSL && !connectSSL())
        return 0;

    return 1;
}

int MB_ESP32_TCPClient::connect(IPAddress ip, uint16_t port, const char *pskIdent, const char *psKey)
{
    return connect(ip.toString().c_str(), port, pskIdent, psKey);
}

int MB_ESP32_TCPClient::connect(const char *host, uint16_t port, const char *pskIdent, const char *psKey)
{

    if (!_ssl->basic_client)
    {
        ssl_client_print( mb_ssl_client_str_1);
        return 0;
    }

    _withKey = true;

    log_v("MB_ESP32_TCPClient connect with PSK");

    if (!_ssl->basic_client->connected())
    {
        if (!_connect(host, port))
            return 0;
    }
    _pskIdent = pskIdent;
    _psKey = psKey;

    if (_isSSL && !connectSSL())
        return 0;

    return 1;
}

bool MB_ESP32_TCPClient::connectSSL()
{
    if (!_ssl->basic_client || !_ssl->basic_client->connected())
        return false;

    _isSSL = true;

    int ret = 0;
    if (_withCert)
        ret = connect_ssl(_ssl, _host.c_str(), _CA_cert, _cert, _private_key, NULL, NULL, _use_insecure);
    else if (_withKey)
        ret = connect_ssl(_ssl, _host.c_str(), NULL, NULL, NULL, _pskIdent, _psKey, _use_insecure);

    _lastError = ret;
    if (ret < 0)
    {
        log_e("MB_ESP32_TCPClient Error: SSL connection, %d", ret);
        stop();
        return 0;
    }

    return 1;
}

int MB_ESP32_TCPClient::peek()
{
    if (!_ssl->basic_client)
        return 0;

    if (!_ssl->basic_client->connected())
        return 0;

    return _ssl->basic_client->peek();
}

size_t MB_ESP32_TCPClient::write(uint8_t data)
{
    return write(&data, 1);
}

int MB_ESP32_TCPClient::read()
{
    if (!_ssl->basic_client)
        return 0;

    uint8_t data = -1;
    int res = read(&data, 1);
    if (res < 0)
    {
        return res;
    }
    return data;
}

size_t MB_ESP32_TCPClient::write(const uint8_t *buf, size_t size)
{
    if (!_ssl->basic_client)
        return 0;

    if (!_ssl->basic_client->connected())
        return 0;

    int res = _isSSL ? send_ssl_data(_ssl, buf, size) : (int)_ssl->basic_client->write(buf, size);
    if (res < 0)
    {
        stop();
        res = 0;
    }
    return res;
}

int MB_ESP32_TCPClient::read(uint8_t *buf, size_t size)
{
    if (!_ssl->basic_client)
        return 0;

    if (!_ssl->basic_client->connected())
        return 0;

    int res = _isSSL ? get_ssl_receive(_ssl, buf, size) : (int)_ssl->basic_client->read(buf, size);

    if (res < 0)
        stop();

    return res;
}

int MB_ESP32_TCPClient::available()
{
    if (!_ssl->basic_client)
        return 0;

    if (!_ssl->basic_client->connected())
        return 0;

    int res = _isSSL ? data_to_read(_ssl) : (int)_ssl->basic_client->available();

    if (res < 0)
        stop();

    return res;
}

uint8_t MB_ESP32_TCPClient::connected()
{

    if (!_ssl->basic_client)
        return 0;

    return _ssl->basic_client->connected();
}

void MB_ESP32_TCPClient::setInsecure()
{
    _CA_cert = NULL;
    _cert = NULL;
    _private_key = NULL;
    _pskIdent = NULL;
    _psKey = NULL;
    _use_insecure = true;
}

void MB_ESP32_TCPClient::enableSSL(bool enable)
{
    _isSSL = enable;
}

void MB_ESP32_TCPClient::setCACert(const char *rootCA)
{
    _CA_cert = rootCA;
}

void MB_ESP32_TCPClient::setCertificate(const char *client_ca)
{
    _cert = client_ca;
}

void MB_ESP32_TCPClient::setPrivateKey(const char *private_key)
{
    _private_key = private_key;
}

void MB_ESP32_TCPClient::setPreSharedKey(const char *pskIdent, const char *psKey)
{
    _pskIdent = pskIdent;
    _psKey = psKey;
}

bool MB_ESP32_TCPClient::verify(const char *fp, const char *domain_name)
{
    if (!_ssl)
        return false;

    return verify_ssl_fingerprint(_ssl, fp, domain_name);
}

char *MB_ESP32_TCPClient::_streamLoad(Stream &stream, size_t size)
{
    char *dest = (char *)malloc(size + 1);
    if (!dest)
    {
        return nullptr;
    }
    if (size != stream.readBytes(dest, size))
    {
        free(dest);
        dest = nullptr;
        return nullptr;
    }
    dest[size] = '\0';
    return dest;
}

bool MB_ESP32_TCPClient::loadCACert(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setCACert(dest);
        ret = true;
    }
    return ret;
}

bool MB_ESP32_TCPClient::loadCertificate(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setCertificate(dest);
        ret = true;
    }
    return ret;
}

bool MB_ESP32_TCPClient::loadPrivateKey(Stream &stream, size_t size)
{
    char *dest = _streamLoad(stream, size);
    bool ret = false;
    if (dest)
    {
        setPrivateKey(dest);
        ret = true;
    }
    return ret;
}

int MB_ESP32_TCPClient::lastError(char *buf, const size_t size)
{
    if (!_lastError)
    {
        return 0;
    }
    mbedtls_strerror(_lastError, buf, size);
    return _lastError;
}

void MB_ESP32_TCPClient::setHandshakeTimeout(unsigned long handshake_timeout)
{
    if (_ssl)
        _ssl->handshake_timeout = handshake_timeout * 1000;
}

void MB_ESP32_TCPClient::flush()
{
    if (!_ssl->basic_client)
        return;

    while (available() > 0)
        read();
}

#endif // ESP32

#endif // MB_ESP32_TCPClient_CPP