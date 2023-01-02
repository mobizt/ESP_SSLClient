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

#ifndef MB_BSSL_TCPClient_CPP
#define MB_BSSL_TCPClient_CPP

#include "BSSL_SSLClient_FS.h"

#if defined(USE_BSSL_SSL_ENGINE)

#include "MB_BSSL_TCPClient.h"

MB_BSSL_TCPClient::MB_BSSL_TCPClient()
{
}

MB_BSSL_TCPClient::~MB_BSSL_TCPClient()
{
  stop();
  WCS_CLASS::setClient(nullptr);
  _basic_client = nullptr;
}

void MB_BSSL_TCPClient::setClient(Client *client, bool enableSSL)
{
  _basic_client = client;
  WCS_CLASS::setClient(client);
  _isSSL = enableSSL;
  _ssl = enableSSL;
}

int MB_BSSL_TCPClient::connect(const char *name, uint16_t port)
{

  if (!_basic_client)
    return 0;

  _isSSL = _ssl;

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

int MB_BSSL_TCPClient::connect(IPAddress ip, uint16_t port)
{
  if (!_basic_client)
    return 0;

  _isSSL = _ssl;

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

bool MB_BSSL_TCPClient::connectSSL()
{
  bool res =  WCS_CLASS::connectSSL();
  _isSSL = res;
  if (!res)
    WCS_CLASS::stop();
  return res;
}

uint8_t MB_BSSL_TCPClient::connected()
{
  return WCS_CLASS::connected();
}

void MB_BSSL_TCPClient::setTimeout(unsigned long timeout)
{
  WCS_CLASS::setTimeout(timeout);
}

void MB_BSSL_TCPClient::stop()
{
  WCS_CLASS::stop();
}

int MB_BSSL_TCPClient::available()
{
  return WCS_CLASS::available();
}

int MB_BSSL_TCPClient::read()
{
  return WCS_CLASS::read();
}

int MB_BSSL_TCPClient::read(uint8_t *buf, size_t size)
{
  return WCS_CLASS::read(buf, size);
}

int MB_BSSL_TCPClient::print(int data)
{
  char buf[64];
  memset(buf, 0, 64);
  sprintf(buf, (const char *)FPSTR("%d"), data);
  int ret = send(buf);
  return ret;
}

int MB_BSSL_TCPClient::print(const char *data)
{
  return send(data);
}

int MB_BSSL_TCPClient::print(const String &data)
{
  return print(data.c_str());
}

int MB_BSSL_TCPClient::println(const char *data)
{
  int len = send(data);
  if (len < 0)
    return len;
  int sz = send((const char *)FPSTR("\r\n"));
  if (sz < 0)
    return sz;
  return len + sz;
}

int MB_BSSL_TCPClient::println(const String &data)
{
  return println(data.c_str());
}

int MB_BSSL_TCPClient::println(int data)
{
  char buf[64];
  memset(buf, 0, 64);
  sprintf(buf, (const char *)FPSTR("%d\r\n"), data);
  int ret = send(buf);
  return ret;
}

int MB_BSSL_TCPClient::send(const char *data)
{
  return write((uint8_t *)data, strlen(data));
}

size_t MB_BSSL_TCPClient::write(const uint8_t *buf, size_t size)
{
  return WCS_CLASS::write(buf, size);
}

int MB_BSSL_TCPClient::peek()
{
  return WCS_CLASS::peek();
}

void MB_BSSL_TCPClient::setInsecure()
{
  WCS_CLASS::setInsecure();
}

void MB_BSSL_TCPClient::enableSSL(bool enable)
{
  _isSSL = enable;
}

void MB_BSSL_TCPClient::flush()
{
  WCS_CLASS::flush();
}

void MB_BSSL_TCPClient::setBufferSizes(int recv, int xmit)
{
  WCS_CLASS::setBufferSizes(recv, xmit);
}

int MB_BSSL_TCPClient::availableForWrite()
{
  return WCS_CLASS::availableForWrite();
}

void MB_BSSL_TCPClient::setSession(BearSSL_Session *session)
{
  WCS_CLASS::setSession(session);
}

void MB_BSSL_TCPClient::setKnownKey(const PublicKey *pk, unsigned usages)
{
  WCS_CLASS::setKnownKey(pk, usages);
}

bool MB_BSSL_TCPClient::setFingerprint(const uint8_t fingerprint[20])
{
  return WCS_CLASS::setFingerprint(fingerprint);
}

bool MB_BSSL_TCPClient::setFingerprint(const char *fpStr)
{
  return WCS_CLASS::setFingerprint(fpStr);
}

void MB_BSSL_TCPClient::allowSelfSignedCerts()
{
  WCS_CLASS::allowSelfSignedCerts();
}

void MB_BSSL_TCPClient::setTrustAnchors(const X509List *ta)
{
  WCS_CLASS::setTrustAnchors(ta);
}

void MB_BSSL_TCPClient::setX509Time(time_t now)
{
  WCS_CLASS::setX509Time(now);
}

void MB_BSSL_TCPClient::setClientRSACert(const X509List *cert, const PrivateKey *sk)
{
  WCS_CLASS::setClientRSACert(cert, sk);
}

void MB_BSSL_TCPClient::setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type)
{
  WCS_CLASS::setClientECCert(cert, sk, allowed_usages, cert_issuer_key_type);
}

int MB_BSSL_TCPClient::getMFLNStatus()
{
  return WCS_CLASS::getMFLNStatus();
}

int MB_BSSL_TCPClient::getLastSSLError(char *dest, size_t len)
{
  return WCS_CLASS::getLastSSLError(dest, len);
}

void MB_BSSL_TCPClient::setCertStore(CertStoreBase *certStore)
{
  WCS_CLASS::setCertStore(certStore);
}

bool MB_BSSL_TCPClient::setCiphers(const uint16_t *cipherAry, int cipherCount)
{
  return WCS_CLASS::setCiphers(cipherAry, cipherCount);
}

bool MB_BSSL_TCPClient::setCiphers(const std::vector<uint16_t> &list)
{
  return WCS_CLASS::setCiphers(list);
}

bool MB_BSSL_TCPClient::setCiphersLessSecure()
{
  return WCS_CLASS::setCiphersLessSecure();
}

bool MB_BSSL_TCPClient::setSSLVersion(uint32_t min, uint32_t max)
{
  return WCS_CLASS::setSSLVersion(min, max);
}

#endif

#endif /* MB_BSSL_TCPClient_CPP */