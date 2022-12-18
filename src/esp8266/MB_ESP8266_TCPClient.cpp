/**
 *
 * The Mobizt ESP8266 TCP Client Class, MB_ESP8266_TCPClient.cpp v1.0.1
 *
 * Created November 15, 2022
 *
 * The MIT License (MIT)
 * Copyright (c) 2022 K. Suwatchai (Mobizt)
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

#ifndef MB_ESP8266_TCPClient_CPP
#define MB_ESP8266_TCPClient_CPP

#ifdef ESP8266

#define LWIP_INTERNAL

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "MB_ESP8266_TCPClient.h"
#include <lwip/tcp.h>
#include <include/ClientContext.h>

MB_ESP8266_TCPClient::MB_ESP8266_TCPClient()
{
}

MB_ESP8266_TCPClient::~MB_ESP8266_TCPClient()
{
  stop();
  WCS_CLASS::setClient(nullptr);
  _basic_client = nullptr;
}

void MB_ESP8266_TCPClient::setClient(Client *client, bool enableSSL)
{
  _basic_client = client;
  WCS_CLASS::setClient(client);
  _isSSL = enableSSL;
}

int MB_ESP8266_TCPClient::connect(const char *name, uint16_t port)
{

  if (!_basic_client)
    return 0;

  _host = name;

  if (!connected())
  {
    if (!WCS_CLASS::connect(name, port))
    {
      WCS_CLASS::stop();
      return 0;
    }
  }

  if (_isSSL && !connectSSL())
    return 0;

  return 1;
}

int MB_ESP8266_TCPClient::connect(IPAddress ip, uint16_t port)
{
  if (!_basic_client)
    return 0;

  if (!connected())
  {
    if (!WCS_CLASS::connect(ip, port))
    {
      WCS_CLASS::stop();
      return 0;
    }
  }

  if (_isSSL && !connectSSL())
    return 0;

  return 1;
}

bool MB_ESP8266_TCPClient::connectSSL()
{

  bool res = _host.length() ? WCS_CLASS::_connectSSL(_host.c_str()) : WCS_CLASS::_connectSSL(nullptr);
  _isSSL = true;
  if (!res)
    WCS_CLASS::stop();

  return res;
}

uint8_t MB_ESP8266_TCPClient::connected()
{
  return WCS_CLASS::connected();
}

void MB_ESP8266_TCPClient::setTimeout(unsigned long timeout)
{
  WCS_CLASS::setTimeout(timeout);
}

void MB_ESP8266_TCPClient::stop()
{
  WCS_CLASS::stop();
}

int MB_ESP8266_TCPClient::available()
{
  return WCS_CLASS::available();
}

int MB_ESP8266_TCPClient::read()
{
  return WCS_CLASS::read();
}

int MB_ESP8266_TCPClient::read(uint8_t *buf, size_t size)
{
  return WCS_CLASS::read(buf, size);
}

size_t MB_ESP8266_TCPClient::write(const uint8_t *buf, size_t size)
{
  return WCS_CLASS::write(buf, size);
}

int MB_ESP8266_TCPClient::peek()
{
  return WCS_CLASS::peek();
}

void MB_ESP8266_TCPClient::setInsecure()
{
  WCS_CLASS::setInsecure();
}

void MB_ESP8266_TCPClient::enableSSL(bool enable)
{
  _isSSL = enable;
}

void MB_ESP8266_TCPClient::flush()
{
  WCS_CLASS::flush();
}

void MB_ESP8266_TCPClient::setBufferSizes(int recv, int xmit)
{
  WCS_CLASS::setBufferSizes(recv, xmit);
}

int MB_ESP8266_TCPClient::availableForWrite()
{
  return WCS_CLASS::availableForWrite();
}

void MB_ESP8266_TCPClient::setSession(BearSSL_Session *session)
{
  WCS_CLASS::setSession(session);
}

void MB_ESP8266_TCPClient::setKnownKey(const BearSSL_PublicKey *pk, unsigned usages)
{
  WCS_CLASS::setKnownKey(pk, usages);
}

bool MB_ESP8266_TCPClient::setFingerprint(const uint8_t fingerprint[20])
{
  return WCS_CLASS::setFingerprint(fingerprint);
}

bool MB_ESP8266_TCPClient::setFingerprint(const char *fpStr)
{
  return WCS_CLASS::setFingerprint(fpStr);
}

void MB_ESP8266_TCPClient::allowSelfSignedCerts()
{
  WCS_CLASS::allowSelfSignedCerts();
}

void MB_ESP8266_TCPClient::setTrustAnchors(const BearSSL_X509List *ta)
{
  WCS_CLASS::setTrustAnchors(ta);
}

void MB_ESP8266_TCPClient::setX509Time(time_t now)
{
  WCS_CLASS::setX509Time(now);
}

void MB_ESP8266_TCPClient::setClientRSACert(const BearSSL_X509List *cert, const BearSSL_PrivateKey *sk)
{
  WCS_CLASS::setClientRSACert(cert, sk);
}

void MB_ESP8266_TCPClient::setClientECCert(const BearSSL_X509List *cert, const BearSSL_PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type)
{
  WCS_CLASS::setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type);
}

int MB_ESP8266_TCPClient::getMFLNStatus()
{
  return WCS_CLASS::getMFLNStatus();
}

int MB_ESP8266_TCPClient::getLastSSLError(char *dest, size_t len)
{
  return WCS_CLASS::getLastSSLError(dest, len);
}

void MB_ESP8266_TCPClient::setCertStore(BearSSL_CertStoreBase *certStore)
{
  WCS_CLASS::setCertStore(certStore);
}

bool MB_ESP8266_TCPClient::setCiphers(const uint16_t *cipherAry, int cipherCount)
{
  return WCS_CLASS::setCiphers(cipherAry, cipherCount);
}

bool MB_ESP8266_TCPClient::setCiphers(const std::vector<uint16_t> &list)
{
  return WCS_CLASS::setCiphers(list);
}

bool MB_ESP8266_TCPClient::setCiphersLessSecure()
{
  return WCS_CLASS::setCiphersLessSecure();
}

bool MB_ESP8266_TCPClient::setSSLVersion(uint32_t min, uint32_t max)
{
  return WCS_CLASS::setSSLVersion(min, max);
}

const char *MB_ESP8266_TCPClient::peekBuffer()
{
  return WCS_CLASS::peekBuffer();
}

void MB_ESP8266_TCPClient::peekConsume(size_t consume)
{
  WCS_CLASS::peekConsume(consume);
}

#endif /* ESP8266 */

#endif /* MB_ESP8266_TCPClient_CPP */