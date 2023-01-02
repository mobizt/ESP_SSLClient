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

#ifndef MB_BSSL_TCPClient_H
#define MB_BSSL_TCPClient_H

#include "BSSL_SSLClient_FS.h"

#if defined(USE_BSSL_SSL_ENGINE)

#include "MB_BSSL_SSL_Client.h"

using namespace BearSSL;

#define WCS_CLASS MB_BSSL_SSL_Client

class MB_BSSL_TCPClient : public WCS_CLASS
{
  friend class ESP8266_TCP_Client;

public:
  MB_BSSL_TCPClient();
  ~MB_BSSL_TCPClient();

  /**
   * Set the client.
   * @param client The pointer to Client interface.
   * @param enableSSL The ssl option; true for enable, false for disable.
   */
  void setClient(Client *client, bool enableSSL = true);

  /**
   * Connect to server.
   * @param name The server host name to connect.
   * @param port The server port to connecte.
   * @return 1 for success or 0 for error.
   */
  int connect(const char *name, uint16_t port) override;

  /**
   * Connect to server.
   * @param ip The IP address to connect.
   * @param port The server port to connecte.
   * @return 1 for success or 0 for error.
   */
  int connect(IPAddress ip, uint16_t port) override;

  /**
   * Get TCP connection status.
   * @return 1 for connected or 0 for not connected.
   */
  uint8_t connected() override;

  /**
   * Get available data size to read.
   * @return The avaiable data size.
   */
  int available() override;

  /**
   * The TCP data read function.
   * @return A byte data that was successfully read or -1 for error.
   */
  int read() override;

  /**
   * The TCP data read function.
   * @param buf The data buffer.
   * @param size The length of data that read.
   * @return The size of data that was successfully read or -1 for error.
   */
  int read(uint8_t *buf, size_t size) override;

  /**
   * The TCP data send function.
   * @param data The data to send.
   * @return The size of data that was successfully sent or 0 for error.
   */
  int send(const char *data);

  /**
   * The TCP data print function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int print(const char *data);

  /**
   * The TCP data print function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int print(const String &data);

  /**
   * The TCP data print function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int print(int data);

  /**
   * The TCP data print with new line function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int println(const char *data);

  /**
   * The TCP data print with new line function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int println(const String &data);

  /**
   * The TCP data print with new line function.
   * @param data The data to print.
   * @return The size of data that was successfully print or 0 for error.
   */
  int println(int data);

  /**
   * The TCP data write function.
   * @param buf The data to write.
   * @param size The length of data to write.
   * @return The size of data that was successfully written or 0 for error.
   */
  size_t write(const uint8_t *buf, size_t size) override;

  /**
   * The TCP data write function.
   * @param b The data to write.
   * @return 1 for success or 0 for error.
   */
  size_t write(uint8_t b) override { return write(&b, 1); }

  /**
   * The TCP data write function.
   * @param buf The PGM data to write.
   * @param size The length of data to write.
   * @return The size of data that was successfully written or 0 for error.
   */
  size_t write_P(PGM_P buf, size_t size) { return WCS_CLASS::write_P(buf, size); }

  /**
   * The TCP data write function.
   * @param buf The string data to write.
   * @return The size of data that was successfully written or 0 for error.
   */
  size_t write(const char *buf) { return write((const uint8_t *)buf, strlen(buf)); }

  /**
   * The TCP data write function.
   * @param stream The stream data to write.
   * @return The size of data that was successfully written or 0 for error.
   */
  size_t write(Stream &stream) { return WCS_CLASS::write(stream); }

  /**
   * Read one byte from stream.
   * @return The data that was successfully read or -1 for error.
   */
  int peek() override;

  /**
   * No certificate chain validation.
   */
  void setInsecure();

  /**
   * Enable/disable the SSL layer transport.
   * @param enable The enable option; true for enable, false to disable.
   */
  void enableSSL(bool enable);

  /**
   * Upgrade the current connection by setting up the SSL and perform the SSL handshake.
   *
   * @return operating result.
   */
  bool connectSSL();

  /**
   * Close the connection
   */
  void stop() override;

  /**
   * Set the timeout when waiting for an SSL response.
   * @param timeout The timeout value, in milliseconds.
   */
  void setTimeout(unsigned long timeout);

  /**
   * Force writing the buffered bytes from write to the network.
   */
  void flush() override;

  /**
   *  Sets the requested buffer size for transmit and receive
   *  @param recv The receive buffer size.
   *  @param xmit The transmit buffer size.
   */
  void setBufferSizes(int recv, int xmit);

  /**
   * Equivalent to connected() > 0
   *
   * @returns true if connected, false if not
   */
  operator bool() { return connected() > 0; }

  int availableForWrite();

  void setSession(BearSSL_Session *session);

  void setKnownKey(const PublicKey *pk, unsigned usages = BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN);

  bool setFingerprint(const uint8_t fingerprint[20]);

  bool setFingerprint(const char *fpStr);

  void allowSelfSignedCerts();

  void setTrustAnchors(const X509List *ta);

  void setX509Time(time_t now);

  void setClientRSACert(const X509List *cert, const PrivateKey *sk);

  void setClientECCert(const X509List *cert, const PrivateKey *sk, unsigned allowed_usages, unsigned cert_issuer_key_type);

  int getMFLNStatus();

  int getLastSSLError(char *dest = NULL, size_t len = 0);

  void setCertStore(CertStoreBase *certStore);

  bool setCiphers(const uint16_t *cipherAry, int cipherCount);

  bool setCiphers(const std::vector<uint16_t> &list);

  bool setCiphersLessSecure();

  bool setSSLVersion(uint32_t min = BR_TLS10, uint32_t max = BR_TLS12);

  bool probeMaxFragmentLength(IPAddress ip, uint16_t port, uint16_t len) { return WCS_CLASS::probeMaxFragmentLength(ip, port, len); };

  bool probeMaxFragmentLength(const char *hostname, uint16_t port, uint16_t len) { return WCS_CLASS::probeMaxFragmentLength(hostname, port, len); };

  bool probeMaxFragmentLength(const String &host, uint16_t port, uint16_t len) { return WCS_CLASS::probeMaxFragmentLength(host, port, len); };

#if defined(ESP8266)
  // peek buffer API is present
  bool hasPeekBufferAPI() const { return true; }

  // return number of byte accessible by peekBuffer()
  size_t peekAvailable() { return WCS_CLASS::available(); }

  // return a pointer to available data buffer (size = peekAvailable())
  // semantic forbids any kind of read() before calling peekConsume()
  const char *peekBuffer() { return WCS_CLASS::peekBuffer(); }

  // consume bytes after use (see peekBuffer)
  void peekConsume(size_t consume) { return WCS_CLASS::peekConsume(consume); }
#endif

  // ESP32 compatibility
  void setCACert(const char *rootCA) { return WCS_CLASS::setCACert(rootCA); }

  void setCertificate(const char *client_ca) { return WCS_CLASS::setCertificate(client_ca); }

  void setPrivateKey(const char *private_key) { return WCS_CLASS::setPrivateKey(private_key); }

  bool loadCACert(Stream &stream, size_t size) { return WCS_CLASS::loadCACert(stream, size); }

  bool loadCertificate(Stream &stream, size_t size) { return WCS_CLASS::loadCertificate(stream, size); }

  bool loadPrivateKey(Stream &stream, size_t size) { return WCS_CLASS::loadPrivateKey(stream, size); }

  int connect(IPAddress ip, uint16_t port, int32_t timeout) { return WCS_CLASS::connect(ip, port, timeout); }

  int connect(const char *host, uint16_t port, int32_t timeout) { return WCS_CLASS::connect(host, port, timeout); }

  int connect(IPAddress ip, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return WCS_CLASS::connect(ip, port, rootCABuff, cli_cert, cli_key); }

  int connect(const char *host, uint16_t port, const char *rootCABuff, const char *cli_cert, const char *cli_key) { return WCS_CLASS::connect(host, port, rootCABuff, cli_cert, cli_key); }

private:
  Client *_basic_client = nullptr;
  bool _isSSL = false;
  bool _ssl = false;
};

#endif

#endif /* MB_BSSL_TCPClient_H */
